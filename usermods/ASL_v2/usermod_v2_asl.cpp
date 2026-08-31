#include "wled.h"
#ifdef ESP32
#include <HTTPClient.h>
#endif
#include "asl_map_data.h"

/*
 * ASL_v2 — WMATA metro map usermod (modern self-contained port).
 *
 * Renders the five DC Metro lines on an LED map. Train positions come either
 * from the live WMATA TrainPositions API or from an offline schedule simulator.
 *
 * Architecture:
 *  - The usermod periodically plots a per-line scenery frame (one byte per
 *    LED: track or station) and reconciles the train table into a sprite
 *    list: each train glides from where it is rendered to the fractional LED
 *    of its latest data point over one plot interval, easing in/out of stops.
 *  - Five registered effects ("ASL Red Line" etc.) draw scenery + sprites on
 *    every strip refresh, resolving colors live from the segment's color
 *    slots. Sprites render with two-LED anti-aliasing through a perceptual
 *    (inverse-gamma) weight curve so apparent brightness stays constant
 *    mid-glide. New trains fade in, vanished trains fade out after one cycle
 *    of grace, and implausible jumps (junk/reacquired API data) dissolve
 *    out+in instead of gliding across the map. UI transitions are left to
 *    the WLED core.
 *
 * Setup: create one segment per line sized to that line's LED count, assign
 * the matching ASL effect, and set the three segment colors:
 *   Fx (1st) = track, Bg (2nd) = train, Cs (3rd) = station.
 * Element brightness is part of the picked color (the picker's value slider);
 * segment opacity dims a whole line and global brightness applies on top.
 */

// pixel meaning codes; value doubles as the SEGCOLOR() slot index
#define ASL_PX_TRACK   0
#define ASL_PX_TRAIN   1   // color slot only — trains render as moving sprites, not frame pixels
#define ASL_PX_STATION 2

// line indices into aslLines[]; order matches the effect registration order
#define ASL_LINE_RED    0
#define ASL_LINE_BLUE   1
#define ASL_LINE_GREEN  2
#define ASL_LINE_ORANGE 3
#define ASL_LINE_YELLOW 4

// per-line scenery frames, track + stations only (written by the usermod,
// read by the effects)
static uint8_t RedFrame[Red_Num_LEDS + 1];
static uint8_t BlueFrame[Blue_Num_LEDS + 1];
static uint8_t GreenFrame[Green_Num_LEDS + 1];
static uint8_t OrangeFrame[Orange_Num_LEDS + 1];
static uint8_t YellowFrame[Yellow_Num_LEDS + 1];

// ---- per-line map-data descriptor (indices match ASL_LINE_*) ----
struct AslLineDef {
  char code[3];                    // WMATA line code
  const uint16_t (*domains[2])[2]; // per-track circuit domains
  const uint16_t *stationSegs[2];  // per-track station circuits
  const uint16_t (*ledArray)[2];   // domain -> LED range (shared by both tracks)
  const uint16_t *stationLEDPos;
  uint16_t numStations;
  uint16_t numDomains;
};

static const AslLineDef aslLines[5] = {
  { "RD", { RedLineTrack1Domains,    RedLineTrack2Domains    }, { RedLineTrack1StationSegments,    RedLineTrack2StationSegments    }, RedLineLEDArray,    RedLineStationLEDPosition,    RedLineNumStationsInLine,    Red_Num_LED_Domains    },
  { "BL", { BlueLineTrack1Domains,   BlueLineTrack2Domains   }, { BlueLineTrack1StationSegments,   BlueLineTrack2StationSegments   }, BlueLineLEDArray,   BlueLineStationLEDPosition,   BlueLineNumStationsInLine,   Blue_Num_LED_Domains   },
  { "GR", { GreenLineTrack1Domains,  GreenLineTrack2Domains  }, { GreenLineTrack1StationSegments,  GreenLineTrack2StationSegments  }, GreenLineLEDArray,  GreenLineStationLEDPosition,  GreenLineNumStationsInLine,  Green_Num_LED_Domains  },
  { "OR", { OrangeLineTrack1Domains, OrangeLineTrack2Domains }, { OrangeLineTrack1StationSegments, OrangeLineTrack2StationSegments }, OrangeLineLEDArray, OrangeLineStationLEDPosition, OrangeLineNumStationsInLine, Orange_Num_LED_Domains },
  { "YL", { YellowLineTrack1Domains, YellowLineTrack2Domains }, { YellowLineTrack1StationSegments, YellowLineTrack2StationSegments }, YellowLineLEDArray, YellowLineStationLEDPosition, YellowLineNumStationsInLine, Yellow_Num_LED_Domains },
};

