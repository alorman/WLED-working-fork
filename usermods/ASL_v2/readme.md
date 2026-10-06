# ASL_v2 — WMATA Metro Map

Drives a physical LED map of the Washington DC Metro. All five lines (Red, Blue,
Green, Orange, Yellow) are rendered with stations as fixed dots and trains as
moving pixels. Train positions come from either:

- **Live mode**: the [WMATA TrainPositions API](https://developer.wmata.com/) (requires an API key), or
- **Sim mode**: an offline schedule simulator (a train departs every *headway*
  interval between the open and close times — every *rush hour headway* inside
  the morning/evening rush windows — and runs the line on a timetable built
  from stopwatch-measured station dwell and station-to-station travel times;
  see `timing/README.md`).

This is the modern self-contained port of the original `ASL_v2` usermod
(branch `asl-variable-station-sim-delays`), which required edits to `wled.h`,
`FX.cpp`, `FX.h` and `usermods_list.cpp`. This version touches **no core files**:
the five line effects are registered at runtime from `setup()` via
`strip.addEffect()`, and all shared state lives inside the usermod.

## How it renders

Every *Plot Refresh Interval* the usermod computes a per-line scenery frame
(one byte per LED: track / station) and reconciles the train data into a
sprite list: each train glides at constant speed from where it is currently
rendered to the fractional LED position of its latest data point, timed to
arrive when the next data point is due (plus 300 ms of slack), so it is
normally still moving when that point arrives:

- **Sim:** a new position every plot interval, so glides last one interval.
- **Live:** WMATA refreshes positions less often than the usermod polls, and
  only in whole circuits. A poll that returns the same snapshot as the last
  one is ignored (trains keep gliding); the time between real feed changes is
  measured and smoothed, and each train that moved glides to its new position
  over that measured interval (default 10 s until measured, capped at 30 s).
  The display therefore runs about one WMATA update behind, in exchange for
  continuous motion. Failed fetches leave trains in place; after 60 s without
  good data, live trains are cleared from the map.

Circuits flow through the pipeline
as floats: the sim interpolates exact fractional positions from its timetable
(continuous motion, no inchworming between circuit boundaries), while live
data is naturally quantized to WMATA's integer circuits. The five registered
effects ("ASL Red Line" … "ASL Yellow Line") paint scenery + sprites on every
strip refresh, resolving actual colors from the segment's color slots at draw
time:

| Segment color slot | Meaning |
|---|---|
| Fx (1st) | train |
| Bg (2nd) | station |
| Cs (3rd) | track (background) |

Element brightness is set through the color itself (the picker's value
slider); segment opacity dims a whole line, and global brightness applies on
top of everything.

A train normally occupies one LED. Trains move slowly on this map (often
several seconds per LED), so blending across the whole gap between two LEDs
would show most trains as a two-LED smear. Instead the **Train Crossfade**
setting (default 0.25) sets a handover window, as a fraction of one LED,
centred on the midpoint between LEDs: outside it the train sits on a single
LED; inside it the train crossfades to the next LED. 0 = the train hops to
the nearest LED with no blending; 1 = full two-LED blending all the way (a
train at LED 47.4 lights LED 47 at 60% and LED 48 at 40%). Blend weights go
through a perceptual inverse-gamma curve (the **Gamma** setting, default 2.2)
so apparent brightness holds during the handover. Because the window is a
distance, a handover takes longer on slow stretches than on fast ones.

New trains fade in over WLED's own **Transition Time** (Config → LED
Preferences, default 750 ms, capped at 5 s for trains), so there is one
place to tune how soft changes are on the device; 0 makes trains pop in and
out.
A train missing from the data fades out: immediately in sim mode (it has
finished its run), after one plot cycle of grace in live mode (live data
routinely drops a train for one fetch). Moves larger than `ASL_TELEPORT_LEDS` (8, compile-time) dissolve out+in
instead of gliding — junk or reacquired API data, or a turnback at a terminal
(direction is part of the sprite identity). UI transitions are left to the
WLED core, and color changes apply instantly without waiting for the next
plot cycle.

## Setup

1. Build with this usermod (see below) and flash.
2. Create one segment per line, sized/mapped to that line's LEDs, or simply
   upload the tracked `presets.json` (see *Presets* below). Reference build
   strip order: Status LED (1), Blue (298), Green (181), Red (267),
   Orange (209), Yellow (185) = 1141 LEDs.
