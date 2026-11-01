# Syndicate 2026 — SDL1 and SDL2 generated config header inclusion

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: source-level follow-up on macro prefix resolution; no runtime or code changes.

## Direct source evidence
- `engine/bflibrary/configure.ac:10-11` sets `AC_CONFIG_HEADERS([include/bfprivconfig.h])` and `AX_PREFIX_CONFIG_H([include/bfconfig.h], [LB])`.
- `engine/bflibrary/configure.ac:27-35` defaults `lb_wscreen_control=no` and conditionally defines `WSCREEN_CONTROL` when enabled.
- `engine/bflibrary/src/x86-win-sdl/sscreen.c:25-27` conditionally includes `bfconfig.h` under `HAVE_CONFIG_H`, and subsequently checks `LB_WSCREEN_CONTROL` at lines 279, 419, 442, 535, 554, 562, 578, 667 and 841.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:25-27` has the same generated-header include condition, and checks `LB_WSCREEN_CONTROL` at lines 292, 523, 551, 644, 663, 671, 687, 798, 804 and 1016.
- `engine/bflibrary/Makefile.am:61` names generated `include/bfconfig.h`, while lines 159-160 add generated include-directory search path.
- Thus both SDL backends use a configuration structure consistent with prefixing `WSCREEN_CONTROL` to `LB_WSCREEN_CONTROL`. This strengthens the source-level resolution in `2026-11-01_wscreen_macro_prefix_generation_resolved.md`.

## Important limits
- Conditional include is **only executed when HAVE_CONFIG_H is defined**; a particular compiled binary's flags have not been inspected.
- The intended default is screen control disabled, not evidence of the existing Windows binary's actual macro values.
- Generated `bfconfig.h`, native compiler command, active SDL version and GitHub Actions Windows workflow remain unverified. No source-level macro defect has been established.

## Verification ledger
PASS: GitHub direct reads of both SDL backends, Autoconf input and Makefile header references.
NOT RUN: configure/preprocessor, build, Windows execution, SDL pixel-pitch validation, screenshot comparison, original retail-asset parsing.
CHANGED: only this report and graphics audit continuity index on graphics audit branch.
NEXT: retrieve verified generated headers/build commands or the exact workflow to establish flags for the Windows executable. Preserve all protected game branches and gameplay.