// ---- moving-train sprites ----
// Trains are not baked into the frames; each is a sprite gliding toward the
// fractional LED position of its latest data point, re-targeted every plot
// cycle and rendered with two-LED anti-aliasing at the strip frame rate.
#define ASL_MAX_SPRITES   128   // active trains (<= MAX_TRAINS) + fading ghosts
#define ASL_FADE_MS       400   // spawn/despawn fade; also each half of a dissolve
#define ASL_TELEPORT_LEDS 8.0f  // moves larger than this dissolve instead of gliding
#define ASL_AA_GAMMA      2.2f  // perceptual boost exponent for coverage weights

enum : uint8_t { ASL_SPR_FREE = 0, ASL_SPR_LIVE = 1, ASL_SPR_GHOST = 2 };

struct AslSprite {
  uint8_t  mode;         // ASL_SPR_*
  uint8_t  lineIdx;      // ASL_LINE_*
  uint8_t  dir;          // track/direction number, part of the identity key
  uint8_t  missed;       // consecutive plot cycles without a data point
  bool     seen;         // matched during the current reconciliation pass
  uint32_t id;           // train ID (live: WMATA TrainId; sim: departure index + 1)
  float    startPos;     // glide origin (fractional LED)
  float    targetPos;    // glide destination (fractional LED)
  uint32_t moveStartMs;
  uint32_t moveDurMs;    // 0 = parked at targetPos
  uint32_t fadeStartMs;  // spawn (fade-in) or ghost (fade-out) start
};

static AslSprite aslSprites[ASL_MAX_SPRITES];
static uint8_t aslPerceptLUT[256]; // coverage -> blend amount, filled in setup()

// eased (smoothstep) sprite position: accelerates away from a stop, brakes
// into the next one
static float aslSpritePos(const AslSprite& s, uint32_t nowMs) {
  if (s.moveDurMs == 0) return s.targetPos;
  uint32_t el = nowMs - s.moveStartMs;
  if (el >= s.moveDurMs) return s.targetPos;
  float t = (float)el / (float)s.moveDurMs;
  t = t * t * (3.0f - 2.0f * t);
  return s.startPos + (s.targetPos - s.startPos) * t;
}

// current fade alpha (0-255): rises after spawn, falls after ghosting
static uint8_t aslSpriteAlpha(const AslSprite& s, uint32_t nowMs) {
  uint32_t el = nowMs - s.fadeStartMs;
  uint32_t a  = (el >= ASL_FADE_MS) ? 255 : (el * 255) / ASL_FADE_MS;
  return (s.mode == ASL_SPR_GHOST) ? (uint8_t)(255 - a) : (uint8_t)a;
}

// fractional LED for one track's data point: station snap (+/-1 circuit)
// first, then linear interpolation within the containing domain — the float
// analogue of the old integer mapRound() plot
static bool aslTargetForTrack(const AslLineDef& L, uint8_t track, uint32_t circuit, float& out) {
  const uint16_t* segs = L.stationSegs[track];
  for (uint16_t x = 0; x < L.numStations; x++) {
    if (circuit + 1 >= segs[x] && circuit <= (uint32_t)segs[x] + 1) {
      out = (float)L.stationLEDPos[x];
      return true;
    }
  }
  const uint16_t (*dom)[2] = L.domains[track];
  for (uint16_t y = 0; y < L.numDomains; y++) {
    if (circuit >= dom[y][0] && circuit <= dom[y][1]) {
      uint16_t c0 = dom[y][0], c1 = dom[y][1];
      uint16_t l0 = L.ledArray[y][0], l1 = L.ledArray[y][1];
      out = (c1 > c0) ? (float)l0 + (float)(circuit - c0) * (float)(l1 - l0) / (float)(c1 - c0)
                      : 0.5f * (float)(l0 + l1);
      return true;
    }
  }
  return false;
}

