# Syndicate 2026 — Main Systems: AIL Allocator/Idle Dispatch Closure and Integrated Verification Gates

**Date:** 2026-11-07 (Australia/Brisbane)  
**Workstream:** MAIN audit, branch `main`. Documentation-only; not a code change or a gameplay test.

## Purpose and predecessor reconciliation

This report closes three remaining *source-identifiable* questions from `2026-11-07-main-ail-timer-service-and-mixer-teardown.md`, and consolidates the verification prerequisites already established across the MAIN reports. It **does not supersede the 136 existing detailed audit reports** or claim that source inspection provides observed frame costs. Relevant previous audits: `2026-11-01-main-audio-timer-callback-workload.md`, `2026-11-01-main-openal-digital-voice-queue-lifecycle.md`, `2026-11-01-main-openal-pool-accounting-init-failures.md`, `2026-11-01-main-profiling-entrypoints-and-timer-selection.md`, `2026-11-01-main-explosion-spatial-collision-and-frame-pacing.md`, `2026-11-06-main-save-write-integrity-static-audit.md`, and `2026-11-07-main-display-swap-lock-failure-propagation.md`.

### Direct source evidence
- `engine/bfsoundlib/src/ailss.c` SHA `e8867a60299317e054800e029522d2e176f6290b`: `AIL2OAL_API_allocate_sample_handle` ~472–504 and `AIL2OAL_API_release_sample_handle` ~734–743.
- `engine/bflibrary/src/x86-win-sdl2/swindows.c`: `LbWindowsControl` ~171–205 and idle-handler registration. 
- `engine/bfsoundlib/src/bfaudio.c` SHA `56dd05654f6bb9b653fb81f0a1a3dab5088b6258`: `sound_update` and `sound_fake_timer_initialize`.
- `engine/bfsoundlib/src/ail.c` SHA `bc54f7ff1abe19ca90de441f861ecbf0647a7600`: startup registration.
- `engine/bfsoundlib/src/aila.c` SHA `ca2b9574d2f1b1fe2e2e2097b2bc9af6c6368e80`: `AIL_API_timer` catch-up.
- `engine/bfsoundlib/src/init_snd.c` SHA `9a21b14297fe6ee886cdd71ad11cfbb69d147f8a`: allocation setup and teardown.
- `engine/bfsoundlib/src/sndtimer.c` SHA `168bb3b598d6834384723d58dc036197d76d9de0`: Bullfrog timer helper implementations.

## MAIN-CLOSE-01 — Exact low-level sample allocator contract

`AIL2OAL_API_allocate_sample_handle(DIG_DRIVER *digdrv)` returns NULL when driver is NULL, or if no `digdrv->samples[i].status == SNDSMP_FREE` slot remains within `digdrv->n_samples`. The exhausted case sets error `Out of sample handles.`. On success it calls `AIL_init_sample(s)` and returns a pointer to the physical `SNDSAMPLE`. Source around the initialization marks sample `SNDSMP_DONE`. `AIL2OAL_API_release_sample_handle` tolerates NULL and marks an existing handle `SNDSMP_FREE`, then clears pipeline providers. This explicitly resolves allocator return semantics that were pending in the 7 November audio slot report.

**Consequence:** The game's `InitAllSamples` doesn't check each return before setting `SoundInstalled=1`; a partial failure would leave NULL entries in its configured game sample-handle range, potentially reached by later slot scans. Conditional risk only, not proof of actual failure under the default 10 game handles and 15 requested physical mixer slots. Note earlier OpenAL audit: partial `alGenSources` failure can truncate `digdrv->n_samples` even after requested pool creation, and the caller does not check its result. This is a plausible route to fewer available physical handles and requires isolated testing.

## MAIN-CLOSE-02 — SDL2 event pump is the audio timer's idle dispatch boundary

`AIL2OAL_API_startup` invokes `sound_fake_timer_initialize`, which registers `sound_update` via `LbRegisterIdleHandler`. `LbWindowsControl` in the SDL2 backend:
1. Calls `SDL_PollEvent` up to **256 times per invocation** (exits early when queue empty) and processes each event.
2. Visits `lbIdleHandlers` from index 0 through the first NULL or configured capacity and invokes each callback once in this traversal.
3. Returns quit state.

The registered `sound_update` calls `AIL_API_timer` only if `AILStartupAlreadyInitiated` is set. Native `game_handle_sdl_events` invokes `LbWindowsControl`; `game_update_full` calls the event handler once up front and may call it *additional times inside a conditional SDL_Delay loop*. Consequently **audio timer advancement can occur during the normal pacing/event phase, not only once per tactical frame**, with callback activity depending on elapsed time, lock status, active timers and whether initialization/registration succeeded. Although SDL has a 256-event cap per pump, this is not a 256-audio-callback limit: `AIL_API_timer` itself can execute catch-up callbacks.

