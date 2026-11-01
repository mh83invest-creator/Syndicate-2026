# Syndicate 2026 — Windows MSYS2 build instructions and SDL backend Makefile selection

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: read-only graphics build-trace continuation.

## Directly verified
1. `engine/bflibrary/Makefile.am:123-156`: `if USE_SDL2` selects the `src/x86-win-sdl2/` versions of screen, palette, surface, mouse and related library sources; the `else` clause chooses their `src/x86-win-sdl/` SDL1 equivalents. The build does not compile both source sets under the same conditional.
2. `engine/bflibrary/configure.ac:143` assigns the `USE_SDL2` Automake condition from the `have_sdl2` result. The preceding graphics audits verified SDL2-first detection (minimum 2.0.0) with SDL1 fallback (minimum 1.2.0).
3. `engine/README.md:306-374` documents a **Windows MSYS2 32-bit MinGW** build, installing `mingw-w64-i686-SDL2` and other 32-bit dependencies and using `autoreconf`, `../configure --prefix=/mingw32 --enable-lbs-structs-expand`, and `make V=1` with `-m32` flags. These are upstream project instructions, not observed CI commands or verification of the current Windows executable.
4. `engine/bflibrary/configure.ac:27-35` defines `WSCREEN_CONTROL` when its separate option is enabled; inspected SDL1/SDL2 screen files use `LB_WSCREEN_CONTROL` conditional checks. No verified alias header or active compiler definition identified in this pass; the macro mismatch remains unresolved.
5. `.github/workflows` was verified as a directory, but the available GitHub file-fetch action refuses to return directory listings. Four guessed workflow file paths previously returned 404. The available GitHub code-search responses returned no useful workflow matches and search is limited to default branch. Therefore no particular Windows Actions workflow filename or job should be asserted.

## Consequence
The source-level backend selection is clearer: Automake USE_SDL2 determines which SDL implementation files are built. A Windows recipe installs SDL2 but does NOT prove all Windows builds, especially GitHub Actions, use SDL2. For graphics integration, determine the exact executable's build flags/linked SDL libraries before assuming backend, pitch handling or palette behaviour.

## Next investigation
Obtain an actual GitHub directory/tree listing or Windows workflow file reference via supported repository/CI access; inspect build job logs and generated config headers, particularly `WSCREEN_CONTROL` and `LB_WSCREEN_CONTROL`. Then use an isolated native build and actual runtime screenshots to verify indexed palette, pitch and scaled blits.

## Verification
PASS: direct Makefile.am, README and Autoconf GitHub reads and checked .github/workflows directory type.
NOT DONE: exhaustive directory file enumeration, Windows workflow job identification, compiled macro inspection, Windows build/run/screenshot or original game asset validation.
No gameplay code, protected baseline, parallel audits or commercial graphics were changed.
