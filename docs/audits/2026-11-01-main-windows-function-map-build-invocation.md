# Syndicate 2026 — MAIN audit: Windows function mapping and actual source-build invocation

Date: 2026-11-01 (Australia/Brisbane). Documentation only, main branch.

## Source evidence
- `engine/conf/fnmap_windows.conf`, blob `d74b6dcdec9d48b83899abcf9aa32c0a2b59475e`.
- `engine/configure.ac`, blob `3deb91f7da5abb9b44eb20351f4b1807f9445fd1`.
- `engine/src/Makefile.am`, blob `dfaad56c157bd9042bec49e681b619794b0fcba3`.
- `.github/workflows/windows-source-v0.7.yml`, blob `ab9c099923d859e85f996e0c11823cf82203f5d9`.
- `.github/workflows/restore-v0.7-source.yml`, blob `eaee0ac2a057daf86942483a4f08d85756626ecf`.

## Confirmed
1. The Windows rename file maps C runtime functions to underscore-export variants, including `access -> _access`, `chdir -> _chdir`, `close -> _close`, `read -> _read`, `write -> _write`, `stat -> _stat`, and `strcasecmp -> _stricmp`. This is a limited function-name translation, not wholesale renaming of game routines.
2. `configure.ac` selects MinGW/Windows, passes `fnmap_windows.conf` as a generated-wrapper rename mapping, retains 4-byte default alignment in the inspected configuration, and removes `-g` from assembler flags for very large `syndre.sx`.
3. Actual Windows CI workflow: `.github/workflows/windows-source-v0.7.yml` runs on `windows-latest`, configures an MSYS2/MINGW32 build, installs i686 toolchain, Python and SDL2-related dependencies, and builds pinned OpenAL Soft 1.24.3 and WildMIDI 0.4.5 source dependencies.
4. Workflow explicitly checks the compiler target is `i686-w64-mingw32`, regenerates configure scripts, runs `../configure --prefix=/ --enable-lbs-structs-expand`, then `make -C release V=1`; it stages/exports Windows build artifacts. The workflow includes an audio import-library linkage adjustment; diagnostics must not silently remove that established step.
5. A separate `restore-v0.7-source.yml` imports an external pinned engine and has a source replacement/commit stage. This is **not** an appropriate diagnostics pipeline; any profiling build must avoid launching that restoration workflow or modifying its baseline.
6. Earlier guessed workflow names `windows.yml` and `build.yml` were not present. Directory enumeration confirmed the actual two workflow filenames; this report relies only on real files.

## Future isolated diagnostic build — proposal, NOT implemented
Use a separate explicitly approved diagnostic branch or worktree based on the protected baseline; retain pinned 32-bit toolchain/SDL2/OpenAL/WildMIDI and current build flags. Keep diagnostics off by default and never edit existing release workflow or restoration workflow. Start with high-resolution bounded C-boundary instrumentation; verify any assembly wrapper additions against actual generated objects and Win32 ABI before use. Collect quiet/combat/explosion traces with probe overhead and baseline comparison. Preserve gameplay 16 FPS, original effects, sound, campaign saves and UI controls.

## Verification
Read GitHub source and CI workflow definitions only. Did not run GitHub Actions, compile, install dependencies, profile, inspect produced binaries or edit engine/workflows. Graphics Audit and Mission Builder untouched.
