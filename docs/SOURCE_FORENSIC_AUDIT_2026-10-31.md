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
