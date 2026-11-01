# Syndicate 2026 — World-map draw destination boundaries

Date: 2026-11-01. Static analysis only.

## Method
Decoded the embedded 950-byte, 50-record map_countries table from engine/src/syndre.sx (84528–84691), using 19 bytes per record and unsigned little-endian coordinate pairs at offsets 0–3. Inspected _draw_map_area (32804–32867): it halves the two coordinates, draws 64 bytes on each of 44 rows, and increments destination 256 bytes after writing 64, giving an effective row stride of 320.

## Findings
- Coordinates: X 46–542 and Y 6–282 (raw table units). Effective starting coordinates are approximately floor(X/2), floor(Y/2).
- The maximum effective right edge is column 334; maximum bottom edge row 184.
- Territory indices 14, 44 and 45 have 64-pixel spans beyond logical column 319. This can wrap a linear write into subsequent rows and needs visual verification.
- These observations do not establish an out-of-allocation write. Linear frame-buffer capacity, base pointer lifetime, screen pitch and clipping behavior must be confirmed at runtime and against all rendering modes.
- The source code does not perform explicit destination clipping in _draw_map_area.

## Mission Builder integration requirements to assess, not implemented
Validate map resource length for 50 x 2816-byte blocks, territory index 0–49 and adjacency links 0 or 1–50, screen destination bounds and supported pitch; fail safely on missing/truncated input; avoid changing legacy resource behavior until the v0.7 gameplay baseline is tested. Keep campaign saves isolated.

## Verification
Source byte enumeration and arithmetic only. No Windows build, asset or runtime memory checks, engine changes or save operations.
