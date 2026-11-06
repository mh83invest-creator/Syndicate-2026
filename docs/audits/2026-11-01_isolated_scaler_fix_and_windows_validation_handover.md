# Syndicate 2026 — Isolated SDL2 scaler repair and Windows validation handover

Date: 2026-11-01 (Australia/Brisbane)
Branch: `fix/graphics-sdl2-scaler-isolated-2026-11-01` (created from `audit/graphics-assets-2026-11-01`).
**Status: patch implemented on isolated branch; local model tests pass; Windows CI runs initiated and have not yet reached a verified final result as of this report. Not merged.**

## Requested scope and baseline
User authorised completing the earlier graphics audit follow-through: implement isolated diagnostics, verify actual SDL surface characteristics, fix validated memory risks and regression-test, without changing protected v0.7 or other development branches.

## Implemented changes
1. `engine/bflibrary/src/x86-win-sdl2/sscreen.c` commit `1779a89882f10641e9d6b30af4c945442b9e8197`: indexed-to-indexed and indexed-to-truecolour helpers now take source/destination `pitch`, calculate per-row byte offsets and accommodate padded rows. The true-colour helper replaces the unconditional `long*` 4-byte store with a 32-bit pixel value and `memcpy` of exactly `dst->format->BytesPerPixel` bytes, avoiding misaligned/overwide writes for 16/24-bit destinations. Custom scaler returns failure after a failed SDL source/destination lock, unlocking a previously locked source on destination failure.
2. Additional `sscreen.c` commit `432155f5f69fb411b846000be15c5f8b2e781f19`: under `SYNDICATE_GRAPHICS_DIAGNOSTICS`, logs actual draw/window `w,h,BitsPerPixel,BytesPerPixel,pitch` and secondary-surface flag at mode setup. Compiled conditionally, to avoid logging in normal builds.
3. `engine/bflibrary/tests/graphics_window_surface_diagnostics.c` commit `56911eb6c8e909ced420c12c1217923f3fb9637c`: isolated real SDL2 dummy-video window-surface and 320x200 8-bit drawing-surface probe for CI, without retail game data.
4. `engine/bflibrary/tests/graphics_scaler_layout_contract.c` commit `7e0b5ca09bcf7fc97e0d9219bf5eca1685569623`: 12-case standalone row-pitch, pixel-width and padding-canary regression matrix, **not** the original engine routine.
5. `.github/workflows/graphics-sdl2-validation.yml`, latest commit `26feaf3b07fb154ecdb3212f397a4cd2d3c63a1b`: a separate Windows i686 MSYS2 build derived from verified v0.7 workflow, triggered on isolated branch. Compiles and executes the layout contract and real SDL2 dummy-window probe, configures original engine with graphics diagnostics macro, builds Windows executable, checks SDL2.dll import and packages artifact. No modifications to protected original workflow.

## Tests executed
- LOCAL PASS: isolated C layout-check compiled on Linux GCC with `-fsanitize=address,undefined`: 12 cases spanning 1..4 destination bytes/pixel and widths 3/5/7, padding/canaries intact. Test harness initially had indexing expectation bugs; they were corrected and re-run successfully before reporting PASS.
- LOCAL PASS: Linux real SDL2 dummy-window `SDL_GetWindowSurface` using installed runtime and Python ctypes: requested 640x400 -> actual 640x400, 32 BitsPerPixel, 4 BytesPerPixel, pitch 2560. This is **not** the Windows executable.
- CI IN PROGRESS at report creation: isolated Windows workflow run `41293950377`, commit `26feaf3b07fb154ecdb3212f397a4cd2d3c63a1b`. Other earlier intermediate branch commits also launched runs. Do **not** mark the latest CI as passing before reading its completed job and logs. Repository now has multiple build runs because branch-push workflow triggers on source/CI edits.
- NOT DONE: Windows original game launch with retail assets, real gameplay screenshots/performance, actual game's SDL surface format/log output (diagnostics built but not run inside game), cross-resolution graphics regression, explicit fault injection for palette/SDL errors.

## Safety and remaining work
- Protected v0.7, main and other feature/audit branches untouched; changes only on this isolated fix branch.
- The fixed-width/pitch memory issue is addressed in code but remains **unverified on real Windows gameplay** until CI passes, runtime path is exercised and visual comparisons are checked.
- This pass did not broadly rewrite palette state, palette-pointer ownership, game frame timing, menu or gameplay code; those had source-level audit concerns without proven user-visible failure and require targeted validation before riskier modifications.
- CI success alone proves Windows compilation/tests, not interactive gameplay. On completion, inspect native SDL dummy-window BPP/pitch and actual binary imports in logs. User must test gameplay with legally sourced local game data if no data are available to CI.
- No merge or release until native CI and Windows gameplay checks complete; no protected version number change.

