# Syndicate 2026 — Native SDL2 surface pitch probe (Linux, not Windows gameplay)

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: non-invasive diagnostic investigation. Graphics audit only, game source untouched.

## Why this probe
The consolidated graphics audit identified custom SDL2 scaler helpers in `engine/bflibrary/src/x86-win-sdl2/sscreen.c:878-971` which advance packed pixel offsets without reading `SDL_Surface.pitch`. The 32-bit Windows v0.7 executable is known from actual CI job `38483184953` to import `SDL2.dll`, but its runtime window surface dimensions, format and pitch have not been logged.

## Runtime surface test actually performed
This session's local Linux environment has `libSDL2-2.0.so.0` but no SDL2 development headers or `sdl2-config`. Ran a Python `ctypes` probe against **real SDL2 runtime library** calling `SDL_CreateRGBSurface(0,width,4,bpp,0,0,0,0)`, reading `SDL_Surface.pitch` and `SDL_PixelFormat.BytesPerPixel`, then freeing each surface. Sizes: width 3, 5, 320 and 640 for each of 8/16/24/32 bits. 16 surfaces successfully created and queried.

| BPP | width | BytesPerPixel | packed row (bytes) | actual SDL pitch (bytes) |
|---:|---:|---:|---:|---:|
| 8 | 3 | 1 | 3 | 4 |
| 8 | 5 | 1 | 5 | 8 |
| 8 | 320 | 1 | 320 | 320 |
| 8 | 640 | 1 | 640 | 640 |
| 16 | 3 | 2 | 6 | 8 |
| 16 | 5 | 2 | 10 | 12 |
| 16 | 320 | 2 | 640 | 640 |
| 16 | 640 | 2 | 1280 | 1280 |
| 24 | 3 | 3 | 9 | 12 |
| 24 | 5 | 3 | 15 | 16 |
| 24 | 320 | 3 | 960 | 960 |
| 24 | 640 | 3 | 1920 | 1920 |
| 32 | 3 | 4 | 12 | 12 |
| 32 | 5 | 4 | 20 | 20 |
| 32 | 320 | 4 | 1280 | 1280 |
| 32 | 640 | 4 | 2560 | 2560 |

## Interpretation
- **Verified SDL2 runtime property**: SDL2-created surfaces can contain row padding, with `pitch > width * BytesPerPixel` for tested non-aligned examples. Therefore source assumption of packed rows is not generally valid across SDL surfaces.
- **Important limitation mitigating typical case**: tested width 640, BPP 32 has exactly packed pitch 2560; the normal requested 640x400, 32-bit destination would not encounter *this particular* pitch mismatch if SDL returned that geometry and format. The tested 320- and 640-wide allocated surfaces were all tightly packed. This does not eliminate source memory-format issues for differing actual SDL window surfaces or unusual strides.
- The SDL2 custom indexed-to-true-colour scaler still writes `long` while advancing by `dst_bpp`, making 16/24-bit formats independently concerning even when the pitch is packed (previous synthetic source-derived harness evidence).
- These observations are from **newly allocated offscreen SDL surfaces in a Linux runtime**, not `SDL_GetWindowSurface` in an actual 32-bit Windows v0.7 executable. Actual Windows window surface format, dimensions, pitch and dispatch remain **not verified**.
- This test **did not execute the game, the source scaler, Windows builds, or actual screen presentation**. There is no claim of observed gameplay corruption.

## Test execution / environment ledger
PASS: Python `ctypes` against local SDL2 runtime, 16 allocated offscreen surface configurations and pitch measurements, all freed. Compiler gcc installed; SDL2 development headers and sdl2-config absent.
NOT RUN: Windows SDL2 surface probe, game launch, screen capture, runtime scaler/error injection, fixes or baseline regression tests.
CHANGED: this documentation and graphics continuity index only.
NEXT gate: obtain Windows executable and instrument safe isolated diagnostic build for actual `SDL_GetWindowSurface` `w/h/BytesPerPixel/pitch`, plus lowres stretch, fullscreen/windowed and mode transitions. Seek approval before changing game engine.
