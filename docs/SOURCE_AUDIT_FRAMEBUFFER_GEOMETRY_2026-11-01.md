# Syndicate 2026 — Verified Mission Framebuffer Geometry and Pointer Overlap

**Date:** 2026-11-01 (Brisbane)
**Status:** Source-backed pointer/arithmetic analysis, **not compiled or runtime validated**.
**Branch:** `feature/v0.9-visual-showcase`; protected baseline `main` v0.7 at `a22b0b9580a9710b4e693e88854351835ef1ee40`.
**Source:** `engine/src/display.c`/ `display.h` / `mouse.c` plus `engine/src/syndre.sx` Git blob `c35e22621a93d1e07c44d40de8867446ff7c64bf` at exact cited lines.
**Method:** Static source analysis and independent integer arithmetic; **no retail assets, Windows launch, screenshots or engine edits**.

## A. Exact 640×400 mission placement in the 640×480 output

The C display backend allocates a `VGABuffer` of **`640*480*2 + 16 = 614,416` bytes** (`display.c:127–131`).

`AppScreenSetup` sets the Bullfrog draw pointer `lbDisplay.WScreen = VGABuffer + 640*480` (`display.c:72–95`), i.e. **offset 307,200**, establishing a second staging area.

When loading a level in Vres16 mode, original assembly `load_map_level` (`syndre.sx:27866–27880`) assigns:
```asm
mov EXPORT_SYMBOL(VGABuffer),%edx
add $0x6400,%edx
...
mov %edx,EXPORT_SYMBOL(VScreen)
```
`0x6400 = 25,600 = 40 * 640` pixels. Thus the **mission-mode `VScreen` points 40 displayed rows into the first 640×480 frame region**.

`update_vscreen_whole_vres16` (`display.c:149–231`) converts four `WScreen` bit planes, each **400*80 = 32,000 bytes**, into **400 rows × 640 pixels = 256,000 palette indices**, starting at `VScreen`.

`swap_wscreen` (`display.c:244–260`) copies **640*480 = 307,200 bytes** from the *base* `VGABuffer` to the Bullfrog draw surface and invokes `LbScreenSwap`. This yields the following **source-derived geometry** for the Vres16 mission:

| Output buffer rows, 0-based | Bytes within first `VGABuffer` frame | Meaning derived from source |
| --- | ---: | --- |
| 0–39 | 0–25,599 | Outside the planar converter's 400-row output (top border) |
| **40–439** | **25,600–281,599** | **640×400 planar-converted mission image** |
| 440–479 | 281,600–307,199 | Outside converted mission image (bottom border) |

This matches `engine/src/mouse.c:31–45`, which subtracts **40** from native mouse Y under `DrwF_ScreenVres16` to produce mission logical mouse coordinates. This **directly links graphics letterboxing and input mapping**. Never change mission vertical placement without changing and testing logical pointer transformation, hover, camera and hit testing.

**Unverified nuance:** The converter begins with `LbMemorySet(VScreen, 0, 640*480)`, clearing **307,200** bytes, even though its converted image only occupies 256,000 bytes. Starting at offset 25,600 means the clear reaches offset **332,800**, or **25,600 bytes into** the nominal second staging area beginning at offset 307,200. This is **within the overall allocation** (614,416 bytes), but the two logical regions overlap during that clear. The later `swap_wscreen` copy overwrites the draw area; whether other code can observe the transient overlap was **not tested**. Do not conclude a native crash or bug from the arithmetic alone.

## B. More precise location of a potential scene-only pixel treatment

This analysis narrows possible visual-treatment boundaries without authorising a feature:

- A **scene-region pixel remap candidate** exists after four-plane conversion, inside the 640×400 image at rows 40–439. That avoids blindly applying a palette shift to the whole 640×480 frame, but it still includes any HUD or effects already present in the planar game image.
- A general **SDL-level palette update** affects any pixels using a given palette index, including agents/UI that share that index; it cannot independently illuminate neon, streetlights or selected windows.
- A **cursor** may be drawn during the original swap path or Bullfrog `LbMouseOnBeginSwap`; whether a proposed overlay applies before or after cursor composition must be verified for the actual graphics mode. No scene segmentation/depth mask is demonstrated by this analysis.
- A true localized light system still requires precise source geometry and occlusion: the original map/tile/sprite data and real rendered output are not yet available.

Thus, **scene placement is known exactly from source**, but **night-lighting feasibility is not yet established**. The low-resolution strategic/menu path uses a different palette/resource/screen mode and does **not** follow this Vres16 40-row placement.

## C. Static arithmetic checks actually performed

Executed independent integer checks (not a Windows build):
- `0x6400 / 640 = 40`.
- Image length `400×640 = 256,000`; converted region ends at byte 281,600 / output row 440.
- Top/bottom output rows = **40 each** in first 307,200-byte frame.
- `4×400×80 = 128,000` planar input bytes; each pixel index is assembled from 4 single-bit planes.
- `25,600 + 307,200 = 332,800 < 614,416` allocated bytes, confirming the overlong `VScreen` clear stays **within the containing allocation**, while crossing the staging-area boundary.

**Checks NOT performed:** Pixel capture, palette screenshot, actual buffer addresses/ownership under SDL locking, rendering with real files, geometry with alternative modes, C test suite, build or gameplay regression.

## D. Safeguards for future authorised diagnostic work

1. Confirm actual runtime `WScreen`/`VScreen`/`VGABuffer` pointers and SDL draw surface addresses at: app startup, mission load, mission rendering, briefing, company menu and mission exit.
2. Confirm mission content maps to output rows 40–439 in a baseline Windows screenshot, including pointer offsets and the behaviour of top/bottom borders.
3. Check whether any async/window/sound event can inspect the staging region during the converter clear and copy. Preserve original lock/unlock order and cursor.
4. Test palette/colour-index ownership for HUD, agents, effects, building tiles, vehicles, windows and signs using legally obtained original data.
5. Do not add graphics overlays, window scaling or camera-speed changes until pixel differences and mouse/control regressions are independently testable.

**Scope integrity:** The source and CI workflow remain unchanged. This report is a navigation aid for future proofs and a precise, asset-independent constraint on the requested v0.9 night lighting.
