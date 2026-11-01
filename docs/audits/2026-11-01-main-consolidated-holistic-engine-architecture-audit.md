# Syndicate 2026 — consolidated holistic engine architecture audit
Date: 2026-11-01 (Australia/Brisbane). Scope: static source inspection, systems map, cross-subsystem risks, evidence gaps and modification preconditions. **No engine changes.**

## Provenance
Directly read on GitHub default branch: `engine/src/main.c` (blob `e9621c6594173569e78366553ddc2de820e407f8`), `game.c` (`9f56221d527075fba2b1f2406fab60af28a4289a`), `display.c` (`9c6ba018f31d67b394c5973d05d6e51824171b68`), `game_data.c` (`23a7159fc016e58872d30b61b46ba62b6efabac7`), `keyboard.c` (`3cf712ec1561a48903beae8a60d3d9d1715636ab`), `mouse.c` (`e689bd62025fd9bbdfe7645eb9d7ea05b5603792`), `sound.c` (`d2f5946d11e11cb123846ee5e54ee4065bf1f07d`), `research.c` (`a4da9c59714755d4bb82bebdc43a971ae437c995`), `player.c` (`00c94010467884bdba7726d399a2ff1ce7ee30cf`), `weapon.c` (`97a6863fdb0fd13255966b1974907d6db6501f6e`), `timer.c` (`73b7edfbf741c48cdf6c78d1e2b6d8239ed4012c`), `osunix.c` (`3c4f23831b9f881eb916de0e3b25c6f253dff3d6`), `oswindws.c` (`abc0b2c3b26b4f3a10e6e8896aa7695c79ba497b`), `engine/src/Makefile.am` (`dfaad56c157bd9042bec49e681b619794b0fcba3`), and full `engine/src/syndre.sx` blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`. Reused earlier source-backed Main Systems reports for OpenAL, collision, graphics and ABI details; those are *prior audit evidence*, not newly executed tests.

## 1. Runtime ownership: full lifecycle
**C startup:** `main.c:266–365` establishes defaults, parses options, calls `game_initialise`, creates display buffers, applies display settings, loads shared sprites/resources, GUI text, mouse/keyboard, `init_audio`, buffered keyboard callback, then invokes assembly `syndicate`. Following return it frees resources, display buffer, logging and memory, then calls `game_quit`. Conditional code means not every setup/free path is executed equally.

**Assembly owns gameplay structure:** `syndre.sx:199–303` uses `select` (label ~54149), `reset_mission_info` (~25849), `initialise_player` (~28217), tactical loop, `free_map_level` (~27640), optional `transfer_people_into_player` (~26924), and `level_finished` (~54998). This is not a typical C-owned game loop. Exit paths can skip transfer/progression and return to selection; one must not unify them without studying escape/failure semantics.

**Tactical tick:** `syndre.sx:215–279` performs key/turn checks, single-player `single_play` (~28088) / `process_computer_players` (~7493) or `multi_play` (~28106), then `move_it`, button/danger/day/end handling, screen wait/copy/palette/map scrolling/map+panel draw/click-map, then non-DOS `ac_game_update` to pump events and pace. The two observed `scroll_map` calls must not be coalesced by intuition.

## 2. Simulation, AI and effects
`move_it:33330–33337`: `call move_worlds` (~33376), `move_people` (~49268), `move_weapons` (~58876), `move_effects` (~19979), `move_objects` (~36831), and **tail `jmp move_vehicles`**. The order is meaningful until proven otherwise. `process_players_turn` (~38397) increments `level__Timer`. `process_computer_players` processes player data/timer; `move_people` calls `person_intel`; `single_play` calls `process_action`; multiplayer follows a separate path. These provide evidence of AI, action-processing and simulation ownership, not a complete behavioural specification.

`move_effects` scans records with 0x1e-byte stride, dispatches via indirect effect-type jump table and restores saved registers/0x1c stack at `jump_1fb44:21311–21317`. Prior audit traced 128 candidate fireball allocation slots, total 256 effect slots, and collision MapWho traversal; these are source structures, not measured per-frame runtime demand. `sprite_colide` saves registers/0x70 local stack and returns at `jump_1378a:4664–4671`. Existing collision and effects order cannot be altered casually because effects can interact with object/vehicle simulation and spatial lists.

## 3. Rendering, camera and timing
`display.c:72–95` maps legacy video modes and sets graphics screen/mouse window. `swap_wscreen:244–261` locks screen as needed, blits Vres16 or MCGA buffer, unlocks then `LbScreenSwap`, relocking if previously locked. `syndre.sx` invokes other assembly display/camera functions before C pacing; `scroll_map` defined at ~19352. `mouse.c:31–49` derives legacy 640×400 scaled coordinates from display mouse state. Prior audit also established SDL software surface presentation and Vres16 conversion as possible performance contributors, **not established bottlenecks**.

`game.c:141–196` frame pacing uses `SDL_GetTicks`, `SDL_Delay`, display unlock/relock and SDL event processing. `game_update:204–208` caps tactical calls at 16 FPS; `game_update_menu:223–227` targets 30 FPS. Both share a `static last_frame_ticks` in `game_update_full`. Never equate timer delay with CPU render/simulation latency; preserve original tactical speed.

## 4. Input and UI
`game_handle_sdl_events:94–103` invokes `LbWindowsControl`, can quit the game and updates scaled mouse coordinates. `keyboard.c:31–109` maintains a 16-slot ring buffer of keydowns; a full ring does not advance write index, so a key is not enqueued; `read_buffered_char` performs shift-aware lookup and excludes values >=128. `syndre.sx` reads key state and handles `click_map` (~2516), `scroll_map` (~19352), `correct_buttons`, tactical panel. Input is therefore shared between SDL/bflibrary C callbacks and assembly UI/mission logic. Changing update cadence or polling can change interaction semantics even if game speed appears unchanged.

## 5. Audio
`main.c` initializes audio once on startup after input setup. `sound.c` exposes setup/init, sample playback, MIDI sequencing, pause/resume and shutdown functions. `game.c:210–219` calls `FreeAudio` and resets input/screen on non-DOS shutdown. Tactical assembly calls audio/music routines at mission changes. Prior Main Systems source audits examined OpenAL buffer pools, queue ownership, timer dispatch through SDL idle callbacks and partial initialization/error paths; they identified *conditional resource-accounting hazards*, not confirmed normal-play slowdown. Audio depends on event-loop cadence and sound callbacks; changing effects or pacing can affect audio service frequency.

## 6. Campaign saves, progression and economy
`game_data.c:121–139` loads save-slot description from `save/%02d.gam` and supplies an empty-slot string if missing. `load_player:141–165` uses fixed binary writes/reads for `players`, `country_states`, `weapons`, `cybmods`, `research`, `selected_team`, with embedded compile-time/assert layout expectations; it returns if open fails but does not check each `LbFileRead` length. `save_player:167–192` **mutates credits** by deducting ten percent when current credits exceed 30000, before opening the output file; failed open can therefore leave memory credits changed. It writes fixed-order binary fields, also without visible per-write count checking. This is a source-backed behavioural risk, not a reproduced defect or approved fix. Investigate file path mapping in `game.c` and `game_data.c` jointly: slot isolation and per-campaign user directory semantics must be preserved.

Campaign-owned data includes player, country status, equipment, cyber-mods, research and team selections, and mission completion transfers people/progression in assembly. `research.c`, `player.c`, `weapon.c` are small C shells; substantial game state logic lives in assembly, so C file size does not measure subsystem complexity. `process_day` participates in timing/campaign progression. Exact save triggers and campaign failure semantics require a more detailed cross-reference before any rewriting.

## 7. Build/platform architecture
`Makefile.am` combines C source and `syndre.sx` original assembly, generated Watcom-to-native wrappers, and Bullfrog/sound libraries. Earlier audits verified Windows 32-bit i686 MinGW, symbol map, OpenAL/WildMIDI dependencies and separate read-only Windows build / write-enabled restoration workflows. Baseline version and separate Graphics and Mission Builder branches must stay isolated. The assembly function and struct layout expectations make an unplanned x64 conversion high-risk.

## Dependency map and preservation constraints
```text
OS/SDL input, timers, audio device, display surface
   ↕ bflibrary / bfsoundlib
