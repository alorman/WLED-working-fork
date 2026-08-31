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
 *  - The usermod periodically plots a "meaning frame" per line: one byte per
 *    LED holding which segment color slot to show (0=track, 1=train, 2=station).
 *  - Five registered effects ("ASL Red Line" etc.) draw those frames each
 *    strip refresh, resolving colors live from the segment's color slots, so
 *    color changes in the UI apply instantly. Transitions/blending are left to
 *    the WLED core.
 *
 * Setup: create one segment per line sized to that line's LED count, assign
 * the matching ASL effect, and set the three segment colors:
 *   Fx (1st) = track, Bg (2nd) = train, Cs (3rd) = station.
 */

// pixel meaning codes; value doubles as the SEGCOLOR() slot index
#define ASL_PX_TRACK   0
#define ASL_PX_TRAIN   1
#define ASL_PX_STATION 2

// per-line meaning frames (written by the usermod, read by the effects)
static uint8_t RedFrame[Red_Num_LEDS + 1];
static uint8_t BlueFrame[Blue_Num_LEDS + 1];
static uint8_t GreenFrame[Green_Num_LEDS + 1];
static uint8_t OrangeFrame[Orange_Num_LEDS + 1];
static uint8_t YellowFrame[Yellow_Num_LEDS + 1];

// ---- effects: paint the current meaning frame, colors from segment slots ----
static void aslDrawFrame(const uint8_t* frame, uint16_t frameLen) {
  const int len = SEGLEN;
  for (int i = 0; i < len; i++) {
    uint8_t m = (i < frameLen) ? frame[i] : ASL_PX_TRACK;
    SEGMENT.setPixelColor(i, SEGCOLOR(m));
  }
}