3. Assign each segment its "ASL … Line" effect and set the three colors.
   The effects always register at the same IDs, so presets keep working
   across builds and build flags: Red **142**, Blue **169**, Green **170**,
   Orange **171**, Yellow **220**, Status **221** (`"fx"` in presets). 142 and
   169–171 are the only slots no stock effect uses; 220/221 are appended
   after WLED's effect table. If another usermod that appends effects is
   added, check the IDs (a debug build logs any mismatch).
   Optionally add a 1-LED segment for a status LED with the "ASL - Status LED"
   effect (see *Status LED* below).
4. In Config → Usermods, set either **Enable Train Sim Mode**, or your WMATA
   **API Key** for live data. Live mode also needs NTP time and Wi-Fi.

### Presets

`presets.json` in this folder is the source of truth for the device's
presets (1 = *Trains and Dim Stations*, 2 = *Only Trains*; both include the
status LED). Firmware updates (OTA) never touch presets, which live in the
device's filesystem, so push them separately:

```bash
curl -F "data=@presets.json;filename=presets.json" http://<wled-ip>/upload
```

The upload **replaces all presets on the device**: save presets changed on
the device back into this file (download `http://<wled-ip>/presets.json`)
before uploading. Uploads are refused while a settings PIN is locked.

### Usermod settings

| Setting | Default | Notes |
|---|---|---|
| Enable Train Sim Mode | on | off = fetch live WMATA data |
| Server Address | WMATA TrainPositions URL | `api_key` is appended automatically |
| API Key | *(empty)* | live mode does nothing without it; spaces and line breaks are removed automatically on save |
| Plot Refresh Interval (s) | 15 | data refresh + glide duration, in seconds (decimals ok, minimum 1). **Live mode: 15 or more recommended** — WMATA refreshes each train's position only about every 15 s, so polling faster makes trains glide and then wait for the next update. Sim mode works well at any value. Replaces the old "Plot Refresh Interval (ms)" setting |
| System Open/Close Time | 05:00 / 00:00 | HH:MM time pickers; first/last train departure (sim), defaults match WMATA weekday hours (5am–midnight). Close earlier than open wraps past midnight (e.g. 22:00–02:00); equal times = 24-hour service |
| Train Headway | 6 | minutes between departures, decimals ok (sim); stored as seconds internally |
| Rush Hour Train Headway | 4 | minutes between departures inside rush windows (0 = no rush service). Applies at the terminals, so the density wave sweeps down each line at travel speed. Defaults match WMATA's published FY2026 peak service |
| Morning Rush Hour Start/End | 07:00 / 09:00 | rush window (sim); same-day only, start at/after end disables it |
| Evening Rush Hour Start/End | 16:00 / 18:00 | rush window (sim); same-day only, start at/after end disables it |
| Fallback Station Dwell (s) | 25 | whole seconds; sim dwell at the terminals and at stations with no measured dwell (blank or 0 = 25, max 600). Measured stations use their own times. Replaces the old "Station Dwell Time (seconds)" setting |
| Train Crossfade | 0.25 | LED-to-LED handover window as a fraction of one LED: 0 = hop to the nearest LED, 1 = full two-LED blend (clamped 0–1) |
| Hop Below Brightness | 12 | below this shown brightness (0–255: global brightness × segment opacity) trains hop instead of crossfading, because at very low brightness the LEDs have too few output levels for a smooth crossfade; 0 = never |
| Gamma | 2.2 | brightness curve of the crossfade (1 = linear, clamped 1–4) |

Open/close times are stored in `cfg.json` as `"HH:MM"` strings and headway as
minutes. Missing or invalid entries fall back to the defaults above (defined
once as `DEF_*` constants in the usermod).

Saving the settings page rebuilds the sim timetables immediately; no reboot
is needed.

### Clock and sim test time

The sim runs on WLED's clock, so it is only as right as that clock. Under
**Enable Train Sim Mode** the settings page shows:

- **Clock** — current local time and where it came from (NTP, RTC,
  browser / API, or *not set*). The same line appears in the Info panel as
  *ASL clock*. *Not set* means the sim is drawing the wrong schedule.
- **Set clock from this device** — sets WLED's clock from the browser's (the
  same mechanism the main UI uses on every command). With the RTC usermod
  built, the new time is written to the RTC chip within a second.
- **Sim test time** + **Apply** / **Real time** — runs the *sim only* as if it
  were the chosen time, then keeps ticking from there (e.g. view rush hour at
  2 pm). It is stored as an offset in RAM: never saved to `cfg.json`, cleared
  by a reboot, and the real clock and RTC chip are never changed. Shown in the
  Info panel as *ASL sim time* while active. Also available over the JSON API:
  `{"ASL":{"simAt":"14:30"}}` and `{"ASL":{"simReset":true}}` to `/json/state`.
