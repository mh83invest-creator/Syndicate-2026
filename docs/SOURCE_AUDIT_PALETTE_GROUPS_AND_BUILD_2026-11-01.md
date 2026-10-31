# Syndicate 2026 — Forensic audit: actual palette grouping, render ownership, and build constraints

**Date:** 2026-11-01 (Australia/Brisbane)
**Status:** Read-only **source-level findings**, not runtime verification or authorization to implement effects.
**Repository:** `mh83invest-creator/Syndicate-2026`
**Branch:** `feature/v0.9-visual-showcase` (do not change gameplay source); known-good fallback v0.7 on `main` at `a22b0b9580a9710b4e693e88854351835ef1ee40`.
**Assembly evidence:** `engine/src/syndre.sx` Git blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`, 85,803 lines. `engine/src/display.c`, `display.h`, `bflibrary`, `src/Makefile.am`, `configure.ac` and CI workflow read from the named branch.
**External reference:** User-provided `freesynd-master.zip`, inspected **locally only**; do not copy or merge it.

## Finding A: built-in level palette selection is highly shared

In the original source:

- `engine/src/syndre.sx:27768–27784`: `init_level_data` uses `current_levno` to index `level_palettes`, formats a `data/hpal%02d.dat` path, and loads the palette.
- `engine/src/syndre.sx:27759–27766`: multiplayer instead formats `data/hpal01.dat`.
- `engine/src/syndre.sx:82677–82684`: byte table consists of **50 nonzero values**, exactly **1,2,3,4,5 repeated ten times**, followed by two zero bytes.
- The above counts were confirmed with an independent programmatic parse of the literal `\xNN` bytes: 52 total (10 copies of each value 1–5, two zeros).

**Important consequence:** Editing any of the five original palette files in place could affect *multiple* missions that reference the same palette, rather than just the requested 2–3 night missions. A per-mission night effect therefore **cannot be safely scoped by replacing a shared hpal file alone**. Do not treat the first five files as an existing five-mode day/dusk/night system: those numbers are original palette-resource IDs, not proved lighting classifications.

The precise mission-to-palette assignment, valid `current_levno` range and original palette colours still require retail asset/runtime evidence. The repeating table establishes sharing, **not a verified map of which specific missions use which palette**.

## Finding B: assembly sprite draw target contradicts simplistic buffer ownership assumptions

- `engine/src/display.h:39–52` comments characterize original `WScreen` as holding static/scrolling elements and `VScreen` as drawing sprites.
- **But** `engine/src/syndre.sx:76222–76246`, `DrawASpriteHires16_A`, loads `WScreen` into `%edi` and calls `DrawSpriteLores16`. This is actual assembly code; it is **not proof that all mission sprites are written exclusively to `VScreen`**.
- `engine/src/syndre.sx:76249–76274`, `DrawASpriteLores16_B`, instead targets `UScreen`.
- `engine/src/syndre.sx:17894–17974`, `swap_screen_vres16`, executes the original cursor/dirty-marking path, followed by non-DOS `ac_update_vscreen_whole_vres16` and `ac_swap_wscreen`.
- `engine/src/display.c:149–231` expands original **four bit planes** into palette indices in `VScreen`; `display.c:244–260` copies `VGABuffer` to the SDL presentation buffer and invokes `LbScreenSwap`. `AppScreenSetup` at lines 72–95 maps original video modes to SDL-compatible 8-bit surfaces.

**Consequence:** `WScreen`, `VScreen`, `UScreen`, `VGABuffer` and the SDL draw/window surfaces have different *phase-dependent roles*. The C header comment cannot be treated as the whole runtime story. Do **not** inject persistent sprites, lights or a background tint based only on the names of these variables. Verify actual writes and buffer life cycle across level/menu/mode transitions first.

## Finding C: the existing 8-bit sprite remapper is not automatically usable inside 4-plane missions

- `engine/bflibrary/src/general/gspr_map.c:418–435` implements `LbSpriteDrawRemap` against the Bullfrog 8-bit drawing abstraction and branches on transparency/flip drawing flags.
- `engine/bflibrary/tests/bflib_test_sprdrw.c:125–135,331–503` explicitly exercises the 8-bit remap function using generated/mocked sprite data and PNG references.
- An exact lexical scan of the original 85,803-line `syndre.sx` found **zero occurrences** of `LbSpriteDrawRemap` or `ac_LbSpriteDrawRemap`, while `DrawASpriteHires16_A` has multiple uses. This is a source text finding, not a proof that no indirect wrapper ever calls the remapper.

**Consequence:** A functioning generic remap API is **not evidence of existing per-object night tinting in the mission renderer**. An adapter, different compositing stage, or custom renderer may be required. This remains unimplemented and unproven.

## Finding D: title/menu artwork, version identifiers and build wiring are distinct concerns

- `engine/src/syndre.sx:82467–82511` contains **different** retail menu resource paths: `moption.dat`, `mconfup.dat`, `mmap.dat`, `mselect.dat`, `mdebrief.dat`, `mbrief.dat`, `mresrch.dat`, `mtitle.dat`, etc. These are not part of the public `engine/res` icon assets.
- `engine/src/syndre.sx:54274–54294` performs title-animation/menu sequencing using `AniPlay`; this is independent of `LbSetTitle` in `engine/src/game.c`.
- `engine/configure.ac:5–15` still defines **upstream SyndicatFX 0.0.6** as its Autoconf package version with a Git-based build suffix when available. This is distinct from the **project's release labels v0.7/v0.9**. `engine/src/main.c:294–296` prints the upstream version. Do not call an unchanged package version a v0.9 functional release.
- `engine/configure.ac` uses a compiled **i386 assembly** probe, branches into MinGW host handling, checks SDL, OpenAL, WildMIDI, libpng and Vorbis, and configures Bullfrog support libraries.
- `engine/src/Makefile.am:4,44,60–79` generates native calling-convention wrappers through `util/mkwrappers`, assembles `syndre.sx` and links `libbullfrog.a` and `libbfsound.a`. `engine/conf/wrappers_game.conf:236–240` declares bridges for `game_update_menu`, `game_update`, `swap_wscreen`, `update_vscreen_whole_vres16`.
- `.github/workflows/windows-source-v0.7.yml:3–6` has push trigger on `main` and manual dispatch; it compiles i686 and checks native DLLs. It **does not test the v0.9 branch** as a consequence of pushes. No v0.9 executable, native startup or game playtest has been verified.

**Consequence:** No menu facelift can be honestly considered achieved by the single preexisting OS-window-title edit. A protected v0.7 CI workflow and a v0.9 handover are not sufficient runtime test evidence.

## Finding E: SDL2 includes an actual off-screen surface API, not just one monolithic framebuffer

Read `engine/bflibrary/src/x86-win-sdl2/sscrsurf.c:31–206`:

- `LbScreenSurfaceCreate` creates SDL software surfaces matching the draw surface's pixel format.
- `LbScreenSurfaceBlit` implements off-screen-to-draw and draw-to-off-screen `SDL_BlitSurface`, optional colour keying, and a temporary 8-bit palette assignment/restoration around blits.
- This establishes an **existing off-screen compositing mechanism at the library layer**. It does *not* establish a scene-depth mask, emissive map, alpha-lit polygons, 32-bit framebuffer for missions, or a performance-safe real-time lighting overlay.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:1050–1085` handles final software blit and `SDL_UpdateWindowSurface`, with mouse begin/end swap handlers.
- For any lighting experiment, the prospective stage must be tested against mode switches, the cursor, UI panels, 4-plane conversion, palette colour mapping, hidden objects, and dirty-region invalidation. No implementation choice is endorsed yet.

