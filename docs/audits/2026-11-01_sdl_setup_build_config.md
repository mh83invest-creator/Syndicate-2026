# Syndicate 2026 — Focused Audit: Screen Setup, Restoration and Windows Build Boundary

Date: 2026-11-01 (Australia/Brisbane)
Status: COMPLETED focused *static-source* investigation; NOT runtime certified.
Branch: `feature/v0.9-visual-showcase`
Baseline: v0.7 `main` at `a22b0b9580a9710b4e693e88854351835ef1ee40`.
Scope: read screen mode setup and restoration source; inspect actual Windows build workflow. No code/workflow changes, tests or architecture migration.

## Confirmed evidence

- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:379+` screen setup calls `LbBaseInitialise()` if needed, retires any previously allocated secondary draw surface, configures fullscreen/windowed state, creates/reuses an SDL window, and fetches a **new window surface** with `SDL_GetWindowSurface`. It may allocate a second draw surface based on bit depth/dimensions and the `LB_WSCREEN_CONTROL` build flag. The code explicitly notes SDL can ignore a requested pixel format, thus checks actual returned format. Do not assume 8-bit physical output even when the logical engine requests 8-bit.
- `sscreen.c:738+` `LbIScreenSurfaceRestoreLost` calls `SDL_GetWindowSurface(lbWindow)` and returns failure if it is NULL. It **does not recreate the draw surface itself**. A prior source audit established `LbScreenSwap` calls restoration and draw-surface checks without inspecting their return codes. This narrows the potential error-recovery hazard; no Windows fault has been reproduced.
- `sscreen.c:617–639` reset invalidates the screen/draw handles; `sscreen.c:642–695` lock/unlock semantics depend on `LB_WSCREEN_CONTROL`; `sscreen.c:1014–1047` checks/replaces a draw surface only in the alternate compilation path.
- `.github/workflows/windows-source-v0.7.yml`: Windows build requests MSYS2 `MINGW32`, validates `gcc -dumpmachine` equals `i686-w64-mingw32`, builds OpenAL Soft 1.24.3 and WildMIDI 0.4.5 locally, and configures source with `--enable-lbs-structs-expand`. **No explicit `--enable-lb-wscreen-control` or `-DLB_WSCREEN_CONTROL` appears in the workflow text.** That is not sufficient to prove whether configure scripts or headers define the macro. This compile-definition question remains open pending configuration/header trace or a real preprocessor build.
- That v0.7 workflow has a dispatch trigger plus pushes restricted to `main` and specific paths. It does not automatically test current v0.9 branch changes and does not run the declared Bullfrog library `make check` test suite.
- The workflow installs the game and checks executables and disallowed MSYS audio imports, but there is no demonstrated launch with commercial game data. The v0.9A one-line window-title change has no dedicated completed build.

## Consequences for the graphics design

1. Select exactly which rendering stage a future effect modifies, and capture its **runtime** pixel format and pitch. Static 8-bit game-space data do not establish 8-bit SDL output.
2. Do not retain SDL window-surface pointers across mode switches or focus events; restoring one surface does not automatically validate every other pointer.
3. Before attempting any renderer implementation, resolve `LB_WSCREEN_CONTROL` from actual build configuration, then validate safe copy/lock lifecycle and surface-error behaviour.
4. Preserve original 16 FPS missions, 30 FPS strategic menus, focus, camera and sound. No changes approved by this audit.

## Verification and limitations

| Check | Status |
|---|---|
| GitHub reads of `sscreen.c` implementation chunks and `windows-source-v0.7.yml` | PASS |
| Static interpretation of SDL setup/restore paths | PASS, source-limited |
| Exact compile-time value of `LB_WSCREEN_CONTROL` | UNRESOLVED |
| Windows i686 compile, compiler preprocessing, DLL inspection | NOT RUN |
| Native output format observation, focus test, scaling harness, gameplay | NOT RUN |
| Graphics implementation or runtime night-lighting proof | NOT PERFORMED |

## Next source audit stage

Trace `LB_WSCREEN_CONTROL` definitions through the autoconf/header/make configuration, with the exact Windows feature flags. Examine `LbIScreenDrawSurfaceCreate` ownership and pitch behaviour. Then continue the assembly pointer lifetime examination around mission transitions, per the prior audit index. No new source changes without separate scope and documented tests.

Rollback: documentation-only feature-branch records; protected `main` unchanged.
