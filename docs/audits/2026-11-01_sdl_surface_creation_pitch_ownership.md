# Syndicate 2026 — SDL surface creation, pixel format and pitch ownership

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Type: static read-only graphics audit.

## Sources verified
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c` lines 284–325, 344–371, 499–528, 673, 799–818.
- `engine/bflibrary/src/x86-win-sdl/sscreen.c` lines 271–312, 386–420, 564, 652–673.
- Previously recorded SDL scaling, palette and display conversion reports.

## Observations
- Both backends can construct an SDL secondary drawing surface around the application's `lbDisplay.WScreen` buffer using `SDL_CreateRGBSurfaceFrom` where the relevant conditional compilation and non-null pointer path is active; otherwise they can allocate a new `SDL_CreateRGBSurface`.
- Both set `lbHasSecondSurface=true` on successful secondary drawing-surface creation and can copy the current palette into an indexed surface when `set_palette` is requested.
- SDL2's mode formatting routine recognises indexed 8-bit, RGB555, RGB565, RGB24 and RGB888 pixel-format options in its switch. This is support in source, not evidence that all modes have been tested or selected in the Windows executable.
- SDL2 can obtain the window surface using `SDL_GetWindowSurface`, and its drawing-surface setup assigns `lbDisplay.GraphicsScreenWidth` from the actual draw surface's `pitch` (line 673). SDL1 also assigns `lbDisplay.GraphicsScreenWidth` from surface `pitch` (line 564).
- The custom indexed scaling helpers previously examined accept raw pixel pointers and dimensions but no explicit source/destination pitch. The coexistence of pitch-aware setup and width-based scaling leaves a conditional surface-layout compatibility question; it does not prove a bug, overrun or visible artifact.
- Backend selection for the actual Windows binary and any relevant `LB_WSCREEN_CONTROL` conditional compilation values have not yet been established. Do not claim that both backends or all branches run in the shipped build.

## Next integration validation
1. Check active Windows build scripts and compile defines selecting SDL1 versus SDL2 and `LB_WSCREEN_CONTROL`.
2. Trace `LbScreenSetupAnyMode` mode-specific secondary-surface allocation decisions; record widths, heights, pixel depth, expected pitch and ownership.
3. Run isolated Windows instrumented screen-setup/swap tests before any graphics patch, with screenshots for 640×480 Vres16, 320×200 MCGA, scaling, cursor and palette fades.
4. Preserve v0.7 protected baseline, gameplay timing, separate campaign saves, and boundaries with core audit and Mission Builder.

## Verification
PASS: direct GitHub C source review of SDL1/SDL2 allocation, format selection and pitch assignment.
NOT RUN: native compilation, actual Windows backend resolution, memory/pitch instrumentation, screenshot equivalence, private asset decode, profiling.
No source-code changes or copyrighted original assets committed. Documentation-only graphics audit.
