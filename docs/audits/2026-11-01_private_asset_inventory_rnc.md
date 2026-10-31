# Syndicate 2026 — Private Retail Asset Inventory and RNC Header Audit

Date: 2026-11-01 (Australia/Brisbane).
Status: completed limited archive/header examination; full asset parsing and runtime validation NOT completed.
Branch: feature/v0.9-visual-showcase.
Protected baseline: main v0.7 at a22b0b9580a9710b4e693e88854351835ef1ee40.

## Important recovered dependency
Previously recorded as requiring later user upload, legally owned game resources are already present locally inside the user-uploaded archive Syndicate_2026_v0.6_Two_Menus_30FPS_Test(1).zip. This archive contains 491 entries, 437 data files. The files include original palette, sprite, map, building block and sound resources. Do not ask the user to re-upload these same assets merely for offline format inspection. Their presence does not establish that the original executable can be run under the current Linux environment, nor does it replace a Windows gameplay test.

## Direct archive inspection
Five high-resolution palettes were found:
- hpal01.dat: 90 bytes on disk, RNC1 header, declared 768-byte decompressed data, 72-byte compressed payload.
- hpal02.dat: 88 bytes, 768 uncompressed, 70 compressed.
- hpal03.dat: 90 bytes, 768 uncompressed, 72 compressed.
- hpal04.dat: 577 bytes, 768 uncompressed, 559 compressed.
- hpal05.dat: 576 bytes, 768 uncompressed, 558 compressed.
RNC header begins with hex 52 4e 43 01; sizes were read as big-endian 32-bit values at offsets 4 and 8. This is **header interpretation**, not yet a validated decompression of every palette. The much smaller compressed sizes of palettes 1–3 do not mean they contain only 30 colours; treating compressed bytes as raw RGB values is incorrect. Expected decoded size 768 fits 256 RGB triplets.

Other relevant user-supplied files include map01.dat, map02.dat, hblk01.dat, col01.dat, hspr-0.dat, hspr-0.tab, and original animation data. Sample maps and building/sprite assets also carry RNC1 signatures. This is material new evidence for the graphics feasibility audit.

## Technical consequences
- Full *offline* palette RGB comparisons and original map/sprite format checks may now proceed without another upload, after selecting a verified RNC unpacker.
- Header-only file examination cannot establish correct palette colours, actual light placement, occlusion or a functioning nighttime screenshot. Night-lighting remains unimplemented.
- No retail game resources may be pushed to this public repository. This report contains filenames, sizes and format observations only; no proprietary bytes.
- The user-confirmed gameplay baseline remains v0.7, whereas this archive is a historical v0.6 packaged test. Cross-version resource equality and current Windows running behaviour must not be presumed without checking.

## Verification
PASS: local ZIP inventory, file list and 18-byte RNC header inspection for hpal01 through hpal05.
NOT RUN: decompression, actual 256-colour RGB comparison, complete map/sprite decode, renderer capture, 32-bit Windows build, gameplay performance/focus checks.
UNRESOLVED: resource-set version compatibility between v0.6 and current branch; original palette decoding and night-lighting effects.

## Continuation
Locate the original RNC decompressor in repository or use a verified offline read-only decoding implementation; decode five palette files and compare their RGB entries; inspect map/block/sprite formats with no public publication of retail bytes. Then design the next test steps for static art feasibility. No engine files or protected main modified.
