# Syndicate 2026 — Fade call-site search coverage and SDL backend selector

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: static graphics audit, documentation only.

## Source-verified build selection
- `engine/bflibrary/Makefile.am:123`: `if USE_SDL2` selects SDL2-specific library build sources; alternative branch uses SDL1 files.
- `engine/bflibrary/configure.ac:25,86-105,143`: configure has `--without-sdl2`, probes SDL2, can fall back to SDL1, and sets `AM_CONDITIONAL([USE_SDL2], [test "x$have_sdl2" == "xyes"])`.
- This describes source configuration, **not** the active backend of the Windows executable or CI configuration.

## Fade call-site search: bounded result
- `engine/bflibrary/src/general/gpalette.c:38-106` contains the generic `LbPaletteFade` function with unguarded division by `fade_to` in two branches, as in prior source audit. Its caller return-value and fade-to-zero reachability are **not yet established**.
- Attempted repository-tree-guided read of application `.c/.cpp/.h` files across `engine/src` and general library sources. The bulk read orchestration hit tool-call limits before a complete set of results was obtained. Hence there is **no completed repository-wide caller inventory** and no evidence to assert a zero input is reached or impossible.
- Earlier targeted inspections of `engine/src/main.c` and `engine/src/game.c` found no direct `LbPaletteFade` calls. These two files cannot stand in for the full repo.

## Pending verification
1. Complete a rate-limited caller inventory of relevant source modules or use a full repository checkout with ripgrep.
2. Examine whether actual callers can pass `fade_to=0`; test independently only if proven relevant.
3. Verify active Windows SDL backend from build artifacts or workflow and monitor screen/palette formats at runtime.
4. Preserve original fade pacing, graphics and gameplay unless a test-backed fix is approved.

## Verification ledger
PASS: direct GitHub read of SDL selection configuration and fade function; narrowed targeted searches.
INCOMPLETE: full fade caller graph, zero-input reachability, Windows backend/build, actual SDL palette recovery.
CHANGED: this report and graphics continuity index only. No engine, protected baseline or parallel branch edits.