static bool aslTargetLED(uint8_t lineIdx, uint32_t circuit, float& out) {
  const AslLineDef& L = aslLines[lineIdx];
  return aslTargetForTrack(L, 0, circuit, out) || aslTargetForTrack(L, 1, circuit, out);
}

static int8_t aslLineIndex(const char* code) {
  for (uint8_t i = 0; i < 5; i++) if (strncmp(code, aslLines[i].code, 2) == 0) return (int8_t)i;
  return -1;
}

static AslSprite* aslFindSprite(uint8_t lineIdx, uint8_t dir, uint32_t id) {
  for (auto &s : aslSprites)
    if (s.mode == ASL_SPR_LIVE && s.lineIdx == lineIdx && s.dir == dir && s.id == id) return &s;
  return nullptr;
}

static void aslSpawnSprite(uint8_t lineIdx, uint8_t dir, uint32_t id, float pos, uint32_t nowMs) {
  for (auto &s : aslSprites) {
    if (s.mode != ASL_SPR_FREE) continue;
    s = { ASL_SPR_LIVE, lineIdx, dir, 0, true, id, pos, pos, nowMs, 0, nowMs };
    return;
  } // table full: the train simply appears on a later cycle
}

static void aslGhostSprite(AslSprite& s, uint32_t nowMs) {
  s.startPos = s.targetPos = aslSpritePos(s, nowMs); // freeze in place while fading out
  s.moveDurMs   = 0;
  s.mode        = ASL_SPR_GHOST;
  s.fadeStartMs = nowMs;
  s.seen        = false;
}

// ---- effects: paint scenery from the frame, then the train sprites on top ----
static void aslDrawFrame(const uint8_t* frame, uint16_t frameLen, uint8_t lineIdx) {
  const int len = SEGLEN;
  for (int i = 0; i < len; i++) {
    uint8_t m = (i < frameLen) ? frame[i] : ASL_PX_TRACK;
    SEGMENT.setPixelColor(i, SEGCOLOR(m));
  }

  const uint32_t trainC = SEGCOLOR(ASL_PX_TRAIN);
  const uint32_t nowMs  = millis();
  for (unsigned n = 0; n < ASL_MAX_SPRITES; n++) {
    const AslSprite& s = aslSprites[n];
    if (s.mode == ASL_SPR_FREE || s.lineIdx != lineIdx) continue;
    uint8_t alpha = aslSpriteAlpha(s, nowMs);
    if (alpha == 0) continue;
    float pos  = aslSpritePos(s, nowMs);
    int   i0   = (int)floorf(pos);
    float frac = pos - (float)i0;
    // split coverage * fade alpha across the two straddled LEDs, boosted
    // through the perceptual LUT so apparent brightness holds mid-glide
    unsigned w0 = (unsigned)((1.0f - frac) * alpha + 0.5f);
    unsigned w1 = (unsigned)(frac * alpha + 0.5f);
    if (w0 > 0 && i0 >= 0 && i0 < len)
      SEGMENT.setPixelColor(i0, color_blend(SEGMENT.getPixelColor(i0), trainC, aslPerceptLUT[w0 > 255 ? 255 : w0]));
    if (w1 > 0 && i0 + 1 >= 0 && i0 + 1 < len)
      SEGMENT.setPixelColor(i0 + 1, color_blend(SEGMENT.getPixelColor(i0 + 1), trainC, aslPerceptLUT[w1 > 255 ? 255 : w1]));
  }
}

static void mode_asl_red(void)    { aslDrawFrame(RedFrame,    sizeof(RedFrame),    ASL_LINE_RED);    }
static void mode_asl_blue(void)   { aslDrawFrame(BlueFrame,   sizeof(BlueFrame),   ASL_LINE_BLUE);   }
static void mode_asl_green(void)  { aslDrawFrame(GreenFrame,  sizeof(GreenFrame),  ASL_LINE_GREEN);  }
static void mode_asl_orange(void) { aslDrawFrame(OrangeFrame, sizeof(OrangeFrame), ASL_LINE_ORANGE); }
static void mode_asl_yellow(void) { aslDrawFrame(YellowFrame, sizeof(YellowFrame), ASL_LINE_YELLOW); }