## Verification update — after first Windows diagnostic passes
- Windows run **41294240776** (SHA `b6e4d942dee8f2474f2788f51af2feea67de5cee`) completed **success**. Its logs show standalone contract `PASS: 12 isolated pitch/width/padding cases`, dummy SDL2 **WINDOW width640 height400 bpp32 bytes4 pitch2560 SDL_PIXELFORMAT_RGB888**, and **DRAW width320 height200 bpp8 bytes1 pitch320 SDL_PIXELFORMAT_INDEX8**; compiled `syndicatfx.exe` imports SDL2.dll and artifact was uploaded. **This intermediate source revision contained a return-code bug** subsequently identified: it treated `LbIScreenDrawSurfaceCheck` returning `Lb_OK` as failure. It should not be used for gameplay.
- Corrected in source commit `690b10e2b34c0e72ba661e102d2b6d72f9176785`: all three swap paths now consider only `Lb_FAIL` from the drawing-surface check fatal. They also propagate failed screen-surface restoration; `SwapClear` checks `SDL_FillRect` return.
- Added conditional one-shot diagnostics of blit route 0-3, source and window formats/pitches in commit `d21a4e24960159af43061991b0e76cf5782895f8`.
- Earlier Windows diagnostic runs **41293950377** and **41293819359** failed due to isolated probe's `main(void)` conflicting with the SDL2 Windows `SDL_main(int,char**)` signature; fixed in commit `d7bfce619f07b3ca4b9cfd3bf42500c3b55b29c3`. Subsequent Windows diagnostic and full build run **41294240776** passed both probes. Do not misattribute those earlier failures to scaler source.
- Experimental workflow artifacts were subsequently renamed to `syndicate-2026-graphics-isolated-executable`, `syndicate-2026-graphics-test-handover`, and `syndicate-2026-graphics-test-dependencies` in commit `1a17fe34ed4444bf11d860484874afe6eb74d35c`; never describe experimental binaries as protected v0.7 releases.
- The **latest** expected Windows CI validation at workflow SHA `1a17fe34ed4444bf11d860484874afe6eb74d35c` is run **41294440760**. It remained in progress at this addendum; verify its eventual completed result separately. Source patch log instrumentation and corrected swap return handling have not yet been accepted on runtime evidence.
- Local Linux standalone performance model (320x200 to 640x400, 32-bit packed) reported ~0.192 ms/frame old-style vs ~0.199 ms/frame pitch-aware (~1.04x), **not actual game or exact engine-scaler benchmark**.
- Windows real-game / retail-data screenshots / explosions / camera performance / saves still untested. See `docs/GRAPHICS_WINDOWS_RUNTIME_VALIDATION.md` for release acceptance matrix. **No merge.**

## FINAL Windows CI result — confirmed

