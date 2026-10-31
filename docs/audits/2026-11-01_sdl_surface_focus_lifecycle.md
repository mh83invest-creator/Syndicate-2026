# Syndicate 2026 — Focused Audit: SDL2 Surface Lifecycle and Focus Boundaries

Date: 2026-11-01 (Australia/Brisbane)
Stage: SDL2 surface/focus static source review — COMPLETED
Overall forensic audit: INCOMPLETE
Branch: `feature/v0.9-visual-showcase`
Protected reference: `main` at `a22b0b9580a9710b4e693e88854351835ef1ee40`

## Scope
Read original tracked `engine/bflibrary/src/x86-win-sdl2/sscreen.c` and `swindows.c` to determine which screen pointers are stable, where SDL focus affects presentation, and what safety boundaries future lighting/UI patches must observe. This is a read-only continuation of `docs/audits/2026-11-01_display_palette_ownership.md`. No bug fixes, instrumentation, CI, or graphics features were authorised or made.

## Source-confirmed findings

1. **Window surface references are not persistent.** `sscreen.c:727–736` explicitly documents SDL2 window screen surface recreation on events such as Alt-Tab. Therefore lighting or screenshot code must not retain `lbScreenSurface->pixels` across frames/window transitions.
2. **Draw surface lifecycle may change too.** `sscreen.c:1009–1047` checks whether the SDL draw surface pixels match `lbDisplay.WScreen` when application WScreen control is not enabled, and may recreate/free the draw surface when they differ. The alternate `LB_WSCREEN_CONTROL` compilation path behaves differently. Verify the actual compile definition before asserting any single pointer-lifetime model.
3. **Locking has conditional pointer behaviour.** `sscreen.c:642–695` locks the current draw SDL surface; when `LB_WSCREEN_CONTROL` is defined it sets `lbDisplay.WScreen` to SDL pixels, updates pitch-based `GraphicsScreenWidth` and resets pointer fields on unlock. An overlay must not cache `lbDisplay.WScreen` across lock boundaries or assume pixel row stride equals logical width.
4. **Physical window pixel access is temporary.** `sscreen.c:700–724` sets `lbDisplay.PhysicalScreen` on lock and clears it on unlock; any surface manipulation must occur only within the proper lock/ownership lifetime.
5. **Draw-to-window presentation contains cursor hooks.** `sscreen.c:1050–1085` restores/rechecks surfaces, invokes `LbMouseOnBeginSwap`, blits if a second surface exists, updates the window with SDL and then invokes `LbMouseOnEndSwap`. An effect inserted at this boundary risks illuminating or occluding the cursor if it does not respect cursor sequencing.
6. **Restore/check returns are not checked by normal swap.** `LbScreenSwap` calls `LbIScreenSurfaceRestoreLost()` and `LbIScreenDrawSurfaceCheck()` without testing their return values. This is a conditional resilience concern, not a demonstrated failure or explanation for the historical v0.8 focus regression.
7. **Focus events refresh input state.** `swindows.c:104–115` handles `SDL_WINDOWEVENT_FOCUS_GAINED` and `FOCUS_LOST`, sets `lbAppActive`, calls `LbInputRestate()`, then calls screen-surface restoration for window events. No evidence in these lines establishes that SDL spontaneously steals focus; the prior v0.8 playtest remains an observed regression whose root cause is unresolved.
8. **Scaling lock failures require tests.** `sscreen.c:983–1006` logs failures to lock source or destination surfaces and continues to conversion code. Both pitch/format and lock handling require a separate test harness, particularly because custom conversion paths are selected for one-byte source pixels. This was already identified as a risk in the prior audit; no patch was made.
9. **Reset explicitly invalidates pointers.** `sscreen.c:617–639` unlocks when necessary, frees a secondary draw surface, then nulls draw/screen handles and clears initialised state. Do not hook persistent effects into reset without a corresponding resource-lifetime plan.

## Consequences for v0.9 graphics feasibility

- A lighting layer can only be considered after specifying its exact surface (planar scene work image, indexed draw image, or output window surface), lifetime, pitch, pixel format, cursor/HUD compositing order and restoration behaviour.
- Window focus changes and surface recreation are normal SDL2 events here, so testing the focus/Alt-Tab path is a mandatory acceptance gate for future overlays.
- No proof of localized emissive lighting, occlusion, vehicle headlight geometry or performance stability follows from these source reads alone.
- The failed v0.8 camera feature and UI changes remain excluded; no changes to timing or rendering have been committed.

## Verification ledger

| Activity | Status |
|---|---|
| GitHub reading of `sscreen.c` and `swindows.c` on v0.9 branch | PASS |
| Source branch/path/function correlation against earlier handover | PASS, static |
| Actual Windows/native i686 compilation or linked-library checks | NOT RUN |
| Automated SDL renderer test, loss-of-surface fault injection or memory sanitizer | NOT RUN |
| Alt-Tab/focus regression playtest and original resource rendering | NOT RUN |
| Overall audit certification | NOT COMPLETE |

## Exact continuation

1. Inspect `sscreen.c` full bodies of `LbIScreenSurfaceRestoreLost`, `LbIScreenDrawSurfaceCreate` and screen setup, including their failure cleanup and surface ownership; establish compile-time flag value in the Windows workflow.
2. Map `VScreen`/palette assembly assignments across mission, menu, title and transition code using the assembly index and prior asset-free report.
3. Distinguish static risks from measured defects using isolated test-only harnesses and eventually the legally supplied retail resources on Windows.
4. Record each finished stage as a separate GitHub handover and update `docs/CONTINUITY_INDEX.md`.

Rollback: audit documentation only, on the v0.9 branch. `main` untouched.