static const char _data_FX_ASL_RED[]    PROGMEM = "ASL Red Line@;Track,Train,Station;;1";
static const char _data_FX_ASL_BLUE[]   PROGMEM = "ASL Blue Line@;Track,Train,Station;;1";
static const char _data_FX_ASL_GREEN[]  PROGMEM = "ASL Green Line@;Track,Train,Station;;1";
static const char _data_FX_ASL_ORANGE[] PROGMEM = "ASL Orange Line@;Track,Train,Station;;1";
static const char _data_FX_ASL_YELLOW[] PROGMEM = "ASL Yellow Line@;Track,Train,Station;;1";


class UsermodASL : public Usermod {
  private:
    static const uint16_t MAX_TRAINS = 100;

    // factory defaults — single source of truth: they initialize the members
    // below, back every readFromConfig fallback, and thus pre-populate the
    // settings boxes on a device with no saved config
    static const uint32_t DEF_OPEN_S     = 18000;  // 05:00
    static const uint32_t DEF_CLOSE_S    = 79200;  // 22:00
    static const uint32_t DEF_HEADWAY_S  = 360;    // 6 min
    static const uint32_t DEF_DWELL_S    = 10;
    static const uint32_t DEF_REFRESH_MS = 5000;

    bool enabled = true;
    bool initDone = false;
    unsigned long lastTime = 0;

    // ---- config (usermod settings page) ----
    // times are stored internally as seconds; in cfg.json open/close are "HH:MM"
    // strings and headway is minutes (decimals ok), converted in read/addToConfig
    bool     simModeEnable = true;
    String   serverAddress = "http://api.wmata.com/TrainPositions/TrainPositions?contentType=json";
    String   apiKey = "";
    uint32_t systemFirstTrainTime = DEF_OPEN_S;    // second of day the first train departs
    uint32_t systemLastTrainTime  = DEF_CLOSE_S;   // second of day the last train departs
    uint32_t headwayTimeSeconds   = DEF_HEADWAY_S; // seconds between train departures
    uint32_t stationDwellTimeS    = DEF_DWELL_S;   // sim: time each train sits at a station
    uint32_t plotRefreshIntervalMs = DEF_REFRESH_MS; // data refresh; below ~3.5s angers WMATA servers

    // ---- runtime state ----
    uint32_t secondOfDay = 0;
    bool     delaysRacked = false;          // re-rackable delay table computed?

    // train table (compact; one entry per active train this cycle)
    uint16_t numTrains = 0;
    uint32_t trainId[MAX_TRAINS];
    uint32_t trainCircuit[MAX_TRAINS];
    uint8_t  trainDirection[MAX_TRAINS];
    uint8_t  trainCars[MAX_TRAINS];
    uint8_t  trainSecondsAtLoc[MAX_TRAINS];
    char     trainLine[MAX_TRAINS][3];
    bool     trainNormal[MAX_TRAINS];

    static const char _name[];
    static const char _enabled[];

    // "HH:MM" -> second of day; returns fallback on malformed/out-of-range input
    static uint32_t parseHHMM(const char* s, uint32_t fallback) {
      int h = -1, m = -1;
      if (!s || sscanf(s, "%d:%d", &h, &m) != 2) return fallback;
      if (h < 0 || h > 23 || m < 0 || m > 59) return fallback;
      return h * 3600UL + m * 60UL;
    }

    static void formatHHMM(uint32_t secOfDay, char* buf, size_t len) {
      snprintf(buf, len, "%02u:%02u", (unsigned)(secOfDay / 3600UL) % 24, (unsigned)((secOfDay / 60UL) % 60));
    }

    void clearTrains() { numTrains = 0; }

