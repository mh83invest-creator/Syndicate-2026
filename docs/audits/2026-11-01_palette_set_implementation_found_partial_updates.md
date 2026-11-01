# Syndicate 2026 — Located LbPaletteSet implementation; partial update semantics

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics-only source audit. No code changes.

## Source location correction
The backend palette implementation lives at:
- `engine/bflibrary/src/x86-win-sdl2/spalette.c`
- `engine/bflibrary/src/x86-win-sdl/spalette.c`
not the previously guessed `spal.c` paths. Located by reading the GitHub repository tree (`git/trees/main?recursive=1`) and then fetched both implementations directly from the **graphics audit branch**. Prior audit's 'implementation not found' status is superseded.

## SDL2 verified findings: spalette.c:50-100
- `LbPaletteSet(const ubyte *palette)` requires an initialized screen and non-null `lbDrawSurface` (lines 58-60).
- Fills internal `lbPalette` using `srcColors[n] & 0x3F` and writes each RGB component of `lbPaletteColors[256]` using six-bit values shifted left two bits (lines 62-78).
- If actual screen-surface format reports <=8 BitsPerPixel, attempts `SDL_SetPaletteColors` on its palette; on a failure logs and changes `ret` to `Lb_FAIL` (lines 79-88).
- Assigns `lbDisplay.Palette = lbPalette` regardless of the screen palette call's return (line 89).
- If a secondary surface exists and `lbEngineBPP <= 8`, independently calls `SDL_SetPaletteColors` for the draw surface. Failure logs and sets `ret=Lb_FAIL` but does not roll back earlier changes (lines 90-99).
- Thus the function has **no transaction/rollback semantics**: internal palette arrays are already modified when SDL update is attempted, and each applicable surface update runs independently. A failed SDL call can lead to divergent palette state, subject to SDL's actual mutation semantics. This is a static risk, not a demonstrated Windows fault.

## SDL1 cross-reference
- `engine/bflibrary/src/x86-win-sdl/spalette.c` similarly defines `LbPaletteSet` (line 50 onward) and uses `SDL_SetColors` for surface updates, with its own `!= 1` error checks. No claim of identical behaviour beyond inspected corresponding structure without targeted deeper analysis.
- Generic fade logic at `engine/bflibrary/src/general/gpalette.c` calls `LbPaletteSet`; detailed caller failure propagation has not yet been audited.

## Relation to setup recovery
`engine/bflibrary/src/x86-win-sdl2/sscreen.c:556-567` sets `lbScreenInitialised = true` before applying the initial palette and calls `LbScreenReset` after failed `LbPaletteSet`. Other callers may respond differently; failure propagation must be traced before asserting gameplay impact.

## Verification ledger
PASS: repository tree lookup, direct audit-branch reads of both SDL palette backends and general palette file, source inspection of SDL2 palette update sequence.
NOT RUN: real SDL palette failure injection; native Windows gameplay; visual screenshots or SDL surface mutation examination.
CHANGED: this report and graphics continuity index only, no engine edits. Protected baseline and parallel branches untouched.
NEXT: audit palette fade and other callers' error handling, SDL1 update ordering, and native SDL failure-state reproduction.