- **Sim / live switch** over the JSON API: `{"ASL":{"sim":true}}` or
  `{"ASL":{"sim":false}}` to `/json/state` (the Metro Map page's *Trains*
  toggle). Saved like the *Enable Train Sim Mode* setting; trains of the old
  mode fade out and the map refills from the new source at once.
- **Sim timetable** over the JSON API (the Metro Map page's *Sim timetable*
  section): `GET /asl/sim` returns the timetable settings (without the rest of
  `cfg.json`, which holds the API key), and
  `{"ASL":{"simCfg":{"open":"05:00","close":"00:30","headway":6,"rushHeadway":4,"amStart":"06:00","amEnd":"09:00","pmStart":"15:00","pmEnd":"18:30","dwell":25}}}`
  to `/json/state` changes any subset of them. Validated like the settings
  page (malformed times keep the current value, headway must be above 0, rush
  headway 0 = no rush service, dwell 1-600 s) and saved to `cfg.json`.
- **Live feed settings** over the JSON API (the Metro Map page's *Live feed*
  section): `GET /asl/feed` returns the server address, whether an API key is
  set, the key itself only while WLED's settings are unlocked (no settings PIN,
  or the PIN has been entered - the same rule as WLED's own settings pages),
  and whether the PIN lock is on. `{"ASL":{"feed":{"server":"http://...","key":"..."}}}`
  to `/json/state` changes either one (server must start with `http://` or
  `https://`; whitespace is stripped from the key) and saves to `cfg.json`.
  Because the key is sent to whatever server is set, this command follows
  WLED's settings protection: if a settings PIN is set, it is ignored until the
  PIN has been entered (WLED Settings).

### Metro Map web page

`web/metro.htm` is a custom landing page: a live map (trains and stations,
WMATA line colours), power, brightness, preset buttons, per-line on/off and
brightness, sim test time / set clock, the Info panel, and links to the WLED
controls and settings.

```bash
curl -F "data=@web/metro.htm;filename=metro.htm" http://<wled-ip>/upload
# then open http://<wled-ip>/metro.htm
```

For development, open the file straight from disk with `?ip=<wled-ip>`
(remembered afterwards); WLED's API allows cross-origin requests. Like
`presets.json`, the page lives in the device filesystem, so a full flash
erase removes it.

The live map reads `GET /asl/live` from this usermod: per line the LED
count and station LEDs, plus every visible train as
`[line, pos, alpha, id, cars, "dest", nonRev]`, where `pos` is the fractional
LED exactly as drawn. Hovering or tapping a train shows its line, destination,
train ID and car count (live mode; the sim has none of these). Trains not in
passenger service are never shown on the LEDs, but the page draws them as
hollow grey rings. The endpoint is only called by the page, so this costs
nothing while the page is closed. Destinations show as WMATA station codes
unless `web/wmata_stations.json` (the response of WMATA's
`Rail.svc/json/jStations`, saved as a file) is present when
`gen_map_coords.py` runs; then they show as names. The map is geographic:
every LED's position comes from the board's LED numbering PDF
(`web/LED-Numbering.pdf`, one label per LED, e.g. `RD0`..`RD266`).
`web/gen_map_coords.py` (standard library, run by hand) reads the labels'
positions out of the PDF, checks every line is complete, and writes them
into the page's `COORDS` line. It also copies the board's grey artwork
(outline, rivers, county lines, track corridors) into the `BACKDROP` line,
which the map draws in translucent white behind the LEDs (strength set by
`BACKDROP_OPACITY` in the page), and the station bullseyes from that artwork
into the `BULLSEYES` line. Stations are not drawn on top of the artwork; each
bullseye is an invisible hover target showing the station's name, plus its
lines at transfer stations. Rerun it if the board layout changes. Without
coordinates the page falls back to a schematic one-bar-per-line diagram.

The artwork makes the page about 180 KB. To save flash and load time, gzip it
and upload `metro.htm.gz` in place of `metro.htm`; WLED serves the compressed
file automatically.

### Info panel

The WLED Info page shows these rows (mode-specific rows only in that mode):