    void injectSimTrain(uint16_t simIdx, uint8_t dir, uint16_t circuit, const char* lineCode) {
      if (numTrains >= MAX_TRAINS) return;
      uint16_t i = numTrains++;
      trainId[i]           = simIdx + 1;
      trainCircuit[i]      = circuit;
      trainDirection[i]    = dir;
      trainCars[i]         = 8;
      trainSecondsAtLoc[i] = 2;
      strlcpy(trainLine[i], lineCode, sizeof(trainLine[i]));
      trainNormal[i]       = true;
    }

    // simulate all trains currently en route on one track of one line:
    // a train departs every headway between open and close; its position is the
    // segment whose cumulative arrival time brackets "now - departure".
    void offlineSimTrains(const char* lineCode, uint8_t dir,
                          const uint16_t trackSegs[], uint16_t segCount,
                          const uint16_t addDelay[], uint16_t delayCount) {
      if (delayCount == 0 || headwayTimeSeconds == 0) return;
      uint32_t opDuration  = systemLastTrainTime - systemFirstTrainTime;
      uint32_t numSimTrains = opDuration / headwayTimeSeconds + 1;
      uint32_t runDuration = addDelay[delayCount - 1]; // total end-to-end run time
      uint16_t lastSeg = (segCount < delayCount) ? segCount : delayCount;

      for (uint32_t i = 0; i < numSimTrains; i++) {
        uint32_t departure = systemFirstTrainTime + i * headwayTimeSeconds;
        if (secondOfDay <= departure || secondOfDay >= departure + runDuration) continue;
        uint32_t t = secondOfDay - departure; // seconds into this train's run
        for (uint16_t y = 0; y < lastSeg; y++) {
          if (t <= addDelay[y] && (y == 0 || t > addDelay[y - 1])) {
            injectSimTrain(i, dir, trackSegs[y], lineCode);
            break;
          }
        }
      }
    }

    // WIP "re-rackable" delay table: build the cumulative timetable at runtime
    // from per-domain travel times + dwell time instead of hand-authored data.
    // Red line track 1 only until validated; sim still runs on the static tables.
    void calculateAdditiveDelays() {
      if (delaysRacked) return;
      const uint16_t segCount    = RedLineTrack1SegmentCount;
      const uint16_t numStations = RedLineNumStationsInLine;
      const uint16_t numDomains  = sizeof(RedLineTrack1DelayDomainsS) / sizeof(RedLineTrack1DelayDomainsS[0]);
      uint16_t stationsPassed = 0;
      for (uint16_t i = 0; i < segCount; i++) {
        bool isStation = false;
        for (uint16_t x = 0; x < numStations; x++) {
          if (RedLineTrack1Segments[i] == RedLineTrack1StationSegments[x]) { isStation = true; break; }
        }
        if (isStation) {
          RedLineTrack1NewAdditiveDelaySegs[i] = stationDwellTimeS;
          stationsPassed++;
        } else if (stationsPassed > 0 && stationsPassed <= numDomains) {
          // spread the domain's travel time evenly across its non-station segments
          RedLineTrack1NewAdditiveDelaySegs[i] =
            (RedLineTrack1DelayDomainsS[stationsPassed - 1] - stationDwellTimeS) / RedLineTrack1DelaySegTotal[stationsPassed - 1];
        } else {
          RedLineTrack1NewAdditiveDelaySegs[i] = 0;
        }
      }
      for (uint16_t i = 1; i < segCount; i++) {
        RedLineTrack1NewAdditiveDelaySegs[i] += RedLineTrack1NewAdditiveDelaySegs[i - 1];
      }
      delaysRacked = true;
      DEBUG_PRINTF_P(PSTR("ASL: racked delays, RD1 run duration %u s\n"), RedLineTrack1NewAdditiveDelaySegs[segCount - 1]);
    }

