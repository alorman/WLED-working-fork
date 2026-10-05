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
sprite list: each train glides from where it is currently rendered to the
fractional LED position of its latest data point over one plot interval,
easing out of and into stops (smoothstep). Circuits flow through the pipeline
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

Trains render with two-LED anti-aliasing: a sprite at LED 47.4 lights LED 47
at 60% and LED 48 at 40% coverage, with weights boosted through a perceptual
inverse-gamma curve (the **Gamma** setting, default 2.2) so apparent
brightness stays constant mid-glide. New trains fade in over the **Fade
Milliseconds** setting (default 400 ms), vanished trains get one plot cycle
of grace (live data routinely drops a train for one fetch) then fade out, and
moves larger than `ASL_TELEPORT_LEDS` (8, compile-time) dissolve out+in
instead of gliding — junk or reacquired API data, or a turnback at a terminal
(direction is part of the sprite identity). UI transitions are left to the
WLED core, and color changes apply instantly without waiting for the next
plot cycle.

## Setup

1. Build with this usermod (see below) and flash.
2. Create one segment per line, sized/mapped to that line's LEDs
   (see `presets-example.json` for the original segment layout; Red=267 LEDs,
   Blue=298, Green=181, Orange=208, Yellow=184 in the reference build).
3. Assign each segment its "ASL … Line" effect and set the three colors.
4. In Config → Usermods, set either **Enable Train Sim Mode**, or your WMATA
   **API Key** for live data. Live mode also needs NTP time and Wi-Fi.

### Usermod settings

| Setting | Default | Notes |
|---|---|---|
| Enable Train Sim Mode | on | off = fetch live WMATA data |
| Server Address | WMATA TrainPositions URL | `api_key` is appended automatically |
| API Key | *(empty)* | live mode does nothing without it |
| System Open/Close Time | 05:00 / 00:00 | HH:MM time pickers; first/last train departure (sim), defaults match WMATA weekday hours (5am–midnight). Close earlier than open wraps past midnight (e.g. 22:00–02:00); equal times = 24-hour service |
| Train Headway | 6 | minutes between departures, decimals ok (sim); stored as seconds internally |
| Morning Rush Hour Start/End | 07:00 / 09:00 | rush window (sim); same-day only, start at/after end disables it |
| Evening Rush Hour Start/End | 16:00 / 18:00 | rush window (sim); same-day only, start at/after end disables it |
| Rush Hour Train Headway | 4 | minutes between departures inside rush windows (0 = no rush service). Applies at the terminals, so the density wave sweeps down each line at travel speed. Defaults match WMATA's published FY2026 peak service |
| Fallback Station Dwell (s) | 25 | whole seconds; sim dwell at the terminals and at stations with no measured dwell (blank or 0 = 25, max 600). Measured stations use their own times. Replaces the old "Station Dwell Time (seconds)" setting |
| Plot Refresh Interval (ms) | 5000 | data refresh + glide duration; keep ≥ 3500 in live mode or WMATA will get angry |
| Fade Milliseconds | 400 | train appear/vanish fade (0 = instant, clamped to 5000) |
| Gamma | 2.2 | motion anti-alias brightness curve (1 = linear, clamped 1–4) |

Open/close times are stored in `cfg.json` as `"HH:MM"` strings and headway as
minutes. Missing or invalid entries fall back to the defaults above (defined
once as `DEF_*` constants in the usermod).

Saving the settings page rebuilds the sim timetables immediately; no reboot
is needed.

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

- `usermod_v2_asl.cpp` — usermod class + the five line effects
- `asl_map_data.h` — static circuit→LED mapping tables (flash-resident)
- `asl_timing_data.h` — sim timing tables, **generated** from `timing/`
- `timing/` — measured ride times (`segment_times.csv`), the original
  workbook, `gen_timing.py`, which regenerates `asl_timing_data.h`, and
  `check_map.py`, which checks the map and timing tables (both run by hand;
  not part of the build)
- `presets-example.json` — the original 5-segment preset; upload to the device
  filesystem (`/edit`) as `presets.json` or recreate segments manually

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
