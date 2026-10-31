# Syndicate 2026 — Bullfrog File Library Follow-up
Date: 2026-11-01 Australia/Brisbane
Status: source verification only; no build, tests or engine modifications
Predecessor: docs/audits/2026-11-01-save-path-divergence.md

## Repository sources inspected
- engine/Makefile.am and engine/src/Makefile.am
- engine/bflibrary/Makefile.am
- engine/bflibrary/include/bffile.h
- engine/bflibrary/src/x86-win-sdl2/sfile.c
- engine/bflibrary/src/x86-win-sdl2/sdir.c
- engine/bflibrary/src/general/gfile.c
- engine/src/game_data.c, engine/src/game.c, engine/src/oswindws.c, engine/src/dos.c (from preceding audit)

## Findings and corrections
LIB-01 (confirmed): The active SDL2 Bullfrog file backend is engine/bflibrary/src/x86-win-sdl2/sfile.c, included conditionally by the bflibrary Makefile.am. bffile.h has LB_FILENAME_TRANSFORM hard-coded to 1. LbFileOpen invokes LbFileExists (which itself transforms the path), then invokes lbFileNameTransform and opens the transformed path. setup_file_names() sets this to SyndFXFileNameTransform, which prefixes relative save/%02d.gam with GetDirectoryHdd(). Therefore the inspected native C LbFileOpen save route resolves to the data-root/save path, subject to actual runtime data-root configuration.

LIB-02 (confirmed): LbDirectoryMake in x86-win-sdl2/sdir.c applies lbFileNameTransform to a temporary path ending in /a before creating directories. Therefore GetDirectoryUser()'s calls to LbDirectoryMake("save", true) and LbDirectoryMake("screenshots", true) ALSO create paths under the transformed data-root, not necessarily the GetDirectoryUser() location. The previous audit's concern is narrowed from 'unverified transformation' to this source-backed mismatch. The user-root creation and DOS low-level path use require runtime/caller verification.

LIB-03 (conditional but source-backed): Lb_FILE_MODE_NEW opens using O_RDWR|O_CREAT|O_BINARY on Windows and O_RDWR|O_CREAT on POSIX; O_TRUNC is not passed. Thus save_player() opening an existing file via this mode starts at offset zero but need not truncate residual tail bytes if an older file is longer than the current written payload. This is a compatibility/diagnostic risk, not demonstrated corruption. Do NOT alter the mode or legacy file format without tests.

LIB-04 (confirmed): LbFileRead and LbFileWrite directly return OS read/write counts; LbFileClose reports close failure and LbFileFlush is available, but save_player() checks no write count or close result and does not call flush. The earlier save-integrity findings are strengthened by the actual backend semantics.

LIB-05 (confirmed): LbFileOpen performs a separate existence test before opening (TOCTOU window); not a demonstrated exploitable issue in this usage. Ensure future save atomicity and failure tests cover missing directories, existing slots and partial writes, without redesigning working behaviour unapproved.

LIB-06 (unresolved): The actual assembly-level references to DOS open, save/load and Continue could not be extracted from engine/src/syndre.sx through connector line-range fetch (returned empty content). No claims are made about the assembly caller graph.

## Test recommendations — NOT performed
- Save twice to the same slot and inspect size and content, including a deliberately oversized fixture, in a synthetic test area.
- Trace exact runtime resolved paths through LbFileOpen, LbDirectoryMake and dos_low_level_open for default registry, missing registry and alternate working directory.
- Inject failed open, short write and close error into a test build and examine reported status and in-memory state.
- Confirm legacy save visibility and mode separation before approving any campaign namespace implementation.

## Constraints and handover
The v0.7 working source and original campaigns remain protected. Mission Builder shares save namespace integration contract; no separate decision/implementation is approved here. All of the above is static inspection; no Windows compilation, game assets, executable testing, or save migration occurred. Next: inspect LbFileFindFirst enumeration and complete assembly calls using a local checkout or sufficiently accessible source segments. No rollback required for this docs-only commit.
