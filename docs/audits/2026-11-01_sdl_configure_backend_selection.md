# Syndicate 2026 — SDL backend selection and WScreen configure trace

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics build configuration static audit; documentation only.

## Source read
- `engine/configure.ac` lines 72-73, 152-173.
- `engine/bflibrary/configure.ac` lines 24-36, 85-106 and 143.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c` and `engine/bflibrary/src/x86-win-sdl/sscreen.c` as cited in preceding graphics audit reports.

## Verified
- Both engine and bflibrary Autoconf scripts offer `--without-sdl2`. Unless disabled, they first attempt `AM_PATH_SDL2([2.0.0])`; success defines `ENABLE_SDL2`.
- Without SDL2, both try `AM_PATH_SDL([1.2.0])` and define `ENABLE_SDL1`; error if neither is found.
- `engine/bflibrary/configure.ac` has `--disable-lb-wscreen-control` and sets the option default to `no`; enabling the option defines `WSCREEN_CONTROL`. Source in the inspected SDL backends references `LB_WSCREEN_CONTROL` conditionally. These macro spellings differ in the inspected text; whether another header maps them is not established. Do not claim compile-time malfunction without preprocessor/build resolution.
- SDL backend code supports different allocation paths based on WScreen ownership, screen mode, second surface and current pixel depth. Autoconf defaults are not proof of the active compile-time macros used in the Windows CI executable.

## Remaining gate
Inspect actual GitHub Actions Windows workflow and its configure/build invocation; inspect any C preprocessor mapping of `WSCREEN_CONTROL` and `LB_WSCREEN_CONTROL`; then record SDL1/SDL2 library selection, surface creation and resolution. Only then validate pitch/mode behaviour in a Windows executable.
Preserve protected v0.7 baseline, existing gameplay mechanics and the independent core / Mission Builder branches. No copyrighted retail assets.

## Verification
PASS: direct GitHub Autoconf source reads and comparison with inspected SDL backend conditional code.
NOT RUN: configure, compilation, CI build log inspection, Windows gameplay/pixel-format checks, graphical screenshots, private archive parsing.
Changed: this report and graphics continuity index only.