    // fetch live train positions from the WMATA API into the train table
    void getWMATAData() {
#ifdef ESP32
      if (apiKey.length() == 0) return;
      HTTPClient http;
      http.setConnectTimeout(3000);
      http.setTimeout(5000);
      String url = serverAddress + "&api_key=" + apiKey;
      if (!http.begin(url)) return;
      int code = http.GET();
      if (code != HTTP_CODE_OK) {
        DEBUG_PRINTF_P(PSTR("ASL: WMATA HTTP error %d\n"), code);
        http.end();
        return;
      }
      String payload = http.getString();
      http.end();

      DynamicJsonDocument doc(24576);
      DeserializationError err = deserializeJson(doc, payload);
      if (err) {
        DEBUG_PRINTF_P(PSTR("ASL: JSON parse failed: %s\n"), err.c_str());
        return;
      }
      JsonArray positions = doc["TrainPositions"];
      uint16_t n = positions.size();
      if (n > MAX_TRAINS) n = MAX_TRAINS;
      for (uint16_t i = 0; i < n; i++) {
        JsonObject t = positions[i];
        trainId[i]           = t["TrainId"] | 0UL;
        trainCircuit[i]      = t["CircuitId"] | 0UL;
        trainDirection[i]    = t["DirectionNum"] | 0;
        trainCars[i]         = t["CarCount"] | 0;
        trainSecondsAtLoc[i] = t["SecondsAtLocation"] | 0;
        strlcpy(trainLine[i], t["LineCode"] | "", sizeof(trainLine[i]));
        trainNormal[i]       = (strcmp(t["ServiceType"] | "", "Normal") == 0);
      }
      numTrains = n;
      DEBUG_PRINTF_P(PSTR("ASL: fetched %u trains\n"), numTrains);
#endif
    }

    void clearFrame(uint8_t frame[], uint16_t frameLen) {
      memset(frame, ASL_PX_TRACK, frameLen);
    }

    void plotStations(uint8_t frame[], uint16_t frameLen, const uint16_t stationLEDPos[], uint16_t numStations) {
      for (uint16_t i = 0; i < numStations; i++) {
        if (stationLEDPos[i] < frameLen) frame[stationLEDPos[i]] = ASL_PX_STATION;
      }
    }

    // reconcile the train table into the sprite list: matched trains re-target
    // (gliding there over one plot interval), new trains fade in, vanished
    // trains get one cycle of grace then fade out, and implausible jumps
    // dissolve out+in instead of gliding (junk/reacquired API data)
    void updateTrainSprites(uint32_t nowMs) {
      for (auto &s : aslSprites) {
        if (s.mode == ASL_SPR_GHOST && nowMs - s.fadeStartMs >= ASL_FADE_MS) s.mode = ASL_SPR_FREE;
        s.seen = false;
      }
      for (uint16_t i = 0; i < numTrains; i++) {
        if (!trainNormal[i]) continue;
        int8_t li = aslLineIndex(trainLine[i]);
        if (li < 0) continue;
        float target;
        if (!aslTargetLED(li, trainCircuit[i], target)) continue; // circuit outside the mapped domains
        AslSprite* s = aslFindSprite(li, trainDirection[i], trainId[i]);
        if (!s) {
          aslSpawnSprite(li, trainDirection[i], trainId[i], target, nowMs);
          continue;
        }
        s->seen = true;
        s->missed = 0;
        float cur = aslSpritePos(*s, nowMs);
        if (fabsf(target - cur) <= ASL_TELEPORT_LEDS) {
          s->startPos    = cur;
          s->targetPos   = target;
          s->moveStartMs = nowMs;
          s->moveDurMs   = plotRefreshIntervalMs;
        } else {
          aslGhostSprite(*s, nowMs);
          aslSpawnSprite(li, trainDirection[i], trainId[i], target, nowMs);
        }
      }
      for (auto &s : aslSprites) {
        if (s.mode == ASL_SPR_LIVE && !s.seen && ++s.missed > 1) aslGhostSprite(s, nowMs);
      }
    }

    void plotScenery() {
      clearFrame(RedFrame, sizeof(RedFrame));
      plotStations(RedFrame, sizeof(RedFrame), RedLineStationLEDPosition, RedLineNumStationsInLine);
      clearFrame(BlueFrame, sizeof(BlueFrame));
      plotStations(BlueFrame, sizeof(BlueFrame), BlueLineStationLEDPosition, BlueLineNumStationsInLine);
      clearFrame(GreenFrame, sizeof(GreenFrame));
      plotStations(GreenFrame, sizeof(GreenFrame), GreenLineStationLEDPosition, GreenLineNumStationsInLine);
      clearFrame(OrangeFrame, sizeof(OrangeFrame));
      plotStations(OrangeFrame, sizeof(OrangeFrame), OrangeLineStationLEDPosition, OrangeLineNumStationsInLine);
      clearFrame(YellowFrame, sizeof(YellowFrame));
      plotStations(YellowFrame, sizeof(YellowFrame), YellowLineStationLEDPosition, YellowLineNumStationsInLine);
    }

