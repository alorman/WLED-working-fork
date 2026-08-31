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

Every *Plot Refresh Interval* the usermod computes a per-line "meaning frame"
(one byte per LED: track / train / station). The five registered effects
("ASL Red Line" … "ASL Yellow Line") paint that frame on every strip refresh,
resolving actual colors from the segment's color slots at draw time:

| Segment color slot | Meaning |
|---|---|
| Fx (1st) | track (background) |
| Bg (2nd) | train |
| Cs (3rd) | station |

The old hand-rolled per-pixel crossfade is gone — transitions are left to the
WLED core, and color changes in the UI apply instantly without waiting for the
next plot cycle.

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
| System Open/Close Time | 00:00 / 22:00 | HH:MM time pickers; first/last train departure (sim). Same-day only — close must be after open or both reset to defaults |
| Train Headway | 6 | minutes between departures, decimals ok (sim); stored as seconds internally |
| Station Dwell Time (s) | 10 | time at each station (sim) |
| Plot Refresh Interval (ms) | 5000 | keep ≥ 3500 or WMATA will get angry |

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
