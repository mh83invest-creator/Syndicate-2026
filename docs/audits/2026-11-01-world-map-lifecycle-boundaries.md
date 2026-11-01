# Syndicate 2026 — World-map lifecycle and drawing boundaries

Date: 2026-11-01. Documentation-only static audit.

## Evidence
- `map_selection` loads `mmapblk_load_files` (syndre.sx 31719–31727), including `data/mmapblk.dat` into `world_map` (83675–83683).
- This resource list is freed on Escape (31994–31999), on two territory-selection returns (32016–32022 and 32061–32067), and on another navigation exit (32084–32086). These are identified paths, not proof of exhaustive cleanup under every possible control flow.
- `_draw_map_area` (32804–32867) reads `world_map` and draws a region with 64 columns and 44 rows. Its source position is calculated from territory-related tables and its destination from display inputs. The routine has no visible local check that the source allocation contains the requested span.
- `check_for_click_on_cityadjust_tax` (32164–32226) also indexes `world_map` through territory and mouse-location calculations. No local allocation-size check is visible.
- The native resource loader reports failed items, but the immediate caller does not inspect its error count before rendering.

## Limitations and next work
Actual `mmapblk.dat` length and file-format layout are not available in the repository; no overrun or crash is demonstrated. Inspect source coordinate tables and draw bounds, then create controlled missing/truncated-resource regression tests for an approved Windows build. Keep Mission Builder's bounded asset-loading design and campaign save isolation separate from the legacy baseline.

## Verification
Assembly and native loader source inspection only. No code, saves, assets or gameplay changed; no Windows test performed. Protected v0.7 baseline maintained.
