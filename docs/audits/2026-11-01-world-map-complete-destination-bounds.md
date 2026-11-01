# Syndicate 2026 — Complete built-in world-map destination bounds
Date: 2026-11-01 (Australia/Brisbane)
Status: static source audit only; engine, saves and Mission Builder unchanged.
Preceding handover: docs/audits/2026-11-01-menu-to-map-call-chain.md.
Source: engine/src/syndre.sx, blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Verified method
Decoded the 950-byte map_countries table (50 records × 19 bytes) at assembly line 84528 onward. The first two unsigned little-endian words of each record are X and Y. In _draw_map_area (32804–32867), the coordinates are halved; the routine scans 64 bytes across by 44 rows at an effective destination row pitch of 320 bytes. The upper bound on the last addressed byte, relative to caller-provided destination base, is (floor(Y/2)+43)*320 + floor(X/2)+63. Zero source pixels skip the actual store, so this is a conservative maximum address, not proof of writes at every pixel.

## Results across all 50 records
- Full 950 bytes decoded successfully.
- Maximum 0-based destination offset: 59214 (territory index 45, raw X=542, raw Y=282, effective origin X=271 Y=141).
- For a valid 64000-byte target frame, even the maximum conservative address is within allocation: bytes 59215–63999 are beyond that maximum.
- Every built-in record has a calculated last-byte offset less than 64000.
- Exactly three records exceed the nominal 319 rightmost column:
  - index 14: raw (532,76), half (266,38), right edge 329, last byte offset 26249.
  - index 44: raw (542,246), half (271,123), right edge 334, last byte offset 53454.
  - index 45: raw (542,282), half (271,141), right edge 334, last byte offset 59214.
- Such row-crossing writes can alter adjacent scanlines even when they remain within a contiguous frame. No visible defect established.

## Pointer and allocation qualifications
Previous source tracing: map_selection is called from the select-menu handler func_37b29 at line 54597; inspected select initialization sets BScreen=WScreen+0xFA00 at 54253–54255. Two _draw_map_area calls use BScreen; two use WScreen. Thus with a 64000-byte BScreen offset, max hypothetical address relative to original WScreen allocation is 123214, less than synthetic WScreen loader requests (256016 MCGA or 256048 vres16). These comparisons are conditional on successful allocation, live pointer and the inspected menu initialization being active. Alternative BScreen assignments at WScreen+128000 and +128024 exist elsewhere; no general path-independent claim is made.

## Remains unverified
Runtime destination pointer integrity and allocation success; world_map source resource decompressed size; missing/truncated resource behaviour; all possible mode transitions; exact visual consequences of wrapping; executable/Windows builds and memory instrumentation. Do not alter clipping, source data or screen-copy counts without a separately approved patch and before/after Windows test.

## Handover and verification
Performed: read full GitHub assembly blob, decoded source table and independently evaluated 50 integer rectangle bounds. This is source verification only, not runtime or gameplay testing. No engine code, original save files, baseline v0.7, or concurrent Mission Builder branch modified. Next task: investigate source world_map allocation size and loader validation, then mode-specific buffer transitions.
