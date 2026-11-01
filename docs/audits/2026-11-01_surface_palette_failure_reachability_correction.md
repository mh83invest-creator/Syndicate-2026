# Syndicate 2026 — Palette failure reachability correction and real SDL availability

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics source audit, correcting interpretation of earlier isolated fault-state model.

## New source-verified distinction
- `engine/src/display.c:72-95`: normal `AppScreenSetup` passes `GraphicsPalette` to `LbScreenSetup`.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:533-539`: setup invokes `LbIScreenDrawSurfaceCreate(palette == NULL)`. **When GraphicsPalette is non-null**, this sets `set_palette=false`, so the creation helper's palette failure path is **not reached during that initial drawing-surface creation**.
- `sscreen.c:551-568`: setup sets `lbScreenInitialised=true` *before* invoking `LbPaletteSet(palette)`; if this later palette step fails, `LbScreenReset` sees an initialized screen and reaches its cleanup path. This is distinct from the initial create failure/reset-short-circuit discussed in prior audit.
- `sscreen.c:1014-1046`: WScreen rebind still invokes `LbIScreenDrawSurfaceCreate(true)`; the helper's optional palette failure remains possible in this path if SDL returns failure, and the rebind callback restores the old draw pointer.
- `sscreen.c:290-325`: the helper still sets `lbHasSecondSurface=true` before attempting optional palette setup, with no clear on palette failure. This remains a source-level conditional state concern, not a demonstrated normal-startup bug.

## Interpretation and correction
The prior `2026-11-01_surface_failure_fault_injection_model.md` correctly reproduced a *stubbed* initial helper palette-failure state when explicitly forcing `set_palette=true`, but that precondition does **not** match the normal `AppScreenSetup(..., GraphicsPalette)` call when the palette pointer is non-null. Therefore **do not cite the stubbed test as evidence of a reachable normal mission startup failure**. A `NULL` palette caller or direct helper/rebind caller can have different reachability; no exhaustively proven set of call sites has been established.

## Actual SDL test capability check
In the current Linux runtime, `pkg-config --modversion sdl2` did not return a version and SDL2 development header `/usr/include/SDL2/SDL.h` was not present. The runtime shared library `libSDL2-2.0.so.0` was found through `ldconfig`. Thus the system may support dynamic SDL API calls, but compiling the game or its source SDL implementation normally using SDL2 dev headers is not presently established. No native SDL palette-failure injection or Windows gameplay test was executed in this pass.

## Verification ledger
PASS: direct GitHub source reads of the real application call, SDL2 helper call, initialized-state ordering, and rebind call; local SDL compiler-package check.
NOT TESTED: actual SDL surface palette failure, Windows executable, Windows CI config, native game compilation, screenshots.
CHANGED: this report and graphics continuity index only. Engine and parallel branches unchanged.
NEXT: obtain real SDL dev toolchain/headers or isolated dynamic API test; audit LbPaletteSet failure/reset and repeat with actual native SDL surfaces. Do not change protected baseline without tested evidence.