## Finding F: FreeSynd is useful but must not be treated as ground truth

The user's separate `freesynd-master.zip` contains the C++ remake. Its `src/map.cpp` and `src/menus/maprenderer.cpp` are useful references for original map dimensions and layered rendering.

**Concrete bug in the reference (not in SyndicatFX):**
- `src/gfx/tile.h:31–32` defines `TILE_WIDTH = 64` and `TILE_HEIGHT = 48`.
- `src/gfx/tilemanager.cpp:125–130` checks whether a tile contains any visible pixel using `a_tile_data[h * TILE_HEIGHT + w]`. For a row-major 64-wide tile this uses height `48` as the stride rather than width `64`.
- **Static/synthetic verification:** with all 3,072 pixels transparent except the final one at offset 3,071, the current loop reads at most offset 2,319; it incorrectly classifies the tile as fully transparent. A loop using `h * TILE_WIDTH + w` correctly detects the visible pixel.
- This is a specific demonstrable issue **in the supplied reference**, not evidence that our engine has the same defect. FreeSynd should remain an independent conceptual resource; do not copy code without semantic and licensing review.

## Provisional feature feasibility and acceptance blockers

| Requested v0.9 feature | Can it be implemented with existing source as-is? | Evidence/next gate |
| --- | --- | --- |
| OS window title | Earlier one-line change exists, still **not built or tested** | i686 compile and Windows visibility |
| Title/menu style | In-game drawing and .dat resources located; **modification scope not yet verified** | Retail menu data/screenshots, font and mouse-region mapping |
| Mission-specific palette colour atmosphere | **Resource selection is possible in source; current palette IDs are shared** | Per-mission mapping, original colours, preservation of HUD and agents |
| Streetlights/headlights/neon/illuminated windows | **Not demonstrated**; no native light emitter model established | World-object geometry, compositing hook, occlusion and performance |
| Persisted day/dusk/night modes | **Not demonstrated** | Noninvasive runtime selection, save-format preservation |
| Use existing `LbSpriteDrawRemap` directly in 4-plane render loop | **Not established**; no direct call sites in assembly | Pixel layout integration and validation |
| Modern GPU shaders | **Not present** in inspected SDL window-surface path | New renderer/architecture proposal would be separate scope |
| Replacing SyndicatFX with FreeSynd | **Explicitly excluded** | User decided to retain existing engine |

### Outstanding work and validation honesty

Completed *in this pass*: source palette table parsed and counted; title/menu resource strings identified; exact original sprite draw-target assembly inspected; SDL off-screen blit implementation read; package/build/ABI path traced; one FreeSynd reference-only defect confirmed with a synthetic example.

Not complete: all 386 files' semantic analysis, untangled buffer alias graph across every screen phase, original commercial assets, live window surfaces/palettes, screenshot comparison, Windows compilation, make-check execution, menu/gameplay/night-time tests, performance measurements. No project engine code or workflow changed.

**Decision unchanged:** Continue read-only audit. Protect main/v0.7, camera/focus, FPS and audio. The above is feasibility evidence, not approval to implement untested graphics.
