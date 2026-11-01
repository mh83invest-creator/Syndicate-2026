# Syndicate 2026 — Windows v0.7 artifact verification boundary

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics/build audit; no executable changes.

## Directly checked configuration
- `.github/workflows/windows-source-v0.7.yml:4-7`: workflow_dispatch enabled; push builds only on `main` for specified paths. A graphics docs-only audit commit does **not** satisfy the push trigger.
- Same workflow: installs `mingw-w64-i686-SDL2`, checks `/mingw32/include/SDL2/SDL.h` and `pkg-config --modversion sdl2` (near lines 58 and 94-96).
- `engine/bflibrary/configure.ac:24-105,143`: configure tries SDL2 by default unless explicitly disabled and uses SDL1 fallback otherwise; the root Windows workflow does not pass `--without-sdl2`.
- The workflow invokes `objdump -p release/src/syndicatfx.exe` (line 153) to print DLL imports, but its automated rejection guard (line 154) checks only for MSYS OpenAL/WildMIDI DLLs, not SDL backend DLL identity.
- Therefore successful execution of this workflow would be strong evidence for SDL2 selection, but its *configured checks alone* do not prove the SDL DLL imported in a particular built executable.

## Attempted run/artifact retrieval and limitation
Tried direct GitHub Actions workflow run and repository artifact listings via the connected GitHub resource fetch interface:
- `/actions/workflows/windows-source-v0.7.yml/runs?per_page=10`
- `/actions/artifacts?per_page=20`
Both were rejected as unsupported URL endpoints by the connector (HTTP 400 invalid argument). No run/artifact list or executable bytes were retrieved. The specialized available commit workflow runs action only returns pull-request-triggered runs and is unsuitable to infer push/dispatch build success from its absence.

## Remaining runtime gates
1. Obtain a known root workflow run ID (e.g. through an allowed connector view) and inspect its artifacts; **not done**.
2. Verify actual `syndicatfx.exe` import table includes SDL2 or SDL1 DLL; **not done**.
3. Capture `SDL_GetWindowSurface` and draw-surface dimensions, BitsPerPixel, BytesPerPixel, pitch, and exact blit dispatch in a safe isolated Windows diagnostic build; **not done**.
4. Reconcile actual game mode/dimension combinations before assigning runtime reachability/severity to the 16/24-bit write-width and padded-pitch scaler risks.

## Verification ledger
PASS: direct graphics-branch reads of Windows workflow and SDL selection configure script; known API requests rejected as recorded.
NOT VERIFIED: CI status, compiled artifact, DLL imports, SDL formats/pitch, runtime gameplay, graphics output.
Changed: this report and graphics audit continuity index only. Protected baseline and other branches not modified.
