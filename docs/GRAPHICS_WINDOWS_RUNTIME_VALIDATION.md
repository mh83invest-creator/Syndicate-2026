# Syndicate 2026 — Windows graphics candidate runtime validation

Candidate branch: `fix/graphics-sdl2-scaler-isolated-2026-11-01`
Baseline: protected v0.7; never overwrite.
Status: execution on real Windows desktop with locally owned retail game assets is required. These instructions are acceptance criteria, **not a claim tests have already run**.

## Build sources and diagnostics
- Experimental workflow: `.github/workflows/graphics-sdl2-validation.yml`. Its generated artifacts must be clearly marked `graphics-isolated`; do not install it over the protected v0.7 folder.
- Experimental engine is built with `-DSYNDICATE_GRAPHICS_DIAGNOSTICS`. On screen setup, logger emits `graphics diagnostics: draw ...; window ...` including w/h, BitsPerPixel, BytesPerPixel and pitch, plus whether a separate draw surface exists.
- First time each scaler route is encountered, logger emits `graphics diagnostics: blit route=N ...`: 0 SDL_BlitSurface same size; 1 SDL_BlitScaled nonindexed source; 2 custom indexed->indexed; 3 custom indexed->truecolour.
- Root v0.7 CI and release numbering untouched. Build package includes no retail data; use separately supplied legally owned local game assets.

## Actual Windows test matrix
1. Run clean start in fullscreen and windowed (`--windowed`), with and without `--no-stretch`.
2. Test 320x200 logical mode and 640x480 logical mode (check mode is truly active); collect log of actual SDL draw/window dimensions, pixel depth, bytes per pixel, pitch and route.
3. Switch modes and alt-tab repeatedly, including returning from focus loss. Check no freeze/crash, incomplete frame, palette scrambling or black screen.
4. Inspect sprites, palette, main menu, mission interface and map backgrounds against unmodified v0.7 **at the same configuration**, using matched screenshots and pixel/image comparisons where feasible. Do not assume identical pixel arrays when presentation dimensions differ.
5. Exercise high-load scenes/explosions. Measure frame timing against baseline (preserve original game speed, avoid stalls and unexpected frame-rate changes). Check mouse/camera scrolling and edge scrolling against baseline.
6. Check campaign, American Revolt and save handling are unaffected; create test saves only, do not overwrite real saves.
7. Compare isolated build vs protected v0.7; record Windows version, GPU/driver, actual SDL2 DLL, display scaling, resolutions, test outcome and logs.
8. Perform negative tests for invalid surface lock/restore and palette calls only in separately instrumented/fault-injection environment; do not intentionally corrupt a normal game save.

## Pass criteria / release gate
- Windows CI compiles diagnostic engine and packages test executable; SDL2 DLL import shown.
- Native SDL2 dummy window probe and 12-case standalone layout regression tests both pass.
- Real Windows game logs show actual scaler route and surface pitches; when custom scaler path is exercised there is no out-of-bounds access under an appropriate test and no visual regression.
- Gameplay, scrolling, explosions, colours, menus, FPS behaviour and saves pass comparative checks.
- Only then consider a reviewed merge; do not change v0.7 or publish as production before those gates.

## Known limitations
- The dummy SDL2 window test uses 640x400 and 320x200 test surfaces, not actual gameplay assets.
- The layout contract test is isolated C, not the original Syndicate renderer.
- The real Windows game has not been launched in this ChatGPT container (no Wine or Windows desktop; no retail game data available here).
- Further palette recovery/ownership risks remain audit findings rather than proven regressions and are not silently fixed by the scaler patch.
