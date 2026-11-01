# Syndicate 2026 — WScreen control macro reconciliation and Windows CI discovery boundary

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Status: SOURCE CHECK COMPLETED, BUILD CONFIGURATION RESOLUTION OPEN.

## Verified source evidence
- `engine/bflibrary/configure.ac:27-35`: `--disable-lb-wscreen-control` defaults to disabled and the enabled branch calls `AC_DEFINE([WSCREEN_CONTROL], 1, ...)`.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c` uses the distinct identifier `LB_WSCREEN_CONTROL` in conditional compilation at lines 292, 523, 551, 644, 663, 671, 687, 798, 804 and 1016.
- `engine/bflibrary/src/x86-win-sdl/sscreen.c` also tests `LB_WSCREEN_CONTROL` at lines 279, 419, 442, 535, 554, 562, 578, 667 and 841.
- `engine/configure.ac` and `engine/bflibrary/configure.ac` prefer SDL2 >= 2.0.0 when available, falling back to SDL1 >= 1.2.0 as documented in the preceding audit.
- Therefore there is a verified **source naming discrepancy**, NOT proof of incorrect preprocessing, a build failure or a rendering bug. The generated config header, compiler defines, included headers, and any alias must be checked before asserting behaviour.

## Windows CI lookup performed
Attempts to fetch candidate paths `.github/workflows/build-windows.yml`, `.github/workflows/ci.yml`, `.github/workflows/build.yml`, and `.github/workflows/windows-build.yml` on the graphics branch returned not found. GitHub code search did not return useful matches for `mingw`, `windows`, `msys2`, or `LB_WSCREEN_CONTROL` on the default-branch search index. This does NOT establish that no Windows workflow exists: filenames may differ and default-branch search does not enumerate the graphics branch.

## Next verification gate
1. Retrieve the actual .github/workflows directory listing or repository tree instead of guessing paths; inspect Windows job configure and linker options.
2. Inspect generated `engine/bflibrary` config header and compiler command for macro definitions and SDL selection; check any `WSCREEN_CONTROL` / `LB_WSCREEN_CONTROL` alias.
3. Compare the exact Windows build artifact/configuration to source; run isolated 640x480 and 320x200 window/palette/pitch diagnostics before suggesting any change.
4. Preserve original game, v0.7 baseline, core audit, Mission Builder and proprietary retail assets.

## Verification ledger
PASS: direct source reads of Autoconf and SDL1/SDL2 backend exact macro references; candidate workflow path checks.
NOT RUN: definitive workflow tree enumeration, generated header inspection, build, preprocessing, Windows game run, surface pitch instrumentation or screenshots.
FILES MODIFIED: this audit report and graphics continuity index only. No engine or other branch changes.
