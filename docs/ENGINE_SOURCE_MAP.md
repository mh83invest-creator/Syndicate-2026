# SyndicatFX source mapping — initial investigation

Reference: `swfans/syndicatfx` tag `0.0.6.1022`. This is **unmodified upstream**, not the user's modified v0.6 source.

## Findings verified in upstream files

- `src/main.c`: `main()` launches `run_intro(argc, argv)`, which constructs `intro >nul:` and calls `system()`; it then enters the game through `syndicate()`.
- `src/main.c`: `syndicate()` is an externally declared assembly routine, not a simple C game-loop function.
- `src/syndre.sx`: imported assembly source is present. Mission camera logic may be here or in other included source; precise entry point must be traced before editing.
- `bflibrary/src/x86-win-sdl/smouse.cpp` and `bflibrary/src/x86-win-sdl2/smouse.cpp`: platform mouse adapters are present; mouse coordinates/events must be followed into game logic to determine scrolling responsibility.
- `src/mouse.c` is small and contains no literal scroll/camera token. A token search does not prove the behavior is absent.

## Change control
1. Keep the v0.6 playable executable as the known-good regression reference.
2. Build clean upstream from the pinned tag first.
3. Version every intended source modification as a reviewed `patches/*.patch` file, or commit a full legally distributable source fork.
4. Trace camera position and mouse edge handling before adding a scrolling patch.
5. Keep the intro problem separate from camera patches and the menu FPS experiments.
6. Do not mark any change tested until a Windows build runs against local game data.

## Source bootstrap
Run `bash scripts/prepare-source.sh` in a Git-enabled shell to materialize upstream into ignored `.work/upstream`. The script refuses to overwrite an existing working copy. A separate GitHub Actions workflow can later consume this same patch sequence.

## Open blocker
The modified C/assembly source used for v0.6 is not present in the known archives or current repository. This document is investigation and source preparation, not a recovered implementation.
