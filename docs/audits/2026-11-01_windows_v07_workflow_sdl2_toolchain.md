# Syndicate 2026 — Windows v0.7 workflow and SDL2 build evidence

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics and Windows build-source audit. No code changes.

## Newly located workflow files
GitHub repository tree on main lists:
- `.github/workflows/windows-source-v0.7.yml`
- `.github/workflows/restore-v0.7-source.yml`
- `engine/.github/workflows/build.yaml`
- `engine/.github/workflows/build-sdl1.yaml`

The main Windows build workflow was fetched and inspected **from graphics audit branch** at `.github/workflows/windows-source-v0.7.yml`. This resolves the prior uncertainty about locating the root workflow. Engine-nested workflows are distinct source files, not necessarily active workflows in the repository root GitHub Actions directory.

## Confirmed Windows build contract in root workflow
- Job `windows-i686` on `windows-latest`, MSYS2 shell with `msystem: MINGW32`.
- Installs `mingw-w64-i686-toolchain`, `mingw-w64-i686-SDL2` and other 32-bit dependencies.
- Explicitly checks for `/mingw32/include/SDL2/SDL.h`, prints `pkg-config --modversion sdl2` and asserts compiler target `i686-w64-mingw32`.
- Runs `autoreconf -ivf` and `../configure --prefix=/ --enable-lbs-structs-expand` with `CFLAGS=-m32`, `CXXFLAGS=-m32`, `LDFLAGS=-m32 -L/mingw32/lib`.
- Compiles using `make -C release V=1`, validates executable packaging, and uploads artifact `syndicate-2026-v0.7-verified-source`, without retail assets.
- The root workflow uses an SDL2 installation and does not pass `--without-sdl2`. The source configure script (`engine/bflibrary/configure.ac`) probes SDL2 first and sets `USE_SDL2` when available; `engine/bflibrary/Makefile.am` then selects SDL2 sources. This is strong **workflow/configuration evidence** for SDL2 in a successful Windows v0.7 build. It does not prove the exact executable DLL imports without inspecting artifacts.

## Other workflow distinction
`engine/.github/workflows/build.yaml` specifies Windows MINGW32 with `mingw-w64-i686-SDL2`; `engine/.github/workflows/build-sdl1.yaml` uses `mingw-w64-i686-SDL` and SDL1. These are separate engine-nested files. No claim made that these are run by the root repository's Actions dispatcher.

## Relation to graphics risk
The SDL2 path is relevant to `engine/bflibrary/src/x86-win-sdl2/sscreen.c` custom indexed-to-truecolour scaler. Win32 `long` generally occupies 4 bytes, unlike Linux LP64 source-derived harness with 8-byte `long`. Earlier 4-byte-store synthetic tests showed a packed-buffer overhang for 16/24-bit destinations, and padded row layout errors; actual Windows window surface BPP, pitch, dimensions and branch dispatch remain unmeasured.

## Verification ledger
PASS: GitHub tree discovery of workflows; audit-branch direct reads of actual root Windows v0.7 workflow and two engine-nested workflows; compared relevant source configure/Makefile selection.
NOT TESTED: actual generated executable, DLL import table, CI run state, SDL window-surface runtime format, graphics or screenshots.
CHANGED: this report and graphics continuity index only; protected v0.7 engine, Mission Builder and other branches untouched.
NEXT: inspect available root Windows workflow runs and artifacts (without claiming tests that were not run), establish executable SDL2 DLL imports, then conduct isolated Windows surface-format/pitch instrumentation on a separate approved test branch.
