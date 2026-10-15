# SYNDICATE 2026 — COMPLETE BUILD CONTINUITY / RECOVERY
Date: 16 October 2026 (Australia)
Repository: https://github.com/mh83invest-creator/Syndicate-2026
Primary source ownership: user's GitHub repository; based on GPL-licensed SyndicatFX 0.0.6.1022. No original commercial game data is stored in GitHub.

## Critical status
LAST USER-CONFIRMED GOOD GAMEPLAY BUILD: v0.7.
CURRENT MOST ADVANCED SOURCE: branch fix/v0.8.1-regression-isolation.
v0.8.1 Windows CI run #38542642745: SUCCESSFUL. HOWEVER, v0.8.1 IS NOT USER-PLAYTESTED. Do not call it stable.
v0.8 experimental branch caused new failures and is NOT approved to merge.
Do not overwrite main with v0.8 or v0.8.1 without successful gameplay confirmation.

## Immutable retrieval points / branches
* Main (known-good v0.7 source): https://github.com/mh83invest-creator/Syndicate-2026/tree/main
  Snapshot  a22b0b9580a9710b4e693e88854351835ef1ee40
* Current v0.8.1 candidate: https://github.com/mh83invest-creator/Syndicate-2026/tree/fix/v0.8.1-regression-isolation
  Snapshot e783f037811603ad2b76308c14dc1a2bc89abfb2
* Experimental v0.8 (FAILED WINDOWS PLAYTEST): https://github.com/mh83invest-creator/Syndicate-2026/tree/feature/v0.8-scroll-speed
  Snapshot 87a3b92a9df1c8bc24b6beb0bacb0b38f483c088
* Earlier successful source experiments: diagnostics/camera-v0.4.

## Gameplay-tested chronology
v0.4: 32-bit Windows native source build with OpenAL32/libWildMidi and SDL2. Fixed mouse edge scrolling all 4 directions/corners; user confirmed. Music/sound worked, cinematic intro still absent.
v0.5: company configuration menu 30 FPS, user confirmed; missions remain 16 FPS.
v0.6: company configuration and world-map menu 30 FPS, user confirmed.
v0.7: company configuration, world-map and briefing tactical-map menus 30 FPS. User confirmed playable. This is our rollback baseline.
v0.8: five-level camera speed experiment with [ and ] keyboard control and SDL persistent preference. Windows CI compiled, but user reported (a) speed adjustments unnoticeable, (b) top-edge scroll veering left, (c) game unexpectedly switching focus to Windows desktop while running. User confirmed (b) and (c) did NOT occur in v0.7. The experimental branch is a FAILED PLAYTEST.
v0.8.1: diagnosis/isolation branch which restores exactly four known-v0.7 source blobs over v0.8: engine/src/syndre.sx, engine/src/game.c, engine/src/game.h, engine/conf/wrappers_game.conf. That deliberately removes the ineffective speed experiment. GitHub run #38542642745 compiled, but gameplay has not been checked. This is an A/B control, not a finished feature.

## Exact v0.8.1 restoration commit
Commit 9ab75f19819d790527764a0162d04342ddd756ce restored four source paths.
v0.8.1 workflow commit e783f037811603ad2b76308c14dc1a2bc89abfb2 is branch head as checked 16 Oct 2026.
Main v0.7 successful restored-source Windows workflow #38473241548; separate native successful run #38483184953.

## Compiling without user development-tool setup
1. GitHub repo -> branch fix/v0.8.1-regression-isolation -> .github/workflows/v0.8.1-regression-build.yml.
2. GitHub -> Actions -> select "Syndicate 2026 v0.8.1 Regression Isolation" -> Run workflow -> select the exact feature branch.
3. Wait for Windows i686 native build success. Inspect logs if red; do not claim build works while red.
4. Download GitHub workflow Artifacts from successful run #38542642745:
   - syndicate-2026-v0.8.1-regression-isolation (native Windows executable)
   - syndicate-2026-i686-dependencies (native 32-bit OpenAL/WildMidi dependencies)
   - syndicate-2026-v0.8.1-handover (handover copy).
   Note: GitHub Actions artifacts may expire; rerun the workflow to recreate them.
5. To run on Windows, create a NEW clean test directory with the output executable, matching 32-bit DLLs including SDL2.dll/OpenAL32.dll/libWildMidi.dll, and LEGALLY OWNED retail game resource folders. The retail data must NEVER go to the public repository.
6. Launch from the full extracted folder using the existing double-click START_SYNDICATE test launcher if supplied in a known-good packaged ZIP; ensure its target is syndicatfx.exe and resources are in their expected paths.
7. Compare against v0.7 on top-edge map scrolling, corner movement, music/sound, menus, agents/mission pacing, and spontaneous loss of window focus.
8. Do not merge v0.8.1 until user confirms these checks.

## Building on a fresh Windows computer
No local compiler is required for the GitHub Actions build; GitHub handles compilation. To archive source locally: switch to the desired branch on GitHub -> green Code menu -> Download ZIP -> extract to an external drive. Optionally install Git to clone and preserve commit history. Keep separate directories for v0.7 stable, v0.8 failed experiment, and v0.8.1 regression-isolation. Retain known-good packaged full game ZIP locally because GitHub does not host retail data.

## Rules for future work
* Record every iteration in docs/releases/vX.Y.md on GitHub: source branch/commit, code delta, automated CI links, user's Windows results, known issues, dependencies, continuity and explicit rollback.
* CI must check the version's handover exists, and upload it as a build artifact. Main remains stable until user game-tests.
* Camera edge fix in v0.7 is sacred until a precise regression-free alternative passes.
* Native Win32 / MinGW i686 build. Keep OpenAL + WildMidi native; reject MSYS-linked audio imports.
* Never increase the mission simulation 16 FPS update rate merely to smooth menus; the three strategic menus run 30 FPS.
* No proprietary Syndicate retail assets in public GitHub.
* User wants self-contained Windows test ZIP with double-click launcher and no requirement to install development tools.

## Future priorities
1. Gameplay-test v0.8.1 to confirm the regressions disappear.
2. Rework camera-speed options as a truly testable visible UI slider, with original v0.7 movement as default; use a safe camera-only method, preserve ABI/calling conventions and SDL focus.
3. Test at all directions/corners and boundaries and verify smoothness, persistence, sound and actual mission pacing.
4. After confirmed scroll controls, improve graphics. Cinematic restoration and explosion/combat slowdown were previously deprioritised.
5. Provide packaged builds plus changelogs and GitHub handovers each time.

## Related handovers
docs/releases/v0.7.md
docs/releases/v0.8.md
docs/releases/v0.8.1.md
docs/HANDOVER_POLICY.md

## A fresh ChatGPT conversation can begin with:
"Continue development of Syndicate 2026. Repository https://github.com/mh83invest-creator/Syndicate-2026. Read docs/BUILD_CONTINUITY_2026-10-16.md and docs/releases/v0.7.md, v0.8.md, v0.8.1.md. Last user-playtested stable is v0.7 on main. Current candidate is fix/v0.8.1-regression-isolation (CI #38542642745 success but gameplay not tested); v0.8 speed prototype failed. Preserve main, maintain handovers for every build, no commercial assets in GitHub, and package self-contained Windows tests with double-click launcher. First verify the regressions and only then redesign camera-speed slider." 
