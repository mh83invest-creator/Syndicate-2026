# Syndicate 2026 — Focused Audit: WScreen Configuration and SDL Draw-Surface Ownership

Date: 2026-11-01 (Australia/Brisbane)
Stage: focused source-level build-flag and draw-surface trace — COMPLETE
Overall project audit: NOT COMPLETE; runtime/build flag output not directly verified.
Branch: `feature/v0.9-visual-showcase`
Protected baseline: `main` v0.7 `a22b0b9580a9710b4e693e88854351835ef1ee40`.
Scope: determine the default `LB_WSCREEN_CONTROL` configuration and how the corresponding SDL draw surface is created, based on original source and the actual Windows workflow. No edits to binaries/engine or workflow.

## Source findings

1. `engine/bflibrary/configure.ac:27–35` declares `--disable-lb-wscreen-control` and explicitly defaults `lb_wscreen_control=no`. `WSCREEN_CONTROL` is defined only when this option equals `yes`. `AX_PREFIX_CONFIG_H([include/bfconfig.h], [LB])` prefixes exported config symbols, consistent with `LB_WSCREEN_CONTROL` use in SDL code.
2. `.github/workflows/windows-source-v0.7.yml` invokes top-level `../configure --prefix=/ --enable-lbs-structs-expand` with no explicit WScreen-control enable option. `engine/configure.ac:16` uses `AC_CONFIG_SUBDIRS([bflibrary bfsoundlib])`. **Expected normal v0.7 workflow configuration:** the Bullfrog subconfigure retains the default `lb_wscreen_control=no`, unless overriding environment/configure logic not captured in these inspected fragments changes it. Treat this as a strongly source-supported expectation, **not a verified preprocessor dump**.
3. In `engine/bflibrary/src/x86-win-sdl2/sscreen.c:290–325`, when `LB_WSCREEN_CONTROL` is *not* defined and `lbDisplay.WScreen` is non-null, `LbIScreenDrawSurfaceCreate` wraps **application-owned pixel memory** via `SDL_CreateRGBSurfaceFrom(lbDisplay.WScreen, width, height, lbEngineBPP, width, ...)`. Otherwise it creates a new SDL surface via `SDL_CreateRGBSurface`.
4. With this surface-from-existing-pixels path, freeing the SDL surface is **not proof the underlying game-owned WScreen bytes are freed**. Ownership of application WScreen allocation and SDL wrapper object must be considered separately. The exact SDL library ownership contract is not validated through a runtime allocation test in this stage.
5. `sscreen.c:1014–1047` detects when the SDL draw surface's pixel pointer no longer matches `lbDisplay.WScreen` in the non-control case; it creates a replacement SDL draw surface and frees the old SDL surface wrapper. That creates a concrete risk if future overlay code retains the old SDL object or its pitch/format across swaps.
6. `display.c:AppScreenSetup` assigns `lbDisplay.WScreen=VGABuffer+640*480` before invoking `LbScreenSetup`, and `display.c:ApSpriteDrawMCGA` temporarily substitutes game `WScreen` into the shared library pointer. Those explicit app-side pointer changes are consistent with the source's non-control draw-surface check. They are not sufficient to prove all assembly pointer aliasing through every game state.

## Feasibility consequence
Any new renderer/lighting hook must handle the expected application-controlled WScreen buffer, potential SDL surface-wrapper recreation, actual drawable pitch/format and cursor layering. Do not treat an SDL surface pointer or pixels as persistent across mode/focus changes. The compile-time branch should be checked using actual Windows `bfconfig.h` before production code modifications.

## Verification ledger

| Check | Result |
|---|---|
| Read `bflibrary/configure.ac`, top-level `configure.ac`, SDL surface creation/recheck and v0.7 workflow from GitHub | PASS |
| Determine *configured default* for WScreen control | PASS: default `no` in source |
| Verify effective Windows generated `bfconfig.h` or compiler -D flags | NOT RUN |
| Compile/execute the SDL draw-surface path, test freeing and surface rebinding | NOT RUN |
| Gameplay, focus, graphics, night lighting or audio regression test | NOT RUN |

## Continuation
Audit the game assembly WScreen/VScreen/GraphicsPalette/PalMem assignment and mission/title transition lifetime using the existing 1,617-label index; capture exact addresses and distinguish pointer slots from pointed-to buffers. Then design a safe SDL test-only harness for pointer rebinding, pixel format and pitch. The full audit remains incomplete until source coverage, private assets and Windows runtime validation permit completion.

No fixes made. No changes to main. Rollback: documentation-only on the feature branch.
