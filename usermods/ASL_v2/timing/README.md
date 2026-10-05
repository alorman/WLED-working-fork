# ASL_v2 sim timing data

Measured WMATA Metrorail ride times that drive the offline train simulator.

## Files

| File | Role |
|---|---|
| `metrorail-timing-workbook-merged.xlsx` | Original stopwatch workbook (template generated with Claude; times are hand-measured on real rides). Kept as the raw record. |
| `segment_times.csv` | **Source of truth.** Same data as the workbook, one row per station or segment, plus the exclusion decisions. Edit this when you add rides. |
| `gen_timing.py` | Helper that turns the CSV into `../asl_timing_data.h`. Standard library only, run by hand, **not** part of the firmware build. |

After editing the CSV:

```
python usermods/ASL_v2/timing/gen_timing.py
```

then build as usual and commit both the CSV and the regenerated header.

## Source and accuracy

All values are stopwatch measurements from riding the lines (1–2 passes per
row so far, a handful of rows still blank). They are not WMATA's published
schedule. WMATA's GTFS feed has scheduled stop-to-stop times if an
authoritative cross-check is ever wanted.

Each line was ridden in one direction only. Track2 reuses the same times in
reverse, which is a reasonable approximation.

## CSV columns

| Column | Meaning |
|---|---|
| `line` | Workbook tab (Red, Orange, Blue, Green, Yellow, Silver) |
| `seq` | Row order within the line, in riding order |
| `kind` | `station` = dwell, doors open to doors closed. `segment` = travel, doors closed to doors open at the next station |
| `name` | Station name. For a segment, the station it arrives at |
| `pass1`–`pass3` | Stopwatch digits exactly as displayed: `135` = 1 min 35 s. Seconds must be 00–59 |
| `exclude` | Space-separated pass numbers to ignore |
| `note` | Why a pass was excluded, "same track as …" markers, etc. |

## Processing rules (implemented in `gen_timing.py`)

- **Pooling:** each station, and each station-to-station segment, is pooled
  across every tab that rides it (shared track), then averaged. A pass set
  that exactly duplicates another tab's row is treated as a copy and counted
  once.
- **Exclusions** are explicit in the `exclude` column; the script applies no
  automatic outlier rule. Current exclusions (approved 2026-10-05):
  - Red: Rockville dwell pass 2, Farragut North dwell pass 1, Wheaton→Glenmont pass 1
  - Orange: Stadium-Armory→Minnesota Ave pass 1 (keep 4:23), Minnesota Ave dwell
    pass 1 (keep 0:23), Federal Center SW dwell, Capitol South→Eastern Market,
    Eastern Market dwell
  - Blue/Yellow: pass 2 of Potomac Yard→Reagan, Reagan dwell, Reagan→Crystal City
    and Crystal City dwell (lap pressed in the wrong row / held train)
  - Blue: Arlington Cemetery dwell pass 2, Stadium-Armory→Benning Rd pass 2
- **Not on this hardware:** the Silver tab is ignored. Potomac Yard is folded
  into Braddock Rd→Reagan National as travel in + dwell + travel out.
- **Yellow** runs Huntington→Fort Totten on this hardware. North of Mt Vernon
  Sq it uses the Green tab's measurements.
- **Missing values** are written as 0. The firmware then uses the *Fallback
  Station Dwell* setting for dwell and the old 3 s/circuit placeholder speed
  for travel.

## Track direction

The firmware's tables are in Track1 order. That matches the riding order on
every tab except Green: Green Track1 runs Branch Ave→Greenbelt, so the script
reverses it. The reasoning is documented at the top of `../asl_map_data.h`
and in the per-line comments there.