| Row | Shows |
|---|---|
| ASL trains | trains in the current data (sim or live) |
| ASL clock | local time and where it came from (NTP, RTC, browser / API, not set) |
| ASL UTC offset | offset actually applied (timezone + DST + manual offset) |
| ASL NTP sync | when NTP last succeeded, or disabled / not yet |
| ASL RTC | not built in / off (no I2C pins) / OK, set the clock at boot / chip found but its time was not set / chip not responding on I2C (pins, wiring or unsupported chip — DS3231/DS1307 only) |
| ASL status LED | what the status LED currently means, in words |
| ASL effect IDs | OK, or a warning if an effect missed its fixed ID (presets would need updating) |
| ASL service *(sim)* | in service (AM/PM rush, current headway) or next departure time |
| ASL run times *(sim)* | end-to-end run time per line from the timing tables |
| ASL sim time *(sim)* | the test time, while one is active |
| ASL feed refresh *(live)* | measured WMATA refresh interval |
| ASL last fetch *(live)* | OK (and how long it blocked), HTTP error, no connection, bad data, no API key, no Wi-Fi |
| ASL feed trains *(live)* | trains in the feed vs drawn, off-map, other lines (e.g. Silver), non-revenue |
| ASL trains per line *(live)* | trains drawn per line |
| ASL data age *(live)* | how long ago positions last changed |
| ASL API calls *(live)* | WMATA requests since boot |

### Status LED

A sixth effect, **ASL - Status LED** (listed first among the ASL effects), shows the map's health on a status LED (on
the reference build: the ESP32-S3 board's onboard RGB LED on GPIO48, added in
LED Preferences as its own 1-LED strip). Put a segment over that LED and
assign the effect. It uses fixed colors and lets WLED do the dimming, so
global brightness and segment opacity apply as for the train lines.

| Status LED | Meaning |
|---|---|
| red, slow blink | no Wi-Fi **and** no working RTC (missing, or never set): the clock cannot be trusted |
| amber, fast blink | clock not set yet, or live mode without working data (no API key, no Wi-Fi, last fetch failed) |
| purple | sim running on a **test time** (see above), not real time |
| green | live mode, WMATA data arriving |
| cyan | sim on real time kept by the RTC (offline, as intended) |
| blue | sim on real time from NTP / network / browser |

"Working RTC" means the RTC set the clock during this boot; it stays counted
after a later NTP or browser sync.

For offline use (no internet, so no NTP), add the stock RTC usermod
(DS1307/DS3231 on I2C) next to this one — `custom_usermods = ASL_v2 RTC` —
and set the global I2C SDA/SCL pins at the top of Config → Usermods. The RTC
stores UTC, so WLED's timezone/DST rules (Config → Time & Macros) still apply
offline.

## How the sim times a run

Each line's run alternates *dwell at station 0, travel to station 1, dwell at
station 1, …* using the per-station and per-domain seconds in
`asl_timing_data.h`. A dwelling train sits on its station dot; a moving train
is interpolated evenly across that domain's LEDs. Track2 runs the same
timeline in reverse. Values that were never measured fall back to the
**Fallback Station Dwell** setting (dwell) or to the old placeholder speed of
3 s per track circuit (travel).

Track1 direction per line (derived from the circuit numbering; details in
`asl_map_data.h`): Red Shady Grove→Glenmont, Blue Franconia→Downtown Largo,
Green Branch Ave→Greenbelt, Orange Vienna→New Carrollton, Yellow
Huntington→Fort Totten. Potomac Yard and the Silver line are not on this
hardware revision.

## Files

- `usermod_v2_asl.cpp` — usermod class, the five line effects and the status LED effect
- `asl_map_data.h` — static circuit→LED mapping tables (flash-resident)
- `asl_timing_data.h` — sim timing tables, **generated** from `timing/`
- `timing/` — measured ride times (`segment_times.csv`), the original
  workbook, `gen_timing.py`, which regenerates `asl_timing_data.h`, and
  `check_map.py`, which checks the map and timing tables (both run by hand;
  not part of the build)
- `presets.json` — the device's presets (source of truth; see *Presets*)

## Building

Add to your `platformio_override.ini` environment:

```ini
custom_usermods = ASL_v2
```

ESP32 only (live mode uses `HTTPClient`).

## Notes / current limitations

- The live HTTP fetch is synchronous; effects pause briefly during each poll.
- Trains with `ServiceType != "Normal"` are ignored, matching the original.
- Unmeasured stretches still use placeholders: Red Metro Center→Gallery Place,
  Yellow Huntington→Eisenhower Ave→King St (travel at 3 s/circuit, which is
  faster than reality), and the dwell at Judiciary Sq, Brookland, Eisenhower
  Ave and every terminal (fallback setting).
- Map data still unresolved: the Stadium-Armory platform circuit (Orange says
  1443, Blue 1461; measured times suggest neither). Only affects where live
  trains dwelling there are drawn. Details and the fixes already made are in
  the comments in `asl_map_data.h`; `timing/check_map.py` re-checks the tables.
