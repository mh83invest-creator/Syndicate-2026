# Syndicate 2026 — Gameplay, AI, Effects and Audio Dependency Audit (static)

**Date:** 2026-11-01 (Australia/Brisbane)
**Branch:** `feature/v0.9-visual-showcase`; **v0.7 `main` remains the gameplay baseline**.
**Methods:** GitHub source reads and textual direct-call scanning of `engine/src/syndre.sx` blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`. Assembly line references are 1-based. Call lists below represent **static source calls within identified address-annotated routine extents**, not proof of all possible runtime execution. Indirect calls, jump tables, data labels and branch conditions require deeper semantic review. No code edits, compilation or runtime measurements.

## 1. Mission update ownership: original assembly, not C wrapper

Original `syndicate` game loop at `engine/src/syndre.sx:198–303` runs input/player/network/AI, movement, end-of-level, frame swapping and camera/redraw before its C `ac_game_update` tick. `engine/src/game.c:205–207` invokes the 16 FPS-paced path. **Changing this order affects gameplay, frame latency and camera; it is excluded from v0.9.**

At `engine/src/syndre.sx:33330–33339`, `move_it` explicitly calls the five simulation subsystems below in this order:

1. `move_worlds`
2. `move_people`
3. `move_weapons`
4. `move_effects`
5. `move_objects`

This is an exact observed call sequence, not a measured per-subsystem time budget. Even graphics-only changes that alter simulation timing can cause a different gameplay feel.

## 2. Object, agent and combat dependencies

| Routine and lines | Direct evidence | Regression boundary |
| --- | --- | --- |
| `process_computer_players` 7493–7576 | Delegates to `CPDoTurn` | Mission AI/computer behaviour |
| `CPDoObjectiveStep` 8467–8899 | Calls objective processing, `CPFollowAndAttackTarget`, shoot/person and vehicle functions, distance checks | Agent targets, objectives and attack logic |
| `CPCompletedObjectiveStep` 9031–9362 | Calls `CPSetupDoMissionObjective`, `CPGetFirstObjectiveInList`, `process_action` | Win/loss/objective progression |
| `process_players_turn` 38397–38598 | Calls panel/weapon checks, `make_packet` | Player input, squad weapon actions and network packet boundary |
| `single_play` 28088–28104 | Calls `process_action` | Control dispatch |
| `multi_play` 28106–28215 | Calls network player count/exchange, action processing and redraw-change functions | Network-specific behaviour, do not conflate with solo play |
| `process_action` 28450–30335 | Contains direct calls to mission loading, panel setup, agent commands, weapon choice/usage, ammo effects and sound | Gameplay/weapon actions can initiate effects and resources |
| `move_people` 49268–49798 | Dispatches named person-state handlers: walking, boarding/exiting vehicles, weapons, bullets, fire, explosions, dying/dead, guarding, wandering | Simulation state transitions and damage |
| `person_intel` 45435–45899 | Includes perception, shooting-visibility, distance, intelligence, weapon choice, and sound calls | Agent detection and shooting rules |
| `move_objects` 36831–37837 | Calls object interaction, `animate_model`, traffic functions and map-area checks | Moving scenery and vehicles/traffic |
| `move_weapons` 58876–59886 | Calls impact/weapon effect handlers, wind/angle, animation and model removal | Projectile/combat presentation and physics interactions |
| `move_effects` 19979–21317 | Calls `animate_model`, `remove_model`, `init_fireball`, `init_effect`, collision, random, sound and movement helpers | Explosion/effect state is intertwined with simulation and audio |
| `check_end_level` 531–1186 | Pause/resume sound and music around game update; checks success/failure and interaction | End-of-mission/audio sequencing |

**Interpretation:** Explosion-heavy slowdown may involve actual effect simulation, projectile/collision work, animation, redraw and audio calls, not simply the SDL output surface. **No profiling has been run**, so there is no justified hotspot ranking. Preserve original explosion appearance and 16 FPS mission pacing rather than accelerating simulation to disguise delays.

## 3. Rendering is interleaved with simulation input

- `draw_mapwho` at 18029–18715 has direct calls to viewport clipping, actor sprite drawing, `set_block_changes`, and occlusion helper `__cover_sprite`.
- `copy_back` at 18910–19034 invokes screen/region copy functions before actor and panel draw.
- `draw_panel` at 39036–39215 includes tactical map, bars, text and mouse-pointer handling.
- `scroll` at 19459–19871 triggers tile and dirty/change redraw, and `scroll_map` is called twice each mission tick (251–266).
- `load_map_level` at 27845–27935 loads mission resources, calls audio bank setup, init blocks and init level, so visual data is lifecycle-bound to gameplay state.

**Lighting consequence:** Introducing glowing objects must preserve entity occlusion and the clear/copy/redraw phases. A bright static object cannot be assumed to persist on screen unless its pixels are redrawn at the right stage. Do not alter collision, perception, pathfinding or vehicle logic to draw a light.

## 4. Native audio dependency map

Based on directly read C source:

| File, lines | Evidence |
| --- | --- |
| `engine/bfsoundlib/src/drv_oal.c:102+` | Opens OpenAL device with `alcOpenDevice`; other sections manage source queues, playback and sample buffers |
| `engine/bfsoundlib/src/init_mus.c:84,229` | Refers to `music.dat` and opens legacy XMIDI driver |
| `engine/bfsoundlib/src/init_snd.c:181+` | Loads `sound.dat` sound-bank data |
| `engine/bfsoundlib/src/bfaudio.c:118+` | Initializes audio configuration and data paths |
| `engine/src/game.c:210–220` | Exit/reset tears down audio using different DOS/non-DOS paths |
| `.github/workflows/windows-source-v0.7.yml:92–104,149–158` | Checks i686 compiler and native OpenAL/WildMIDI import libraries; rejects unwanted MSYS audio DLL imports |

**Implication:** Rendering/audio co-exists with original effect calls and window/input event processing. Never silently substitute audio libraries, change sample trigger rates or alter update scheduling while polishing graphics.

## 5. Current verification coverage

| Requirement | Evidence today |
| --- | --- |
| Existing gameplay fidelity | Historically user-verified v0.7, NOT repeated in this audit |
| Static subsystem architecture | The direct call sites above are confirmed in actual source |
| Correct 16 FPS mission / 30 FPS three strategic loops | Confirmed source path, not frame-time measurement |
| Sound effects/music | C dependencies traced; no current audio playback verification |
| AI/pathfinding/vehicle/weapon compatibility | Source routines identified; **not systematically tested** |
| Explosion FPS/performance | **NOT measured** |
| Real game binary + retail mission data | **NOT executed** |
| Native Windows focus/scroll/corners | **NOT tested in this audit** |
| Automated gameplay/mission tests | No such tests demonstrated by current audit; existing bflibrary tests are graphics/maths-oriented |

## 6. Recommended next evidence-based test matrix (no feature work authorised)

When original retail game files and a runnable Windows environment become available:

- Record baseline mission frame-time distribution: quiet map, moving vehicles/pedestrians, small firefight, multi-explosion scene. Report wall-clock frame times and stage timings separately; never report guessed FPS.
- Test four edge scroll directions, all four corners, repeated top edge, keyboard movement, mouse drag, window focus/alt-tab and restoration.
- Test agents moving/firing, entering vehicles, hits/explosions, death, weapon selection and objectives.
- Test save/load with credits below and above the original 30,000 threshold, without modifying user save data; account for the original save-credit adjustment.
- Verify music and sound FX through mission lifecycle, campaign/strategic screens, window switching and application exit.
- Check 320×200 strategic/menu vs 640×400 logical mission display and 640×480 backing surface under fullscreen/windowed mode.
- Compare screenshot pixel regions and timings with the known-good v0.7 package, where legal original assets permit.

## 7. Status and handover

The **relevant named source paths are mapped**, but this is a static architectural audit, not semantic review of every unnamed assembly function or a full verified behaviour map. Full asset-free analysis could continue by decoding control-flow dispatch tables and lower-level graphics/ABI routines, but conclusive correctness requires an actual build, assets and runtime tests.

**Code modified: NONE.** No FreeSynd code copied. No CI workflow modified. `main` protected. Preserve versioned v0.9A/v0.9B plans and the exclusions of the v0.8 camera-speed experiment. Only audit documents are added during this work.
