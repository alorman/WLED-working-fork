# ASL_v2 — WMATA Metro Map

Drives a physical LED map of the Washington DC Metro. All five lines (Red, Blue,
Green, Orange, Yellow) are rendered with stations as fixed dots and trains as
moving pixels. Train positions come from either:

- **Live mode**: the [WMATA TrainPositions API](https://developer.wmata.com/) (requires an API key), or
- **Sim mode**: an offline schedule simulator (a train departs every *headway*
  interval between the open and close times and moves along a per-segment
  cumulative timetable).

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
easing out of and into stops (smoothstep). The five registered effects
("ASL Red Line" … "ASL Yellow Line") paint scenery + sprites on every strip
refresh, resolving actual colors from the segment's color slots at draw time:

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
| System Open/Close Time | 05:00 / 22:00 | HH:MM time pickers; first/last train departure (sim). Same-day only — close must be after open or both reset to defaults |
| Train Headway | 6 | minutes between departures, decimals ok (sim); stored as seconds internally |
| Station Dwell Time (seconds) | 10 | time at each station (sim) |
| Plot Refresh Interval (ms) | 5000 | data refresh + glide duration; keep ≥ 3500 in live mode or WMATA will get angry |
| Fade Milliseconds | 400 | train appear/vanish fade (0 = instant, clamped to 5000) |
| Gamma | 2.2 | motion anti-alias brightness curve (1 = linear, clamped 1–4) |

Open/close times are stored in `cfg.json` as `"HH:MM"` strings and headway as
minutes. Missing or invalid entries fall back to the defaults above (defined
once as `DEF_*` constants in the usermod).

Changing timing settings re-racks the computed delay tables automatically.

## Files

- `usermod_v2_asl.cpp` — usermod class + the five line effects
- `asl_map_data.h` — static circuit→LED mapping tables (flash-resident)
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
- `calculateAdditiveDelays()` (runtime-generated "re-rackable" timetable) is
  computed for Red line track 1 only and not yet consumed by the simulator,
  which still runs on the static `*AdditiveDelaySegments` tables.
- Trains with `ServiceType != "Normal"` are ignored, matching the original.
