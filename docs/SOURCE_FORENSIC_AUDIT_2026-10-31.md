# Syndicate 2026 — Forensic Source Architecture Audit

**Audit date:** 31 October 2026 (Australia/Brisbane)
**Status:** INITIAL SOURCE SURVEY COMPLETE; **FULL FORENSIC AUDIT NOT COMPLETE**. Do not use this as approval to implement graphics or lighting.
**Repository:** `mh83invest-creator/Syndicate-2026`
**Inspected branch:** `feature/v0.9-visual-showcase`
**Known-good source origin:** `main` at `a22b0b9580a9710b4e693e88854351835ef1ee40` (v0.7).
**Change freeze:** No new gameplay, camera, timing or graphics implementation authorised during this audit.
**Evidence basis:** actual GitHub file reads and existing `docs/releases/v0.7.md`, `docs/releases/v0.8.md`, `docs/releases/v0.8.1.md`, `docs/BUILD_CONTINUITY_2026-10-16.md` (the latter historical documents reside on their respective experimental branches), and `docs/releases/v0.9*.md`.
**Audit test status:** source reading only, no current compilation, instrumentation, test executable or Windows playtest.

## Executive findings and current risk

1. This is a hybrid *original-game assembly + C glue + Bullfrog support libraries* engine, not a new standalone C game. `engine/src/Makefile.am` explicitly includes `syndre.sx` and generated wrappers, links `../lib/libbullfrog.a` and `../lib/libbfsound.a`.
2. Existing display handles legacy 8-bit modes and VGA buffers. `engine/src/display.c` maps modes 19/18 to 320x200/640x480 8-bit surfaces; `swap_wscreen` blits original buffers before swapping.
3. Resolution-dependent mouse coordinate transforms are visible in `engine/src/mouse.c`: VRes16 uses screen coordinates with 40-pixel vertical offset, other modes double coordinates, and vertical input is clamped. Rendering/scaling alterations therefore require input-region regression testing.
4. `engine/src/main.c` chooses fullscreen and display mode, loads `load_files_vres16` or `load_files_mcga`, configures pointer sprites and initializes input/audio. Resource load paths must be traced before title/menu redesign.
5. `engine/src/guitext.c` loads GUI text from files and constructs a list of strings; this is a string resource loader, **not evidence of a high-level modern UI layout API**.
6. `engine/src/game.c` retains mission `game_update_full(true, 16)` and separate `game_update_menu` function. v0.7 documentation confirms three 30 FPS strategic menu paths; never globally change simulation cadence.
7. `engine/src/timer.c` draws timing from `SDL_GetTicks`; audio initialization is mediated by `engine/src/sound.c` and `bfsoundlib`. Changes to scheduling or event pumping are cross-system risks.
8. The assembly file `engine/src/syndre.sx` is present with Git blob SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf`, but GitHub's UTF-8 fetch returned an **empty content string**. Its full contents, procedures and dependencies were NOT inspected. This is a **critical audit blocker**.
9. GitHub build source uses GNU Autotools/Make, native Windows MinGW i686, SDL2, OpenAL Soft and WildMIDI. Reference is `.github/workflows/windows-source-v0.7.yml`; historic Windows compile passed, but no v0.9 build yet exists.
10. The v0.8 camera-speed experiment compiled but caused failed user gameplay (top-edge veering and desktop focus switching). A green CI run is insufficient to establish runtime safety.

## Files directly read and preliminary classification

| File | Observed responsibility | Inspection coverage |
| --- | --- | --- |
| `engine/Makefile.am` | Top-level subdirectories: bflibrary, bfsoundlib, conf, lang, src, util | Read |
| `engine/configure.ac` | Autoconf feature and build setup | Read; not fully audited |
| `engine/src/Makefile.am` | Engine source list, wrapper generation, static library linking, Windows DLL packaging | Read |
| `engine/src/main.c` | Process startup, CLI, rendering mode, data loading, input/audio startup | Read |
| `engine/src/game.c` | Core initialization, SDL events, pacing, quit and other coordination | Read |
| `engine/src/game.h` | High-level game declarations | Read |
| `engine/src/display.c` | Video mode mapping, VGA buffers, screen blits and sprite wrappers | Read |
| `engine/src/game_data.c` | Save/data paths and resource state | Read |
| `engine/src/guitext.c` | GUI string loading and indexing | Read |
| `engine/src/mouse.c` | Mouse/display coordinate conversion | Read |
| `engine/src/keyboard.c` | Keyboard handling and bridging | Read |
| `engine/src/timer.c` | SDL-backed tick functions | Read |
| `engine/src/sound.c` | Sound and audio platform coordination | Read; detailed signal-flow analysis pending |
| `engine/src/player.c` | Player wrappers | Read |
| `engine/src/research.c` | Research wrappers | Read |
| `engine/src/weapon.c` | Weapon/equipment wrappers | Read |
| `engine/src/wrcountry.c` | World/country wrappers | Read |
| `engine/src/dos.c` | DOS compatibility implementation | Read |
| `engine/src/oswindws.c` | Windows-specific platform path/service support | Read |
| `engine/conf/wrappers_game.conf` | Original calling conventions and bridge generation definitions | Read; cross-reference pending |
| `engine/src/syndre.sx` | Original game assembly | **UNREADABLE through connector; critical blocker** |
| `engine/README.md` | Upstream build, platform and retail-data setup | Read |
| `.github/workflows/windows-source-v0.7.yml` | i686 CI compiler/dependencies/artifacts | Read |

**IMPORTANT:** `bflibrary/`, `bfsoundlib/`, `lang/`, `util/`, most `conf/`, resource formats, and the complete repository tree **have not yet been enumerated or independently audited**. This is a representative source inspection, NOT a file-by-file codebase inventory.

## Architecture and dependency sketch (provisional)

```text
Native executable / CLI (src/main.c)
  -> process bootstrap (src/game.c)
  -> data paths and resources (src/game_data.c; original game data)
  -> 8-bit display / VGA buffers (src/display.c -> bflibrary)
  -> pointer/input (src/mouse.c, src/keyboard.c -> bflibrary)
  -> audio (src/sound.c -> bfsoundlib -> native OpenAL/WildMIDI)
  -> timing/events (src/game.c, src/timer.c -> SDL)
  -> original game logic and UI/mission rendering (src/syndre.sx)
       <-> generated assembly/C wrappers (conf/*.conf, util/mkwrappers)
       <-> game/player/research/weapon/country state (src/*.c + headers)
```

Edges above are inferred from build lists/includes and known handovers; **exact runtime call graph and ownership boundaries are not yet established**.

## Feasibility matrix — provisional, NOT permission to implement

| Feature | Current determination | Blocker / verification needed |
| --- | --- | --- |
| Native OS window title | Simple source-level change already exists in v0.9A branch; **not runtime verified** | Windows build and check title visibility |
| Title screen / menu styling | **Unknown complexity** | Decode original assembly UI functions, sprite/font/palette assets, hit testing, draw order |
| Company configuration / world map / briefing visual polish | **Unknown complexity** | Identify exact menu draw routines and input coordinate/palette interactions; retain three 30 FPS paths |
| Font sharpness / scaling | **Potentially coupled** | Audit bflibrary sprite/font rendering, scaling and mouse coordinates |
| Cursor/highlight changes | **Potentially coupled** | Audit pointer sprite loading, UI hit boxes, coordinate conversion and saved states |
| Night-time palette variants | **Unproven feasible** | Trace palettes, per-mission selection, palette writes, sprites and world rendering |
| Streetlights, headlight pools and neon | **Unproven feasible** | Audit mission objects/light ownership, sprite layering, palette depth and per-frame effects |
| Illuminated building windows | **Unproven feasible** | Asset geometry, window representation, palette/texture modes, map and sprite lighting |
| Per-mission day/dusk/night categories | **Unproven feasible** | Mission metadata, loader, persistence/data compatibility |
| Renderer overhaul or full dynamic lights | **Out of v0.9 scope** | Requires separate architecture/design approval |
| Camera-speed feature | **Excluded** | Failed v0.8 gameplay, preserve v0.7 code |

## Forensic audit work remaining (mandatory before v0.9A implementation)

1. Obtain a **byte-faithful source tree** / access that can decode `engine/src/syndre.sx`. GitHub connector text fetching cannot currently expose it. Do not infer internals from filenames or previous prototypes.
2. Enumerate every repository path and build input, including `bflibrary`, `bfsoundlib`, `conf`, `lang`, `util`, `res`, workflows and auxiliary source/assets. Record inventory with hashes, line counts and module ownership.
3. Build a static symbol, caller/callee and ABI map across C, generated wrappers and assembly. Identify UI/menu/briefing draw entry points, palette switches, FPS/menu dispatch and camera scrolling boundaries.
4. Trace graphics format loading to framebuffer to SDL display, including palette and sprite life cycle, scaling, selection/cursor draw order, clipping and dirty rectangles.
5. Trace mission environment and map data structures, sprites, vehicles, buildings, night/dusk states (if any), light affordances and asset legality.
6. Trace game simulation, AI, agent pathing, objectives, projectiles/combat, input/event pump, save/load and audio ownership, documenting places where presentation code can accidentally alter them.
7. Trace library and platform dependencies, Windows packaging, game data expectations and tests. Verify reproducibility on the **exact working branch**, never treating prior compile as current evidence.
8. Document exact file/line/symbol references for every proposed presentation change, plus regression tests for each dependency.
9. After audit, propose a scoped feasibility and implementation sequence for approval; do **not** develop unverified graphics in parallel.

## Testing / outstanding limitations

- GitHub connector source read and metadata: PASS for listed readable files.
- Total-file/whole-repository coverage: NOT ACHIEVED; no full tree listing through current GitHub connector.
- Original assembly inspection: BLOCKED by empty connector response.
- Static full-project compile, ABI/type checks and runtime profiling: NOT RUN.
- Windows 32-bit compile and DLL checks for v0.9: NOT RUN.
- Gameplay, GUI, audio, focus, scrolling and visual tests: NOT RUN.
- No source implementation files changed by this audit.
- Previous v0.9A experimental *window title* commit remains on branch; development frozen, no additional feature code changes.

## Handover: exact next steps

Stay on `feature/v0.9-visual-showcase`; protect `main` and user-tested v0.7 package. First resolve assembly retrieval and complete tree enumeration. Expand this audit with per-module findings and line/symbol references, then finish dependency mapping and feature feasibility review. Only after that, revisit v0.9A implementation scope. Keep all milestones separately documented; do not present this initial survey as a full source dissection.

## Changelog
- Initial source survey established real build layout, legacy framebuffer/input/asset coupling, identified original assembly access as critical blocker, and froze feature development pending a complete audit.

## Addendum — Assembly blocker resolved by Git blob retrieval

**Correction to earlier findings:** The original source survey described `engine/src/syndre.sx` as inaccessible because regular GitHub contents reads returned empty strings. **This limitation has now been bypassed:** the GitHub blob-by-SHA endpoint returned the entire 1,848,334-character source text (85,803 lines), blob SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf`. The earlier "unreadable" classifications above are historical, NOT the current status. A local Git checkout remains unavailable because the runtime cannot resolve github.com, so there has been no local compilation or full-tree clone.

### Extracted assembly symbol inventory (source evidence)

Simple lexical scan of original assembly discovered approximately **2,924 line-anchored named labels** (this count includes data labels and may not equal the number of procedures) and 73 `GLOBAL(...)`-style declarations including macro definitions. These counts are **not a semantic call graph**.

| Key symbol | Assembly line (1-based) | Why it matters |
| --- | ---: | --- |
| `mission_briefing` | 1207 | Mission briefing processing |
| `draw_brief_tact_map` | 2141 | Briefing tactical map presentation |
| `draw_config_menu` | 10925 | Company/config menu presentation |
| `draw_config_menu_box` | 11270 | Menu frame/panel candidate |
| `draw_logo` | 11326 | Branding/logo presentation candidate |
| `draw_tact_map` | 16259 | Tactical map drawing |
| `draw_mapwho` | 18029 | Map actor/entity drawing |
| `draw_element` | 19109 | Object rendering candidate |
| `draw_agent_element` | 19173 | Agent element drawing |
| `full_redraw` | 19285 | Redraw coordination |
| `scroll_map` | 19352 | Input-to-camera direction |
| `scroll` | 19459 | Camera movement implementation |
| `init_map_data` | 25694 | Map initialization |
| `load_map_level` | 27845 | Map/level loading |
| `map_selection` | 31709 | Strategic world/map selection |
| `_draw_map_area` | 32804 | Map area draw |
| `draw_panel` | 39036 | In-game panel |
| `draw_apanel` | 40659 | Additional panel |
| `DisplayScreenTitle` | 55935 | Screen title function |
| `DrawScreenGadget` | 55974 | UI control drawing |
| `DrawMenuGadget` | 56074 | Menu gadget drawing |
| `DoPalette` | 61958 | Palette operations |
| `ASM_LbPaletteSet` | 75203 | Palette application |
| `DrawASpriteHires16_A` | 76222 | High-resolution sprite drawing |
| `DrawSpriteLores16` | 77185 | Low-resolution sprite path |
| `mission_load_files` | 82620 | Mission resource list |
| `level_palettes` | 82677 | Palette data |
| `level__Vehicles` | 85726 | Vehicle state data |
| `level__Worlds` | 85742 | World-state data |
| `level__Objectives` | 85770 | Objective-state data |

**Critical confirmation:** Three calls to `ac_game_update_menu` appear at lines **2065, 10533 and 31936**. These are the three special 30 FPS menu call sites described in the v0.7 handover. Leave their dispatch alone.

The symbol-name scan did **not** find named labels specifically for streetlights, neon, illuminated windows, or dynamic light volumes. **Absence of matching label names does not establish absence of the feature**; unnamed `func_*` procedures, assets, palette handling and object rendering still require deeper analysis.

### Updated feasibility and regression implications

- Menu polish is now traceable to concrete assembly functions (`draw_config_menu`, `draw_logo`, `DrawMenuGadget`, `DisplayScreenTitle`, `draw_brief_tact_map`, `map_selection`), but changing these is not approved until actual draw buffer, hit boxes, palette changes and asset data references are mapped.
- Night lighting is still technically **unproven**. There are identifiable candidate render/palette/map symbols, but no demonstrated per-object light system. Actual implementation must account for 8-bit indexed colour and asset compatibility.
- Camera functions `scroll_map` and `scroll` and all three `ac_game_update_menu` sites are regression guardrails. The exact failed v0.8 wrapper code must remain excluded.
- `engine/src/mouse.c` resolution-dependent coordinates and pointer initialization in `main.c` require cross-mode mouse hitbox tests if sprite or menu scaling changes.
- The 85,803-line assembly source now makes comprehensive static inspection possible via the blob connector, **but a full semantic dissection of every block has not been completed**.

### Remaining work after blocker removal

1. Obtain full repository tree listing and byte-faithful local files for support libraries (`bflibrary`, `bfsoundlib`), assets/formats, translation, utilities and config. Git checkout failed from this environment (DNS resolution error); a GitHub connector file list/tree endpoint was not exposed.
2. Conduct focused source slices for each assembly subsystem and create a call/dependency graph, including unnamed `func_*` routines and actual parameter/state dependencies.
3. Determine how original retail assets encode map tiles, windows, signs, headlamps, palettes, selection visuals and sprites, without committing proprietary data.
4. Produce feature-by-feature implementation feasibility and test plans only after verifying actual render and data paths.
5. Keep v0.9A feature implementation paused; no additional game source changes were made during this audit.

### Verification of this addendum

GitHub original assembly blob retrieval: **PASS**. Symbol/label occurrence scan: **PASS (lexical only)**. Three menu-call-site check: **PASS**. Full semantic analysis: **NOT COMPLETE**. Full checkout: **BLOCKED**. Windows compile and gameplay: **NOT RUN**.

## Addendum — Complete GitHub tree enumeration and SDL2 backend tracing (31 October 2026)

**Decision:** Retain the SyndicatFX-derived engine as the sole Syndicate 2026 implementation. FreeSynd (`CeRiAl/freesynd`) may be downloaded by the user and consulted as a **reference only**. Do not migrate engines, copy its source, merge its history, or assume gameplay equivalence. No gameplay code changes authorised during the audit.

### Repository inventory: previous tree-listing blocker resolved

GitHub's approved `git/trees/feature/v0.9-visual-showcase?recursive=1` endpoint returned `truncated=false`, **413 tree entries / 386 tracked blobs** for the audited branch. The main code groups include:

| Tree prefix | Tracked blobs | Coverage this pass |
| --- | ---: | --- |
| `engine/bflibrary/src/` | 99 | Selected graphics backends and generic graphics inspected |
| `engine/bflibrary/tests/` | 74 | Enumerated, individual tests not reviewed or run |
| `engine/bflibrary/include/` | 53 | `bfscreen.h` examined |
| `engine/src/` | 38 | All identified C source modules previously read; assembly symbol survey |
| `engine/bfsoundlib/src/` | 33 | Enumerated, detailed audit pending |
| `engine/bfsoundlib/include/` | 29 | Enumerated, detailed audit pending |
| `engine/conf/` | 8 | Wrappers game config previously examined |
| `engine/lang/`, `engine/util/`, `engine/res/` | 7 / 7 / 5 | Enumerated, detailed audit pending |

Counts are computed from tracked blob paths in the returned tree, not independent validation of file contents or runtime dependencies. The earlier document's claim that full tree enumeration was unavailable is now superseded. Local compilation and runtime inspection remain unavailable/not performed.

### Windows SDL2 screen and palette — verified code paths

The following paths exist in the actual branch and were read:

- `engine/bflibrary/src/x86-win-sdl2/sscreen.c` (~40,994 characters): at lines **497–499**, an SDL window is created using `SDL_CreateWindow(lbDrawAreaTitle,...)`. At **1050 onwards**, `LbScreenSwap` drives surface swapping with `LbMouseOnBeginSwap` / `LbMouseOnEndSwap`. At **1170–1172**, an update path calls `SDL_UpdateWindowSurface`. It is a **software surface / screen-update path**, not evidence of a GPU shader or modern programmable lighting pipeline.
- `engine/bflibrary/src/x86-win-sdl2/spalette.c` (~3,492 characters): `LbPaletteSet` at **50** converts palette data to `SDL_Color`, and at **82–94** calls `SDL_SetPaletteColors` for screen/draw surfaces when appropriate. This confirms an **indexed palette system**, important for lighting feasibility and risk of globally recolouring UI, sprites and missions.
- `engine/bflibrary/src/general/gpalette.c` (~9,579 characters): shared palette operations; `engine/bflibrary/src/general/gspr_map.c` (~12,337 characters): mapped sprite drawing paths; `engine/bflibrary/src/general/gfont.c` (~4,774 characters): shared font operations; `engine/bflibrary/include/bfscreen.h` (~20,590 characters): public screen API. These have been read and relevant function names located; complete contracts/call graphs remain to be established.
- Engine display integration remains in `engine/src/display.c` and original assembly. A new lighting subsystem is **not** established merely because the SDL2 backend can show colours.

**Practical implication for v0.9A:** The direct sprite/font/palette draw paths are observable. Redrawing controls or selecting a new rendering mode without testing mouse coordinates, palette state and native surface swaps risks regressions. Avoid speculative changes.

**Practical implication for v0.9B:** Palette-based atmospheric darkening may be a technical exploration candidate, but a *global* palette shift would also change other content and is not approved as a finished solution. Individually localised pools of light, headlights, bright windows and neon must be demonstrated with verified map rendering and layering data. There is still no evidence of native per-pixel dynamic lighting.

### Audit test evidence for this addendum

- GitHub recursive tree retrieval: **PASS**, non-truncated, 386 file entries.
- SDL2 backend/palette source reads: **PASS** for named paths and line locations.
- Indexed palette and SDL software-surface code paths: **CONFIRMED** from source.
- Whole-codebase semantic review: **NOT COMPLETE**.
- FreeSynd files downloaded or incorporated: **NO**.
- Native Windows build, SDL runtime surface behaviour, screenshots and gameplay regression tests: **NOT RUN**.
- Game source changes: **NONE**.

### Updated next forensic tasks

1. Audit `engine/bflibrary/src/x86-win-sdl2/sscreen.c` draw/copy paths fully and cross-reference generic palette, sprite and font APIs to assembly UI calls.
2. Inspect `engine/bfsoundlib` dependencies, runtime audio call paths and startup/shutdown transitions.
3. Audit map/level file loading, tile/object/sprite composition, and separate game/GUI palette lifetimes; identify feasible per-mission atmosphere hooks without changing gameplay.
4. Inventory and inspect automated tests and CI workflow configuration, then form a traceable test matrix. Record **which** checks can run without proprietary retail files and which need Windows user testing.
5. Continue semantic assembly call-graph tracing (including unnamed `func_*` routines) before proposing v0.9A/B code edits.

## Addendum — Mission map and sprite-rendering cross-reference (31 October 2026)

**Working-engine decision unchanged:** SyndicatFX v0.7 remains the sole implementation. The user provided `freesynd-master.zip` as an external reference. The archive was successfully opened locally for read-only examination; **none of it was copied into Syndicate 2026 or committed to GitHub**. The FreeSynd snapshot is a separate project's code, not a verified substitute for the current engine.

### FreeSynd original-resource reference: verified

The supplied ZIP contains a FreeSynd source tree. Concrete functions inspected:

| Reference source | Lines | Actual code evidence |
| --- | --- | --- |
| `freesynd/src/mapmanager.cpp` | 57–79 | `MapManager::loadMap` constructs `map%02d.dat`, calls `File::loadOriginalFile`, populates a `Map`, and caches by map ID. |
| `freesynd/src/map.cpp` | 47–87 | `Map::loadMap` reads little-endian X/Y/Z dimensions at offsets 0/4/8, a column lookup table beginning at offset 12, and tile IDs by column and height. |
| `freesynd/src/menus/maprenderer.cpp` | approx. 55–122 | `MapRenderer::render` traverses isometric tile positions and draws visible map tiles, then objects associated with each tile. |
| `freesynd/src/menus/maprenderer.cpp` | 221 onwards | `drawObjectsOnTile` draws queued `MapObject` instances; `addObjectToDraw` accounts for vehicle elevation and orders same-tile objects front-to-back using relationship checks. |
| `freesynd/src/gfx/tile.cpp` | 47 and 80 | Tile drawing methods `Tile::drawTo` and `Tile::drawToScreen`. |

**Interpretation:** The original map format has independently documented layered tile structure in FreeSynd. Its renderer provides a useful conceptual reference for scene ordering and occlusion. **This is not yet a verified exact format/algorithm match to SyndicatFX**; original assembly/map loader and asset samples must be checked before transferring assumptions. The user-supplied archive was not built or run.

### Current SyndicatFX graphics capabilities: additional verified references

| Current engine source | Lines | Meaning and caution |
| --- | --- | --- |
| `engine/bflibrary/src/general/gspr_map.c` | 198, 286, 390, 418 | Contains `LbSpriteDrawTrRemap`, `LbSpriteDrawSlRemap`, `LbSpriteDrawFCRemap` and `LbSpriteDrawRemap`. These are colour-remapping sprite operations. Their existence suggests palette-index remapping can occur on individual sprites, but does **not** demonstrate lighting or correct map-object integration. |
| `engine/bflibrary/src/general/gpalette.c` | 38, 180, 276–288 | Contains `LbPaletteFade` and palette-colour lookup functions. Global palette fading is distinct from localized light and could affect UI, agents or the entire scene. |
| `engine/src/display.c` | 234–255 | `blit_wscreen_vres16`, `blit_wscreen_mcga` and `swap_wscreen` copy the original framebuffer and then call `LbScreenSwap`. Any proposed overlay has to choose an appropriate stage before/after blitting and respect buffer locks. |
| `engine/src/game_data.c` | 95, 113 | Original file-name transformation and filename setup; map file loading inside original assembly still requires deeper inspection. |
| `engine/src/game_data.c` | 141–190 | Player-save serialization writes/reads multiple existing state structures in a fixed sequence. Adding atmosphere settings into these structs would risk save compatibility; do not change their layout without separate format study. |

### Feasibility update — still preliminary

1. **Selective sprite colour remapping:** supported at the *library API level*. Unknown whether and where it can safely shade map objects independently in the original mission render loop.
2. **Whole-palette day/night tint:** technically conceivable given palette APIs, but risky and does not satisfy required localized sources. Do not treat it as v0.9B complete.
3. **Per-pixel streetlight pools / headlights / emissive windows / neon:** **not established**. Need original-map rendering order, sprite layering, palette-index semantics, possible mask/overlay draw functions and real asset representation.
4. **Mission-specific atmosphere settings:** unknown; must inspect mission metadata and original level loading. Do not expand saved structures blindly.
5. **Visual UI improvements:** still require cross-checking `DrawMenuGadget`, `draw_config_menu`, graphics resources and mouse hit boxes; no UI gameplay code changes were made.

### Additional audit tasks and regression gates

- Trace assembly `init_map_data`, `load_map_level`, `draw_element`, `draw_mapwho`, `draw_panel`, `full_redraw` and `DoPalette` call sites and data flows.
- Establish the exact mapping between original `mapNN.dat` data and SyndicatFX level/map buffers; compare against FreeSynd only as reference.
- Enumerate existing sprite-remap consumers to establish if the API is already used in mission rendering or only generic library code.
- Test palette lifetime boundaries across mission, menus and briefings in source; identify safeguards for maintaining readable interface and actors.
- Form minimal isolated tests for palette remapping and scene overlay drawing, then build them on a separate branch only after the forensic feasibility audit.
- Keep `main`, the camera correction, 16 FPS mission update, three 30 FPS strategic menu calls, input/audio and original gameplay entirely unchanged.

### Verification

- User-provided FreeSynd ZIP: **read-only extraction and source inspection passed**.
- SyndicatFX graphics/data C source: **GitHub source reads passed** for the named paths.
- This is a **static source cross-reference**; no matching retail map-data samples were analysed and no equivalence asserted.
- No gameplay code changes; no Windows compile, new build, screenshots or playtests.

## Addendum — Source audit synthesis: mission assets, rendering dependency gates, audio and test inventory

**Scope:** This closes the **read-only architectural reconnaissance stage** and records current feature gates. It is **NOT** a complete proof of every function across all 386 files, a compiled verification of the branch, or a green light to implement night lighting. Source inspections cited here use the GitHub blob `c35e22621a93d1e07c44d40de8867446ff7c64bf` for original assembly; line numbers refer to that 85,803-line file.

### Concrete assembly call chains and original data loading

| Source and location | Direct evidence | Implication |
| --- | --- | --- |
| `engine/src/syndre.sx:27845–27935`, `load_map_level` | Calls `clear_savegame`, `LbFileReadRNC`, `ac_LbDataLoadAll`, `ac_sound_bank_setup`, `init_hires_blocks`, `init_level_data`; references `level__Seed`, `level__MapNumber`, `mission_load_files`, `WScreen`, `VScreen` | Mission/map load is coupled to original level state, compressed resource reading, sprite blocks and audio banks. A per-mission visual treatment must not corrupt these lifetimes. |
| `engine/src/syndre.sx:82620–82673`, `mission_load_files` | Load table includes `data/map`, `map_buf`, `h_col`, `h_blocks`, `h_sprites_data`, `h_sprites`, `frames`, `elements_ani`, `starts_ani` | Rendering depends on original map, colour, block, sprite and animation resources; original asset formats require further field decoding. |
| `engine/src/syndre.sx:19109–19172`, `draw_element` | References `h_sprites`, calls `DrawASpriteHires16_A` | Confirms a map-element sprite draw path; not evidence of emissive textures or lights. |
| `engine/src/syndre.sx:18029–18715`, `draw_mapwho` | References map actor and people arrays, viewport coordinates; calls `LbScreenSetGraphicsWindow`, `DrawASpriteHires16_A`, `set_block_changes`, `__cover_sprite` | Entity draw order, screen clipping and occlusion must be audited before any per-object overlay. |
| `engine/src/syndre.sx:19285–19350`, `full_redraw` | Calls `draw_horizontal`, `set_all_changes` and references map boundaries | Camera, tile redraw and change tracking are intertwined. No presentation change should affect these without precise testing. |
| `engine/src/syndre.sx:39036–39215`, `draw_panel` | Calls `draw_apanel`, `draw_bars`, `draw_tact_map`, `change_mouse_pointer`, text and window helpers | Gameplay panel draws are coupled to cursor and map state. Maintain readability and input. |
| `engine/src/syndre.sx:61958–62031`, `DoPalette` | References `PalMem`, calls `ac_LbPaletteSet` | Palette changes are original-game operations; effects could alter UI and scene simultaneously. |
| `engine/src/syndre.sx:19352–19458`, `scroll_map` | Dispatches directly to `scroll` for camera directions | Preserve known-good v0.7 camera calling semantics. |
| `engine/src/syndre.sx:19459+`, `scroll` | References `player_view_map_x/y`, calls `draw_horizontal`, `set_all_changes` | Camera speed and redraw are coupled; never import failed v0.8 step wrappers. |

These are **local routine references**, not a fully resolved dynamic execution trace.

### Palette and rendering conclusions

- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:878, 924, 976, 1050–1084` contains custom blit/scaling paths, `LbI_SDL_BlitScaled`, and `LbScreenSwap`; the swap blits a draw surface and calls `SDL_UpdateWindowSurface`. No native shader/lighting pipeline has been demonstrated.
- `engine/bflibrary/src/x86-win-sdl2/spalette.c:50–99` applies indexed palette colour data to SDL screen/draw surfaces. This is useful for palette handling but **global palette edits alone cannot deliver distinct localized lights**.
- `engine/bflibrary/src/general/gspr_map.c:198,286,390,418` exposes mapped-sprite drawing variants, which may provide visual colour transforms at sprite scale. They do not establish occlusion-safe streetlight pools, headlights or window glow.
- **Technical risk ranking (preliminary):** text/panel resource styling = medium (assembly/UI coupling); font/sprite scaling = medium–high (mouse coordinate/hitbox coupling); palette-wide night tint = high for legibility/UI colours; per-source night lighting = high/unknown pending actual asset/draw-order and output testing. No item is approved for implementation by this rating.

### Audio and timing evidence

- `engine/bfsoundlib/src/drv_oal.c:102` opens an OpenAL device with `alcOpenDevice`; other visible paths manage OpenAL sound sources, buffers and playback.
- `engine/bfsoundlib/src/init_mus.c:229` opens an XMIDI driver, while `init_snd.c:181` references `sound.dat`; `init_mus.c:84` references `music.dat`.
- `engine/bfsoundlib/src/bfaudio.c:118+` performs audio initialization and configures paths/state. Legacy AIL, sound-bank and native audio-device functionality have dependencies across multiple libraries.
- `engine/src/game.c:142–193` implements SDL tick/delay-based pacing; `game_update` at 205–207 invokes `game_update_full(true,16)`; `game_update_menu` at 224–226 uses 30. Three original-assembly menu call sites remain verified.
- **Regression guardrail:** visual work must not change the event pump, tick pacing, library initialization, MIDI/audio processing or native DLL dependency packaging.

### Repository test and build inventory

- Recursive tracked tree returned `413` entries, `386` tracked blobs, `truncated=false`.
- `engine/bflibrary/tests/` contains 74 files, including test sources for math, polygon/line/triangle and sprite drawing, test helpers/mocks, and reference PNG images. **Presence does not imply tests have run.**
- Windows source build workflow: `.github/workflows/windows-source-v0.7.yml`; source restore workflow: `.github/workflows/restore-v0.7-source.yml`. Upstream engine has additional `engine/.github/workflows/build.yaml` and `build-sdl1.yaml` workflows. Existing Windows v0.7 CI is historical baseline proof, not v0.9 build evidence.
- Public `engine/res/` provides small icons and Windows resource declarations, **not commercial mission art**. Original retail map/mission resources remain outside public GitHub.
- No Windows test game data, current executable run, screenshots, benchmark or current v0.9 CI evidence has been inspected.

### FreeSynd role and evidence limits

The user-supplied FreeSynd ZIP is reference-only. Its `src/mapmanager.cpp` and `src/map.cpp` document `map%02d.dat` loading and 3D tile-layer indexing, and `src/menus/maprenderer.cpp` documents painter-style tile/object composition. This supports conceptual resource mapping, not byte-for-byte equivalence or drop-in code. No FreeSynd code is committed or approved for integration.

### Completion classification and user dependency

**Read-only reconnaissance completed:** full tracked-file inventory, original assembly retrieval and key symbol/call-site mapping, core C module identification, SDL2 indexed-palette/screen backend, reference asset decoding, audio/dependency scan and available test/workflow inventory.

**Full forensic audit not complete:** no line-by-line semantic proof for every one of the 386 tracked files, no decompiled unnamed assembly call graph, no original retail map/sample bytes to verify exact formats and window/sign materials, no runtime frame captures, no build/CI on the v0.9 development branch, no performance measurements. Therefore definitive feasibility or safety of proposed lighting and UI features is **not established**.

**User action required for next evidence tier:** provide a legally owned, nonpublic *local test environment* or representative original Syndicate mission/map/sprite/palette assets through an appropriate private channel; do **not** commit retail game files to public GitHub. Also a Windows playable test environment is required for end-to-end frame/interaction verification. Without these, only further static code inspection is possible.

**Development decision:** do not begin v0.9A graphics or v0.9B lighting implementation. Preserve `main`, v0.7 camera, timing and audio. The preexisting v0.9A native title string edit remains unverified, and has not been expanded during the audit.


## Continuation reference — 1 November 2026

A separate, evidence-based static deep dive has been committed at **`docs/SOURCE_AUDIT_RENDERING_AND_TEST_GATES_2026-11-01.md`**. Read it alongside this original audit before further technical work. It records the distinct **16-index planar mission render path**, sprite/framebuffer ownership, menu/mouse/assembly ABI relations, original camera edge normalization, direct UI and mission render call sites, and a specific CI testing gap (**Automake declares seven executable tests through `TESTS = $(check_PROGRAMS)`, while current v0.7 Windows workflow compiles but does not invoke `make check`**). It also defines a preliminary feature-feasibility matrix and test prerequisites. No game or workflow source was changed for this read-only audit.

## Continuation — Asset-free source audit advanced, 1 November 2026

**New permanent companion reports on this branch:**

- `docs/ASM_ADDRESS_LABEL_INDEX_2026-11-01.md` — machine-generated, **lexical** index of 1,617 original address-annotated assembly labels (code/data/unclear heuristics, source offsets, call-site counts). Does **not** certify 1,617 functions or a complete call graph.
- `docs/SOURCE_AUDIT_ASSET_FREE_CONTINUATION_2026-11-01.md` — manually traced actual mission frame lifecycle, double `scroll_map` dispatch, 16-colour planar to SDL presentation, original per-level `hpalXX.dat` palette selection, original high/low resource tables and allocation pointers, title/UI asset route, SDL mouse/focus paths, save-data invariants, known-good branch file-hash comparison, and quantified converter iteration count.

**Key preservation finding:** Git blob comparisons between current audit branch and `main` showed original assembly, camera/input source, display source, SDL2 screen and palette backends, save-data implementation and v0.7 Windows workflow **identical**. Only `engine/src/game.c:85` differed in the checked source set, reflecting the earlier, **unbuilt** `LbSetTitle("Syndicate 2026")` update. This is a selected-file comparison, not proof that every branch file is identical.

**Important risk:** The mission loop performs `swap_screen_vres16` before scroll/copy-back/actor/HUD draws for the next visible cycle, and low-level palette allocation/aliases differ by screen phase. A universal scene-darkening hook or arbitrary sprite-remap insertion risks affecting HUD, cursor, colour transitions, and redraw order. The existing 8-bit sprite remapping API cannot simply be assumed compatible with the four-planar mission rendering path.

**Audit status:** Static, read-only source reconnaissance has materially progressed. Nevertheless, **the full codebase audit is still not finished**, and no lighting/UI implementation is authorised. Original retail assets and actual Windows frame/playtest evidence will be needed for conclusive feasibility. Further code-only investigation remains possible without the user's immediate involvement.
