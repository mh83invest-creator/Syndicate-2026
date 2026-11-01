# Syndicate 2026 — SDL2 screen ownership under LB_WSCREEN_CONTROL

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: documentation-only static SDL2 source inspection.

## Confirmed conditional behaviour
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:520-539`: with `LB_WSCREEN_CONTROL` defined, secondary surface creation is conditional on source/physical BPP or dimensions differing; without the macro, the guarded condition is absent, so the surface-create block executes.
- Lines 551-554: if defined, setup explicitly sets `lbDisplay.WScreen` and graphics window pointer to NULL.
- Lines 642-651: screen-lock status checks differ by macro state.
- Lines 654-678: if defined, `LbScreenLock` refreshes `lbDisplay.WScreen` from the SDL draw surface's `pixels`, sets `GraphicsScreenWidth` to actual `pitch`, and recomputes the graphics window pointer. On lock failure in this path, pointers are nulled.
- Lines 681-690: if defined, `LbScreenUnlock` nulls the two library display pointers.
- Lines 1014-1045: when defined, `LbIScreenDrawSurfaceCheck` returns immediately; otherwise it compares current draw-surface pixel pointer with `lbDisplay.WScreen` and recreates the drawing surface when they differ.
- These branches describe materially different pointer ownership and lifecycle. Neither branch establishes a defect without knowing actual macro definitions and runtime state.

## Configuration gap and CI access
- `engine/bflibrary/configure.ac:33` defines `WSCREEN_CONTROL`; SDL screen source checks `LB_WSCREEN_CONTROL`. No verified header mapping yet.
- GitHub file connector refuses directory reads for `.github/workflows`; the PR-filtered Actions lookup did not identify the active workflow. An attempted public GitHub API web route was inaccessible. This does not prove the workflow is absent.
- Do not infer which branch was used in current Windows build from source defaults or documentation.

## Consequences / next step
Resolve actual generated config/header and compiler defines before touching rendering code. Obtain workflow filename and compile logs through an authoritative tree listing, checkout or user-provided CI logs; then test both display modes and screen pointer restoration on Windows. Maintain protected v0.7 and separate core/mission audits.

## Verification
PASS: direct branch source inspection of SDL2 conditional blocks at stated lines.
NOT DONE: macro mapping, generated configuration, active CI determination, build, runtime graphics tests or private asset review.
CHANGES: this report plus graphics handover index only.
