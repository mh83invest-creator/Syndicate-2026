# Syndicate 2026 — Private Menu and Title Asset Header Inventory

Date: 2026-11-01 (Australia/Brisbane)
Status: focused static archive audit completed; visual rendering and semantics not verified.
Branch: feature/v0.9-visual-showcase. Protected baseline: main v0.7 a22b0b9580a9710b4e693e88854351835ef1ee40.
Private input: user-uploaded v0.6 Two Menus 30 FPS Test ZIP, data/ members. No private bytes committed to GitHub.

## Findings
- mtitle.dat (79,964 bytes), mselect.dat (164,860), mbrief.dat (140,606), moption.dat (53,158), mmap.dat (155,430) and mconfup.dat (83,822) exist in the attached resource set. Their first four bytes are not an RNC1 signature; this does not mean they are raw pixel grids.
- mselect.pal (679 bytes) has RNC1 header and declares 768 uncompressed bytes (256 RGB triplets). Its complete decoder output passed the independently developed read-only RNC1 trial's unpacked CRC check.
- mspr-0.dat (144,855 bytes) and mfnt-0.dat (5,962 bytes) are separately present as menu sprites and font resources by filename/provenance, but their complete logical layout is not established solely by header inspection.
- mconscr.dat (64,000 bytes) exists with no RNC1 signature. Its size is consistent with a 320 by 200 single-byte plane, but that interpretation is a hypothesis until actual draw/copy code paths are checked.
- These are actual available original game-data resources; do not assume a new game title/menu is achievable by changing the native SDL window title alone.

## Implications
Further menu visual planning can be grounded in these existing resources, the original title animation path in syndre.sx, and the source-identified mouse hit boxes. Any layout/text/font redesign must map specific assets and exact hit-box coordinates, preserve navigation, and be tested against language variants. No replacement art, sprites, font metrics or menu style was implemented.

## Verification ledger
PASS: archived filename presence, data lengths, RNC header checks and previously validated mselect.pal unpacked CRC.
NOT RUN: title asset decoding, sprite/font format parsing, menu rendering, in-game navigation, actual visual screenshots, Windows build.
OPEN: title animation frame structure, asset format and replacement constraints, exact UI/selection coverage and language variants.

Next stage: inspect mtitle/mselect draw and animation source, associate header fields with the original video decode logic, and map all menu UI geometry. Keep game assets private. No source or workflow edits; protected baseline untouched.
