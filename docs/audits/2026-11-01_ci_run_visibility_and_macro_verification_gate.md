# Syndicate 2026 — GitHub Actions visibility and SDL macro verification gate

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: read-only graphics audit, following Windows build / SDL backend source trace.

## New GitHub Actions check
The available GitHub connector's `fetch_commit_workflow_runs` action was executed for graphics continuity commit `3d5aa83d24b00076572ad4bf0c5e921830ea931d`. It returned **zero workflow runs**. Crucially, this connector action is documented to filter to **pull-request-triggered runs** and its first page only. Therefore the empty result must NOT be interpreted as no workflow, no Windows CI, no build, or build failure; a direct-push documentation commit is not an adequate test of that API.

## Still established by source
- `engine/bflibrary/Makefile.am:123-156` selects `src/x86-win-sdl2` or `src/x86-win-sdl` with `USE_SDL2`.
- `engine/bflibrary/configure.ac` chooses SDL2-first and defines `WSCREEN_CONTROL` when the opt-in setting is enabled; the screen C implementation checks `LB_WSCREEN_CONTROL`. Missing or present preprocessing alias has **not** been established.
- Windows MSYS2/MinGW 32-bit recipe and SDL2 package dependency are documented in `engine/README.md:306-374`; this is not proof of current Actions job configuration.
- The GitHub file-fetch action reports that `.github/workflows` is a directory but cannot enumerate its contents; guessed paths and default-branch search are inadequate substitutes for a real tree listing.

## Recommended next audit evidence
Obtain a real workflow directory inventory through GitHub's tree/contents API or a full repository checkout. Identify Windows workflow file and its exact build commit; inspect run/job IDs, logs, configure line, generated config headers and linked SDL dependencies. Check preprocessor handling of `WSCREEN_CONTROL` vs `LB_WSCREEN_CONTROL` using actual compiler flags. Only after these can the Windows surface/pitch behaviour be responsibly attributed to one backend.

## Verification ledger
PASS: GitHub Actions connector returned zero PR-filtered runs for the specific graphics commit; source references above were verified in previous reports.
NOT DONE: exhaustive workflow enumeration, generic push-workflow listing, build log inspection, generated preprocessor output, compilation, Windows gameplay or graphics testing.
Changed: report and graphics continuity only. No source files, protected v0.7 baseline or other branches touched.
