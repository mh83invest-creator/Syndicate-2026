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