main.c startup → shared resources & pointers → assembly syndicate()
                                           ↙        ↓          ↘
                                   menu/select   mission        campaign saves
                                       ↓          init                ↑
                                  research/equip  ↓                   |
                                           tactical loop (input/AI/actions)
                                                      ↓
                                worlds → people → weapons → effects
                                            → objects → vehicles
                                                      ↓
                               mapwho/collision/objective/time checks
                                                      ↓
                                 palette/map/camera/panel/present
                                                      ↓
                            SDL events, audio idle handling, frame pacing
                                                      ↓
                        mission cleanup / agent transfer / level_finished
                                                      ↓
                                    campaign progress & selection
```
This is a simplified relationship diagram, not proof of all dynamic branches.

## Risk register (evidence vs confirmation)
| Risk | Source basis | Status |
|---|---|---|
| Save mutates credits before verifying file creation | `game_data.c:173–180` | **Confirmed order in code**; failed-write gameplay impact untested |
| Fixed-size binary save format | `game_data.c:146–163,182–189` | Confirmed schema coupling |
| Input events serviced during pacing | `game.c:149–195` | Confirmed; gameplay impact unmeasured |
| Shared menu/tactical frame clock | `game.c:146,205–227` | Confirmed; transition behaviour unmeasured |
| Effect pool scanning/collision load | Assembly and earlier dedicated audit | Structural cost confirmed; hotspot not profiled |
| Rendering/copy cost | `display.c:244–261` | Path confirmed; cost unmeasured |
| OpenAL pool/queue conditional hazards | Earlier dedicated source audits | Candidate; not reproduced |
| Assembly ABI/struct coupling | `Makefile.am`, wrappers and save assertions | Confirmed structural constraint |

## Audit completeness and remaining work (not claim of exhaustive understanding)
This consolidates **high-level ownership and cross-system interactions**. It does NOT establish a complete instruction-level behavioural description of the ~85k-line assembly, every state transition, all multiplayer network contracts, exact mission selection/branch logic, every save caller, objective evaluation, UI screen state, asset content/schema or Windows runtime operation. Those need targeted deep audits and tests. No gameplay or performance measurements were made.

### Recommended next source-deep-dive order, still no modifications
1. Mission selection/init/exit/progression and persistent vs temporary memory ownership, including fail/abort/retry.
2. Campaign, research, economy, save-call graph and binary layout.
3. Input/menu/camera/AI/action commands, network differences.
4. Asset loading, sprite/palette/display memory, sound-bank/audio service/cleanup.
5. Spatial MapWho/collision simulation, weapons/explosions/physics and all scene update stages.
6. Interdependency regression matrix, then user-approved isolated build/test. Avoid altering protected v0.7 until whole-engine map and acceptance criteria are mature.

## Verification
Static inspection of listed source files and previous source-backed audits; no branch creation, engine/CI modifications, Windows build, gameplay tests, runtime profiling or performance fix. Parallel Graphics/Mission Builder unchanged.
