# Syndicate 2026 — Palette fade error handling and SDL1 palette recovery

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: read-only graphics source audit; documentation changes only.

## Direct findings
- `engine/bflibrary/src/general/gpalette.c:38-106`: `LbPaletteFade(from_pal, fade_to, fade_steps)` has a timed mode when `fade_steps == 1`, looping `fade_count=0` through `fade_to`; each step waits for VBI, invokes `LbPaletteSet(palette)` (line 66) and `LbScreenSwap()` (line 69). **Neither return is inspected**, so neither a failed palette update nor a failed swap directly stops this loop.
- Same function's incremental mode (`fade_steps != 1`) calls `LbPaletteSet(palette)` at line 103, likewise without inspecting its return. Both branches call `LbPaletteGet(fade_from_pal)` at line 50 or line 89 without inspecting its return, so an unsuccessful starting-palette fetch does not explicitly abort.
- Function returns `fade_count` (line 105), not the result of its internal palette or swap calls. Thus callers cannot infer SDL update success merely from the returned fade counter.
- Both interpolation branches calculate `fade_count * diff / fade_to` (lines 63 and 100), with no local guard for `fade_to==0`. Reachability with zero from actual callers is **not established**, and no runtime divide-by-zero is claimed.
- `engine/bflibrary/src/x86-win-sdl/spalette.c:50-100`: SDL1 `LbPaletteSet` also updates internal palette arrays before separate `SDL_SetColors` calls, flags failures without rollback and continues subsequent update attempts, comparable to the previously audited SDL2 sequence.
- `engine/bflibrary/src/x86-win-sdl/spalette.c:102-128`: SDL1 has an explicit `LbIPaletteRestoreLost` function restoring the display's palette after focus switching, with a checked `SDL_SetColors` failure result; this specific restoration helper is distinct from SDL2 window-surface reference restoration.

## Implications and uncertainties
- A partial surface palette update reported by `LbPaletteSet` can be ignored by the fade loop, which continues to the next frame/step. Real SDL partial-change behaviour, actual window output, application fade callers and focus restoration invocation are not proven.
- No runtime fault injection was executed. Do not alter original fade pacing, colours or game logic without an approved fix and native verification.

## Verification ledger
PASS: direct GitHub reads of `general/gpalette.c` and SDL1 `spalette.c`; function return-path inspection and SDL1 backend comparison.
NOT RUN: Windows executable, SDL palette failure, fade test, divide-by-zero reachability, screenshots.
CHANGED: this report and `docs/GRAPHICS_AUDIT_CONTINUITY.md` only on graphics audit branch.
NEXT: inspect actual palette fade call sites, possible `fade_to==0` inputs, SDL1 focus-palette restoration caller; design narrowly scoped failure instrumentation.