  public:
    void setup() override {
      // perceptual anti-alias curve: boost fractional coverage by 1/gamma so a
      // train split across two LEDs reads as bright as one fully lit LED
      for (int i = 0; i < 256; i++)
        aslPerceptLUT[i] = (uint8_t)(powf((float)i / 255.0f, 1.0f / ASL_AA_GAMMA) * 255.0f + 0.5f);
      strip.addEffect(255, &mode_asl_red,    _data_FX_ASL_RED);
      strip.addEffect(255, &mode_asl_blue,   _data_FX_ASL_BLUE);
      strip.addEffect(255, &mode_asl_green,  _data_FX_ASL_GREEN);
      strip.addEffect(255, &mode_asl_orange, _data_FX_ASL_ORANGE);
      strip.addEffect(255, &mode_asl_yellow, _data_FX_ASL_YELLOW);
      initDone = true;
    }

    void loop() override {
      if (!enabled || (strip.isUpdating() && (millis() - lastTime < plotRefreshIntervalMs + 200))) return;
      if (millis() - lastTime < plotRefreshIntervalMs) return;
      lastTime = millis();

      secondOfDay = hour(localTime) * 3600UL + minute(localTime) * 60UL + second(localTime);

      calculateAdditiveDelays();
      clearTrains();
      if (simModeEnable) {
        offlineSimTrains("RD", 1, RedLineTrack1Segments, RedLineTrack1SegmentCount, RedLineTrack1AdditiveDelaySegments, RedLineTrack1AdditiveDelaySegmentsCount);
        offlineSimTrains("RD", 2, RedLineTrack2Segments, RedLineTrack2SegmentCount, RedLineTrack2AdditiveDelaySegments, RedLineTrack2AdditiveDelaySegmentsCount);
        offlineSimTrains("YL", 1, YellowLineTrack1Segments, YellowLineTrack1SegmentCount, YellowLineTrack1AdditiveDelaySegments, YellowLineTrack1AdditiveDelaySegmentsCount);
        offlineSimTrains("YL", 2, YellowLineTrack2Segments, YellowLineTrack2SegmentCount, YellowLineTrack2AdditiveDelaySegments, YellowLineTrack2AdditiveDelaySegmentsCount);
        offlineSimTrains("GR", 1, GreenLineTrack1Segments, GreenLineTrack1SegmentCount, GreenLineTrack1AdditiveDelaySegments, GreenLineTrack1AdditiveDelaySegmentsCount);
        offlineSimTrains("GR", 2, GreenLineTrack2Segments, GreenLineTrack2SegmentCount, GreenLineTrack2AdditiveDelaySegments, GreenLineTrack2AdditiveDelaySegmentsCount);
        offlineSimTrains("BL", 1, BlueLineTrack1Segments, BlueLineTrack1SegmentCount, BlueLineTrack1AdditiveDelaySegments, BlueLineTrack1AdditiveDelaySegmentsCount);
        offlineSimTrains("BL", 2, BlueLineTrack2Segments, BlueLineTrack2SegmentCount, BlueLineTrack2AdditiveDelaySegments, BlueLineTrack2AdditiveDelaySegmentsCount);
        offlineSimTrains("OR", 1, OrangeLineTrack1Segments, OrangeLineTrack1SegmentCount, OrangeLineTrack1AdditiveDelaySegments, OrangeLineTrack1AdditiveDelaySegmentsCount);
        offlineSimTrains("OR", 2, OrangeLineTrack2Segments, OrangeLineTrack2SegmentCount, OrangeLineTrack2AdditiveDelaySegments, OrangeLineTrack2AdditiveDelaySegmentsCount);
      } else if (WLED_CONNECTED) {
        getWMATAData();
      }
      plotScenery();
      updateTrainSprites(millis());
    }