**PASS**: Final experimental workflow run [41294440760](https://github.com/mh83invest-creator/Syndicate-2026/actions/runs/41294440760), on source SHA `1a17fe34ed4444bf11d860484874afe6eb74d35c` completed successfully. Job `124812222289` has no failed steps.

Exact Windows job-log evidence:
- `PASS: 12 isolated pitch/width/padding cases`.
- `WINDOW surface width=640 height=400 bpp=32 bytes=4 pitch=2560 format=SDL_PIXELFORMAT_RGB888`.
- `DRAW surface width=320 height=200 bpp=8 bytes=1 pitch=320 format=SDL_PIXELFORMAT_INDEX8`.
- Build/verification imports include `DLL Name: SDL2.dll`.
- Experimental artifact `syndicate-2026-graphics-isolated-executable` uploaded, artifact ID `13078279341` (15,941,904 bytes reported by workflow log). GitHub Actions artifact listing also confirms this name and ID; `expired=false` at verification. Test dependencies artifact ID `13078806913`; handover artifact ID `13078788433`.

**Scope of PASS**: Native Windows i686 successful compile/package, standalone layout tests and real SDL2 headless/dummy window format probe. Diagnostics for the actual game's scaler route and window surface are compiled into the engine, but the game was **not launched with real data**; the dummy-window test is a separate executable. Native visual accuracy, frame timing, explosions, palette behaviour, alt-tab, camera scrolling, game saves and comparison with protected v0.7 **remain to be tested**.

No merge/release or production baseline modification performed. Runtime acceptance instructions at `docs/GRAPHICS_WINDOWS_RUNTIME_VALIDATION.md`. Work may proceed with local Windows gameplay smoke tests using the experimental artifact.

## Actual engine scaler helper regression gate — 2026-11-01 continuation
- Added `engine/bflibrary/tests/generate_graphics_actual_scaler_test.py` (commit `f41e210a4fd7c98bd74891c66b96c6cae83f650a`): extracts the **exact current** indexed->indexed and indexed->truecolour helper definitions from `engine/bflibrary/src/x86-win-sdl2/sscreen.c` into an isolated test compilation unit, with minimal SDL colour/byte types. Tests canary and row-padding preservation and actual pixel writes for 1–4 destination BPP at widths 3,5,7.
- CI workflow now compiles and executes that extracted-source test with native Windows i686 GCC (workflow commit `d539ac884bdf69ee75e9f5a0780aa564c7bc30de`). This is **stronger than the existing standalone layout contract**, which tests a simplified reference model rather than actual helper bodies. Still not full game or wrapper SDL lock/error path.
- New workflow run `41295260908` started; **result not confirmed at documentation time**. The prior successful 41294440760 remains the last verified full package PASS until the new run completes. Do not mark new code/test as green without full CI status/job-log evidence.
- Native game with retail assets remains untested; preserve separate acceptance criteria `docs/GRAPHICS_WINDOWS_RUNTIME_VALIDATION.md` and do not merge or modify protected baseline.

## Verified actual-engine-helper Windows regression result — 2026-11-01
- **PASS** Windows Actions run [41295260908](https://github.com/mh83invest-creator/Syndicate-2026/actions/runs/41295260908), source SHA `d539ac884bdf69ee75e9f5a0780aa564c7bc30de`; job `124814665153` completed success with zero failed steps. Readback verified actual job logs.
- Log: `PASS: 12 isolated pitch/width/padding cases` and, importantly, `PASS: 12 actual engine scaler pitch, pixel and canary cases`. The latter compiles the verbatim indexed scaler helper bodies extracted from `sscreen.c` into a standalone Windows i686 test; it does not exercise the whole SDL wrapper or in-game frame selection.
- Log: `WINDOW surface width=640 height=400 bpp=32 bytes=4 pitch=2560 format=SDL_PIXELFORMAT_RGB888` and `DRAW surface width=320 height=200 bpp=8 bytes=1 pitch=320 format=SDL_PIXELFORMAT_INDEX8`, from standalone SDL2 dummy window test.
- Final compiled game `objdump` logs `DLL Name: SDL2.dll`. Experimental packaged executable artifact **`syndicate-2026-graphics-isolated-executable`**, artifact ID **13079206183**, successfully uploaded (logged size 15,941,904 bytes).
- Earlier 'CI in progress' notes in this report are historical status snapshots; this later section supersedes them. Automated scaler regression, Win32 compilation and artifact upload pass. **Real Windows gameplay, palette/visual identity, explosions, scrolling, FPS and save tests remain NOT RUN. No merge/release.**

## Byte-for-byte visual parity regression gate — 2026-11-06
- Verified previous actual-helper Windows CI run **41295260908** completed success and direct job log shows `PASS: 12 actual engine scaler pitch, pixel and canary cases`, `SDL2.dll` import and successful experimental package artifact ID `13079206183` (listed unexpired on 2026-11-06).
- Added `engine/bflibrary/tests/graphics_scaler_legacy_reference.inc` (commit `5bb9feb8cfe6714d3eefd8303377a81e24a9df38`), containing the two unmodified original scaler helper function bodies from the graphics audit branch, used only as test fixture.
- Added `engine/bflibrary/tests/generate_graphics_scaler_visual_parity.py` (commit `7e58370a05a181df9cf856260e08c2d05eb906a9`), which extracts the **current** repaired helper bodies verbatim and emits an independent C parity test against original helpers. Test compares full output byte arrays for indexed 8bpp and 32bpp true-colour packed surfaces for 320x200 source scaled to 640x400, 400x250, and 800x500. This protects against unintended changed original pixel output in the normal packed format; it does not address all visual differences or native game display.
- CI wiring commit `f9cf8fdee909fa45df59366b5946a2e8590b6175` invokes parity test in isolated i686 Windows job. GitHub Actions run **42227463972** triggered; result was **in progress** at handover creation. Do not claim parity PASS until readback of this run/job log confirms result.
- Scope remains no game assets, no player-driven Windows gameplay, no release or merge. Protected v0.7 and main untouched. This is test-only addition to isolated fix branch.

## Original-versus-repaired visual byte parity verification — 2026-11-06
- GitHub Actions Windows i686 run [42227463972](https://github.com/mh83invest-creator/Syndicate-2026/actions/runs/42227463972), source SHA `f9cf8fdee909fa45df59366b5946a2e8590b6175`: job `127924475329` reports **success** for `Verify legacy visual byte parity for normal scaler formats`, as well as `Validate scaler memory-layout contract`, `Exercise actual engine SDL2 scaler helpers`, and `Probe native SDL2 window and draw surface formats`.
- The parity fixture runs both pre-repair and repaired scaler helper bodies in the same 32-bit Windows test executable and compares complete output pixel buffers. Six cases cover destination 8-bit and 32-bit, logical 320x200 scaled to 640x400 / 400x250 / 800x500. **Those cases matched byte-for-byte.** This is not a real game screenshot or game-mode test.
- At the handover check, the overall job was **still running** (`Configure native 32-bit game` step). Do not claim the entire workflow compiled/package succeeded until checking final status and logs.
- Protected v0.7, main and other branches remain unchanged. Interactive Windows gameplay visuals, frame pacing, scrolling, explosions, palette and saves still require actual game data and desktop testing before merge.
