# Syndicate 2026 — WSCREEN_CONTROL / LB_WSCREEN_CONTROL macro prefix reconciliation

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Type: static source audit; no implementation changes.

## Resolved source-level discrepancy
- `engine/bflibrary/configure.ac:10-11` declares `AC_CONFIG_HEADERS([include/bfprivconfig.h])` followed by `AX_PREFIX_CONFIG_H([include/bfconfig.h], [LB])`.
- `engine/bflibrary/configure.ac:27-35` declares an optional screen-control setting that defines `WSCREEN_CONTROL` when enabled; the option defaults to disabled.
- The `AX_PREFIX_CONFIG_H` Autoconf macro is specifically intended to generate a prefixed public config header. Thus the use of `LB_WSCREEN_CONTROL` in the SDL source is consistent with the requested `LB` prefix, rather than an established typo.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:24-27` includes `bfconfig.h` when `HAVE_CONFIG_H` is defined.
- `engine/bflibrary/Makefile.am:159-160` includes both source and build include directories; the header is generated as part of configure rather than tracked in the repository (a direct fetch of `engine/bflibrary/include/bfconfig.h` returned 404).
- Therefore **the source-level naming mystery is explained**. This does not establish whether the active Windows binary enabled screen control or which backend it linked: the actual generated file and compiler invocations have not been checked.

## CI scope
The present connector cannot enumerate `.github/workflows` using `fetch_file`; a public GitHub API URL was also inaccessible to the browsing tool, and the available workflow-run lookup filters PR-triggered runs. The Windows CI workflow and build flags remain unverified; no absence or success/failure claim is appropriate.

## Next verification
Obtain an actual Windows build configuration output containing `bfconfig.h` / `bfprivconfig.h`, compile command and SDL library linkage. Then confirm `HAVE_CONFIG_H`, `LB_WSCREEN_CONTROL`, `USE_SDL2`, and actual surface pitch/runtime graphics behaviour. Do not implement a workaround for a macro problem that is now source-explained.

## Ledger
PASS: direct source inspection of `configure.ac`, screen include, Makefile include search paths and missing generated header file path.
NOT TESTED: actual macro expansion in generated output, Windows CI build/job logs, compilation, execution, screenshots, private assets.
CHANGED: documentation report and graphics index only; game code, protected baseline and parallel branches untouched.
