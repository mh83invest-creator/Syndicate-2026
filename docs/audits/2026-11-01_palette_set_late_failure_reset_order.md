# Syndicate 2026 — Late palette failure and SDL2 reset ordering

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics-only static source audit; no engine modifications.

## Verified caller path
- `engine/src/display.c:72-95`: `AppScreenSetup` passes the application `GraphicsPalette` into `LbScreenSetup`. When that pointer is non-null, creation of the draw surface in SDL2 is invoked with `set_palette = (palette == NULL)`, therefore false.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:556-567`: `LbScreenSetup` assigns `lbScreenInitialised = true` **before** applying the non-null palette through `LbPaletteSet`. On a failed result, it logs, calls `LbScreenReset()`, and returns failure.
- `sscreen.c:617-639`: `LbScreenReset` declines cleanup only when `lbScreenInitialised` is false. For the **late palette-failure path above**, the flag has already been set true, so reset executes: mouse suspend, unlock if locked, frees the draw surface if `lbHasSecondSurface`, clears that flag, nulls draw and screen surface pointers, and marks uninitialized.
- This differs from a draw-surface **creation** failure occurring before initialized is true, for which setup invokes reset but the guard can prevent cleanup.
- Separately `sscreen.c:1014-1047` continues to invoke `LbIScreenDrawSurfaceCreate(true)` for WScreen pointer rebinding, a different optional draw-surface palette-copy path already documented.

## Assessment
The normal late application palette failure follows a cleanup-capable reset path, as opposed to the earlier stubbed initial helper palette failure. Static source evidence does **not** establish whether `LbPaletteSet` can partially mutate any window or draw palette before returning failure, nor confirm a successful real SDL reset/retry. The implementation of `LbPaletteSet` was not independently verified in this particular pass; avoid any claim about its internal atomicity.

## Test ledger
PASS: direct GitHub read of application palette call, SDL2 initialization ordering and reset implementation; confirmed conditional control-flow semantics.
NOT DONE: real SDL palette failure injection, `LbPaletteSet` implementation/rollback audit, Windows build or game execution, graphical verification.
Changed in GitHub: this report and the graphics audit continuity index only.
Next: identify `LbPaletteSet` implementation and palette propagation to SDL draw/window surfaces; test partial failure recovery when native SDL environment available.
