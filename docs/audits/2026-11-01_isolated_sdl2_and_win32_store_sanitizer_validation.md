# Syndicate 2026 — isolated SDL2 surface and Win32-width store tests

Date: 2026-11-01 (Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: diagnostic work external to the repository; documentation only in GitHub.

## Tests actually performed in this turn
1. Local Linux environment had `gcc` and runtime `libSDL2-2.0.so.0`. Using Python `ctypes`, called real `SDL_CreateRGBSurface` for 4-row surfaces and read `SDL_Surface.pitch`; freed each surface. Results: width3/8bpp packed3 actual pitch4; width3/16bpp packed6 pitch8; width3/24bpp packed9 pitch12; width5/24bpp packed15 pitch16; width640/32bpp packed2560 pitch2560.
2. Built an **isolated 4-byte store model**, not the game scaler, using gcc `-fsanitize=address,undefined -fno-omit-frame-pointer -O0 -g`. Six-pixel packed allocation at 2-,3-,4-byte destination strides, writing `uint32_t` at `dst + i*dst_bpp`. On 2-byte stride UBSan flagged misaligned store and ASan reported heap-buffer-overflow. On 3-byte stride UBSan flagged misalignment and process completed; **ASan did not report overflow in this particular run**, though 4-byte writes overlap adjacent 3-byte pixels. Four-byte stride completed without sanitizer warning.
3. Source basis: `engine/bflibrary/src/x86-win-sdl2/sscreen.c:962-965` stores `*((long *)(dst_buf+dst_offset))` then `dst_offset += dst_bpp`. The Win32 i686 ABI has 4-byte long. These tests model the memory-write width and strides, **not the full palette conversion or source sampling**.

## What can and cannot be concluded
- Real SDL2 surfaces can have `pitch > width * BytesPerPixel`, though the tested width640/32bpp surface was tightly packed.
- Under a tightly allocated 2-byte-per-pixel model, a 4-byte store advancing two bytes produced a real AddressSanitizer buffer overflow; 3-byte case had a misalignment warning and overlapping writes, but no ASan report in this run. This reinforces the code-level hazard, **not a reproduced Windows build/gameplay bug**.
- Actual 32-bit Windows `syndicatfx.exe` SDL window surface and blit dispatch still need native runtime observation. Existing CI run 38483184953 confirms SDL2.dll import.

## Status
PASS: five real SDL2 surface cases; isolated C sanitizer 2/3/4-byte cases executed with recorded outcomes.
NOT RUN: original game scaler routine, Win32 executable, actual SDL window surface, Windows gameplay, screenshot comparison and regression testing.
CHANGED: this report and graphics continuity index only. No game engine or other branch modifications.
NEXT: separate, explicitly scoped Windows diagnostic build/run to log actual window surface format, pitch, and dispatch. Do not claim the static audit or these isolated tests certify gameplay.