Whether `LbWindowsControl` is also reached outside these functions, exact number of successful idle-handler registrations, actual cadence on target Windows build and resulting performance remain unmeasured. `LbBaseReset` clears idle handlers at overall library reset. Audio shutdown toggles `AILStartupAlreadyInitiated` on an examined full shutdown route, but audit all unusual restart paths before claiming universally safe registration lifecycle.

## MAIN-CLOSE-03 — Bullfrog timer helper suspicious but active reachability unproven

`SetupBullfrogSoundTimer` appears to break its free-slot search on **`stinf->used`**, not `!stinf->used`, and `ReleaseBullfrogSoundTimer` indexes `sound_timer[st-1]` with no local bounds guard. Searches of approximately thirty focused `engine/bfsoundlib/src/*.c` implementations and ten relevant `engine/src/*.c` files produced **no non-definition source references** to either function. These checks were limited to these accessible C sources; they do not establish absence in `syndre.sx`, generated wrappers, external data, all build variants or binary symbol references. Do not prioritise a behavioural change to this helper absent a verified reachable caller.

## Priority-ordered integrated verification plan

| Priority | Topic | Static evidence | Required next gate |
| --- | --- | --- | --- |
| P0 | Protected runnable baseline and build chain | Existing v0.7 and Windows build reports; no recent executable test | Reacquire and retain exact known runnable Windows artifact and source commit; verify build process, dependencies and campaigns before any proposed implementation. Do not mislabel source readbacks as runnable testing |
| P0 | Save integrity and campaign isolation | `save_player` unchecked writes, mutable credits before open, native `save/%02d.gam`, Windows `O_CREAT` without truncate, path-transform split | Disposable save copies; test failed open, partial write, load validation, legacy format and distinct original/American Revolt namespaces before any save fix |
| P0 | Explosion slowdown and 16 FPS preservation | Fireball 128-slot repeated scans, 256-slot effects dispatch, same-pass new fireball handling, MapWho traversal, collision and audio timer catch-up are plausible separate work stages | Approved *diagnostic-only* 32-bit Windows build to measure simulation stages, collision counts, scene draw, bitmap conversion, SDL present, event pumping, `AIL_API_timer`, `SS_serve`, and intentional sleep; compare with uninstrumented control at identical scenarios |
| P1 | Resource-loading and original campaigns | Prior RNC, map, sound-bank and resource consumer failure audits | Only synthetic/disposable corrupt/short reads, correct cleanup, no changes to proprietary or user data, and cross-workstream review for loader interfaces |
| P1 | Display reliability and graphics integration | Ignored SDL surface restore/check errors; custom scaling ignores certain lock errors; screen relock busy loop | Fault-injection on disposable Windows build; coordinate any bitmap/palette/presentation change with Graphics audit |
| P1 | Sound/mixer state and timer | Native 10 requested game handles; low-level NULL allocation result; possible timer catch-up; incomplete muted cleanup | Live allocator failure/restart tests, mute/unmute and sound bank transitions; measure timer callbacks before attributing explosion slowdown to audio |
| P1 | Mission Builder / resource ABI | Existing separate Mission Builder branch and RNC/mission load contracts | Preserve independent branch; consult completed Mission Builder handover before touching campaign map/object/resource formats |
| P2 | UI input/camera/menu behaviour | Existing 16/30 FPS split and mouse/window event audit; improvements requested but not presently authorised | Validate protected camera/edge scrolling, menu pacing and original input behaviour only after audits and controlled test build are ready |

**Dependency order:** finish static gaps and handover → acquire reproducible Windows baseline (no gameplay playtesting until cross-workstream audit completion, per user instruction) → obtain explicit approval for disposable instrumentation changes → compare data against original baseline → propose specific fixes with impact analysis → approve and implement in isolated branches → build, automated checks and *actual* Windows/gameplay regression with user download.

## Completion boundary

**Verified now:** low-level allocator NULL semantics, RELEASE-to-FREE behaviour, actual SDL idle callback dispatch, 256-event cap, game/audio pacing call interaction, constrained C callsite search and existing audit reconciliation.

**Not verified now:** total exhaustive assembly timer-helper callsites, compiled Windows features, end-to-end runtime throughput, CPU or memory benchmark, actual sound-channel exhaustion, user installation, runnable executable, save behaviour, graphics or game correctness.

**Files modified by this audit:** only this new report and the shared development handover. No code, assets, gameplay, saves, CI or other audit branch changes. Preserve v0.7, original 16 FPS simulation, 30 FPS menu behaviour, existing camera/mouse/keyboard, full original/American Revolt support, original audio and effects. Maintain separation of Graphics and Mission Builder streams.