    void addToJsonInfo(JsonObject& root) override {
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");
      JsonArray arr = user.createNestedArray(F("ASL trains"));
      arr.add(numTrains);
      arr.add(simModeEnable ? F(" (sim)") : F(" (live)"));
    }

    void addToConfig(JsonObject& root) override {
      JsonObject top = root.createNestedObject(FPSTR(_name));
      top[FPSTR(_enabled)]                = enabled;
      top[F("Enable Train Sim Mode")]     = simModeEnable;
      top[F("Server Address")]            = serverAddress;
      top[F("API Key")]                   = apiKey;
      char hhmm[6];
      formatHHMM(systemFirstTrainTime, hhmm, sizeof(hhmm));
      top[F("System Open Time")]          = hhmm;
      formatHHMM(systemLastTrainTime, hhmm, sizeof(hhmm));
      top[F("System Close Time")]         = hhmm;
      top[F("Train Headway")]             = headwayTimeSeconds / 60.0f;
      top[F("Station Dwell Time (seconds)")] = stationDwellTimeS;
      top[F("Plot Refresh Interval (ms)")] = plotRefreshIntervalMs;
    }

    bool readFromConfig(JsonObject& root) override {
      JsonObject top = root[FPSTR(_name)];
      bool configComplete = !top.isNull();

      configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled, true);
      configComplete &= getJsonValue(top[F("Enable Train Sim Mode")], simModeEnable, true);
      configComplete &= getJsonValue(top[F("Server Address")], serverAddress);
      configComplete &= getJsonValue(top[F("API Key")], apiKey);

      String hhmm;
      configComplete &= getJsonValue(top[F("System Open Time")], hhmm, "");
      systemFirstTrainTime = parseHHMM(hhmm.c_str(), DEF_OPEN_S);
      configComplete &= getJsonValue(top[F("System Close Time")], hhmm, "");
      systemLastTrainTime  = parseHHMM(hhmm.c_str(), DEF_CLOSE_S);

      float headwayMin = 0.0f;
      configComplete &= getJsonValue(top[F("Train Headway")], headwayMin, 0.0f);
      headwayTimeSeconds = (headwayMin > 0.0f) ? (uint32_t)(headwayMin * 60.0f + 0.5f) : 0;
      if (headwayTimeSeconds == 0) headwayTimeSeconds = DEF_HEADWAY_S; // missing, non-positive, or rounds to zero

      configComplete &= getJsonValue(top[F("Station Dwell Time (seconds)")], stationDwellTimeS, DEF_DWELL_S);
      configComplete &= getJsonValue(top[F("Plot Refresh Interval (ms)")], plotRefreshIntervalMs, DEF_REFRESH_MS);
      if (plotRefreshIntervalMs < 1000) plotRefreshIntervalMs = 1000;

      // same-day service only: close must be after open
      if (systemLastTrainTime <= systemFirstTrainTime) {
        systemFirstTrainTime = DEF_OPEN_S;
        systemLastTrainTime  = DEF_CLOSE_S;
      }

      delaysRacked = false; // timing settings may have changed; re-rack on next loop
      return configComplete;
    }

    void appendConfigData(Print& settingsScript) override {
      // upgrade the open/close text fields to native HH:MM time pickers;
      // [0] of each named pair is the hidden type field, [1] the visible input
      settingsScript.printf_P(PSTR(
        "for(let n of['System Open Time','System Close Time']){"
          "let f=d.getElementsByName('%s:'+n);"
          "if(f[1]){f[1].type='time';f[1].style.width='120px';}}"), _name);
      settingsScript.printf_P(PSTR("addInfo('%s:Train Headway',1,'minutes between departures (decimals ok)');"), _name);
      settingsScript.printf_P(PSTR("addInfo('%s:API Key',1,'WMATA key, only used in live mode');"), _name);
    }

    uint16_t getId() override { return USERMOD_ID_UNSPECIFIED; }
};

const char UsermodASL::_name[]    PROGMEM = "ASL-usermod-v2";
const char UsermodASL::_enabled[] PROGMEM = "enabled";

static UsermodASL asl_usermod;
REGISTER_USERMOD(asl_usermod);
