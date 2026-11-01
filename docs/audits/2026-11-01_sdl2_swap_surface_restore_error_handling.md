# Syndicate 2026 — SDL2 swap path, surface restoration and return-value handling

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: static graphics audit only; no source modifications.

## Confirmed source behaviour
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:284-325`: secondary drawing surface is created by wrapping application-managed `lbDisplay.WScreen` using `SDL_CreateRGBSurfaceFrom` when applicable, or by allocating a surface using `SDL_CreateRGBSurface`; `lbHasSecondSurface` is set.
- `sscreen.c:497-512`: screen setup obtains SDL window surface through `SDL_GetWindowSurface`.
- `sscreen.c:727-745`: `LbIScreenSurfaceRestoreLost` refreshes the SDL window-surface reference; comments explain SDL2 window operations (including focus changes) can recreate the surface. It returns success/failure.
- `sscreen.c:1060-1085`: `LbScreenSwap` performs mouse begin/end swap; if secondary surface exists, it invokes `LbI_SDL_BlitScaled(draw,window)` then updates the window with `SDL_UpdateWindowSurface`. Thus indexed-to-true-colour scaler can be reached in the frame presentation path subject to documented format/dimension conditions.
- `sscreen.c:1088-1118`: `LbScreenSwapClear` uses the same scaled-copy and SDL window-update pattern.
- `sscreen.c:1093-1097` calls `LbIScreenSurfaceRestoreLost` and `LbIScreenDrawSurfaceCheck` without using their return values before proceeding. The analogous `LbScreenSwap` preamble must be checked against the complete function before any generalized claim; the presented excerpt establishes this for SwapClear.
- `sscreen.c:839-850`: fullscreen mode logic explicitly tolerates certain 640x400/640x480 indexed or RGB24 requests without an exact matching hardware display mode; pixel format equality with closest mode is not enforced.

## Risk and open questions
- Surface restoration and rebinding are designed into the swap route. Discarded failures in SwapClear could permit later operations with invalid/stale surfaces, but no failing runtime call has been observed, and no defect is claimed.
- SDL's actual window surface format, row pitch and lifetime vary with environment; static requested formats do not prove the format used by the current Windows build.
- Custom scaled indexed presentation still requires source/destination sizes to differ and source to be one-byte per pixel; 16/24-bit target write-width concerns documented in prior audit. Padded destinations can also be mishandled as demonstrated by isolated synthetic tests.

## Next test
For a real Windows build, trace SDL backend selection, runtime window/draw surface dimensions, `format->BytesPerPixel`, `pitch`, and error returns from restore/check/swap under window resize, fullscreen toggle and focus changes. No changes authorised without reproducible evidence.

## Verification
PASS: direct GitHub source read of SDL2 surface allocation, window restoration, swap/SwapClear and fullscreen format logic.
NOT TESTED: Windows executable, live SDL surface formats, focus-change behaviour, game screenshots, runtime sanitizer or private graphics.
Changed: documentation report and graphics continuity index only. Protected baseline and other audit branches untouched.