static void mode_asl_red(void)    { aslDrawFrame(RedFrame,    sizeof(RedFrame)); }
static void mode_asl_blue(void)   { aslDrawFrame(BlueFrame,   sizeof(BlueFrame)); }
static void mode_asl_green(void)  { aslDrawFrame(GreenFrame,  sizeof(GreenFrame)); }
static void mode_asl_orange(void) { aslDrawFrame(OrangeFrame, sizeof(OrangeFrame)); }
static void mode_asl_yellow(void) { aslDrawFrame(YellowFrame, sizeof(YellowFrame)); }

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
    static const uint32_t DEF_OPEN_S     = 0;      // 00:00
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

    // rounded linear map of a circuit ID within its domain onto an LED range
    static int mapRound(int x, int in_min, int in_max, int out_min, int out_max) {
      return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
    }

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

    // place every matching train of one track onto the line's frame:
    // trains at (or within +/-1 of) a station circuit snap to the station LED,
    // otherwise the circuit is mapped proportionally within its domain.
    void plotTrains(uint8_t frame[], uint16_t frameLen, const char* lineCode,
                    const uint16_t trackDomains[][2], const uint16_t stationSegs[],
                    uint16_t numStations, const uint16_t ledArray[][2],
                    const uint16_t stationLEDPos[], uint16_t numDomains) {
      for (uint16_t i = 0; i < numTrains; i++) {
        if (!trainNormal[i] || strncmp(trainLine[i], lineCode, 2) != 0) continue;
        bool plotted = false;
        for (uint16_t x = 0; x < numStations; x++) {
          if (trainCircuit[i] >= (uint32_t)stationSegs[x] - 1 && trainCircuit[i] <= (uint32_t)stationSegs[x] + 1) {
            if (stationLEDPos[x] < frameLen) frame[stationLEDPos[x]] = ASL_PX_TRAIN;
            plotted = true;
            break;
          }
        }
        if (plotted) continue;
        for (uint16_t y = 0; y < numDomains; y++) {
          if (trainCircuit[i] >= trackDomains[y][0] && trainCircuit[i] <= trackDomains[y][1]) {
            int led = mapRound(trainCircuit[i], trackDomains[y][0], trackDomains[y][1], ledArray[y][0], ledArray[y][1]);
            if (led >= 0 && led < frameLen) frame[led] = ASL_PX_TRAIN;
            break;
          }
        }
      }
    }

    void plotAllLines() {
      clearFrame(RedFrame, sizeof(RedFrame));
      plotStations(RedFrame, sizeof(RedFrame), RedLineStationLEDPosition, RedLineNumStationsInLine);
      plotTrains(RedFrame, sizeof(RedFrame), "RD", RedLineTrack1Domains, RedLineTrack1StationSegments, RedLineNumStationsInLine, RedLineLEDArray, RedLineStationLEDPosition, Red_Num_LED_Domains);
      plotTrains(RedFrame, sizeof(RedFrame), "RD", RedLineTrack2Domains, RedLineTrack2StationSegments, RedLineNumStationsInLine, RedLineLEDArray, RedLineStationLEDPosition, Red_Num_LED_Domains);

      clearFrame(BlueFrame, sizeof(BlueFrame));
      plotStations(BlueFrame, sizeof(BlueFrame), BlueLineStationLEDPosition, BlueLineNumStationsInLine);
      plotTrains(BlueFrame, sizeof(BlueFrame), "BL", BlueLineTrack1Domains, BlueLineTrack1StationSegments, BlueLineNumStationsInLine, BlueLineLEDArray, BlueLineStationLEDPosition, Blue_Num_LED_Domains);
      plotTrains(BlueFrame, sizeof(BlueFrame), "BL", BlueLineTrack2Domains, BlueLineTrack2StationSegments, BlueLineNumStationsInLine, BlueLineLEDArray, BlueLineStationLEDPosition, Blue_Num_LED_Domains);

      clearFrame(GreenFrame, sizeof(GreenFrame));
      plotStations(GreenFrame, sizeof(GreenFrame), GreenLineStationLEDPosition, GreenLineNumStationsInLine);
      plotTrains(GreenFrame, sizeof(GreenFrame), "GR", GreenLineTrack1Domains, GreenLineTrack1StationSegments, GreenLineNumStationsInLine, GreenLineLEDArray, GreenLineStationLEDPosition, Green_Num_LED_Domains);
      plotTrains(GreenFrame, sizeof(GreenFrame), "GR", GreenLineTrack2Domains, GreenLineTrack2StationSegments, GreenLineNumStationsInLine, GreenLineLEDArray, GreenLineStationLEDPosition, Green_Num_LED_Domains);

      clearFrame(OrangeFrame, sizeof(OrangeFrame));
      plotStations(OrangeFrame, sizeof(OrangeFrame), OrangeLineStationLEDPosition, OrangeLineNumStationsInLine);
      plotTrains(OrangeFrame, sizeof(OrangeFrame), "OR", OrangeLineTrack1Domains, OrangeLineTrack1StationSegments, OrangeLineNumStationsInLine, OrangeLineLEDArray, OrangeLineStationLEDPosition, Orange_Num_LED_Domains);
      plotTrains(OrangeFrame, sizeof(OrangeFrame), "OR", OrangeLineTrack2Domains, OrangeLineTrack2StationSegments, OrangeLineNumStationsInLine, OrangeLineLEDArray, OrangeLineStationLEDPosition, Orange_Num_LED_Domains);

      clearFrame(YellowFrame, sizeof(YellowFrame));
      plotStations(YellowFrame, sizeof(YellowFrame), YellowLineStationLEDPosition, YellowLineNumStationsInLine);
      plotTrains(YellowFrame, sizeof(YellowFrame), "YL", YellowLineTrack1Domains, YellowLineTrack1StationSegments, YellowLineNumStationsInLine, YellowLineLEDArray, YellowLineStationLEDPosition, Yellow_Num_LED_Domains);
      plotTrains(YellowFrame, sizeof(YellowFrame), "YL", YellowLineTrack2Domains, YellowLineTrack2StationSegments, YellowLineNumStationsInLine, YellowLineLEDArray, YellowLineStationLEDPosition, Yellow_Num_LED_Domains);
    }

  public:
    void setup() override {
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
      plotAllLines();
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
      top[F("Station Dwell Time (s)")]    = stationDwellTimeS;
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

      configComplete &= getJsonValue(top[F("Station Dwell Time (s)")], stationDwellTimeS, DEF_DWELL_S);
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
