# Syndicate 2026 — Main Audit Reconciliation and Development Execution Checkpoint

Date: 2026-11-07 (Australia/Brisbane)
Owner: MAIN systems. User explicitly authorised progression through audit reconciliation, baseline verification, diagnostic build and requested improvements, with Graphics handled in a separate chat.
Main branch source reference at reconciliation: `0538467009c7446ff8a0376c869fa2e2a670da72` (before this document).
Protected version: **v0.7**, original gameplay speed and 16 FPS tactical, 30 FPS on three strategic/menu screens. Historical user-tested v0.7 package remains the playable reference.

## Step 1 — Audit reconciliation: performed, merges deliberately withheld

Compared live `main` with `feature/mission-builder-audit`, `audit/graphics-assets-2026-11-01`, and `feature/v0.8-scroll-speed` using GitHub compare:
- Mission Builder: branches diverged, ~176 commits ahead and ~259 behind main as inspected, with original branch handover `docs/mission-builder/HANDOVER.md`. Its `CONSOLIDATED_AUDIT_READINESS_2026-11-01.md` says **not ready to merge**; requires authentic mission fixtures, safe RNC decoder, parser/write/pack tests, campaign separation and Windows integration.
- Graphics audit: diverged, ~262 ahead and ~259 behind; its own continuity handover documents isolation and asset/rendering constraints. Graphics changes **reserved for separate user chat**; no graphics branch merged, modified or cherry-picked.
- Existing v0.8 camera branch: diverged, ~11 ahead and ~259 behind. Its release notes record **failed Windows gameplay validation**: no perceptible speed adjustment, upward camera veering left, unexpected desktop focus switches. This version is not a playable release, and its prototype controls do **not** satisfy the required visual scroll-speed slider.
- Main systems: holistic source architecture, effects/collision, audio, rendering, saving, DLL/build, resources and ABI have extensive static audits. Known unresolved issues require runtime measurement or fault injection. Existing long handovers are evidence, not a release certification.

**Merge result: NONE.** Branches intentionally remain separate to preserve game, assets and campaign data.

## Step 2 — Baseline identification and reproducibility: partially verified

- Historical successful Windows i686 build `38473241548` at commit `25fdd79136302a81ed91e9b6e26770ca8f86cbfc`, completed 2026-10-13 (UTC), confirmed via GitHub Actions result.
- Another `main` build `38483184953` completed success at `a22b0b9580a9710b4e693e88854351835ef1ee40` (2026-10-13 UTC).
- Source comparison `a22b0b9580a9710b4e693e88854351835ef1ee40...main` reported 259 commits ahead and **no changed `engine/` or `.github/` files**. Therefore code and verified Windows workflow remained byte-identical at file level over those documentation commits. This does **not** show that current main was rebuilt today.
- v0.7 release handover reports historical successful user game testing on a separate retail-data-inclusive local package; current source-rebuilt executable was not separately playtested.
- Existing Windows workflow `.github/workflows/windows-source-v0.7.yml` is still intact; source build uses 32-bit MinGW32, native OpenAL/WildMIDI, SDL2 and does not commit retail game data. `restore-v0.7-source.yml` is a powerful `contents:write` source-replacement workflow: **do not run or change it**.
- Fresh v0.7 executable verification/gameplay at this checkpoint: **NOT DONE**.

## Step 3 — Isolated diagnostics: source and CI workflow implemented; runtime evidence pending

Created branch `diagnostics/explosion-frame-timing-2026-11-07` from `main`.

- `engine/src/game.c`: compile-time gated `SYNDICATE_DIAGNOSTICS` SDL performance-counter aggregates for C update/pacing, initial SDL event pump and subsequent wait phase (tactical/menu separately). Logging only on `game_quit` into an SDL user-writable diagnostics file with stderr fallback; no retail game data logged. **Not full explosion/effects/renderer/audio profiler.**
- `engine/src/Makefile.am`: define `SYNDICATE_DIAGNOSTICS` **on diagnostic branch only**.
- `.github/workflows/windows-diagnostics-frame-timing.yml`: branch-gated isolated 32-bit Windows build with read-only token; separate output artifacts; copies established native-toolchain source build method.
- Versioned branch handover `docs/releases/v0.7-D1-diagnostics.md` documents measurements, limits and tests. Latest source/diagnostic workflow commit after report creation: `6c0ea0c001b837ec4a04fddade95cb6a776b3a64`.
- GitHub Actions run `42347343321` triggered by isolated diagnostics workflow at commit `560cdf15c553ecb634c8c9ffc53395d49ac9e6f6`. **In progress when inspected**; no success claim. The documentation-only handover commit following the trigger does not change executable source.
- GitHub file readback passed for C source, Makefile.am, diagnostics workflow and handover. This is a static verification, not compiled proof.
- No changes to `main` engine, gameplay assembly, graphics, sound, camera, mission data or saves.

**Diagnostic limitations:** no actual game tick/move_effects/MapWho or renderer timing, no per-frame histograms; C pacing data alone cannot diagnose explosion slowdown. A later isolated probe pass needs verified assembly ABI, instrumented reference comparison and actual Windows runtime/CI.

## Step 4 — Improvements: root-cause and regression gates, implementation withheld

This authorisation permits development, but it is not justification to merge unverified optimisations:
1. Explosion slowdown first: preserve original fireball count, simulation, damage, sound, AI, 16 FPS. Perform equivalent baseline/diagnostic tests of idle, single blast, overlap and combat; separately measure effects, linked MapWho traversal, SDL presentation and AIL timer servicing. Only after identifying top measured cost, implement a single isolated and behaviour-preserving optimisation and regression-test it.
2. Camera: do not merge known-failed `feature/v0.8-scroll-speed`; inspect wrapper register/stack misuse and original input coordinate normalisation in a separate controlled fix branch. Requirement remains visible slider with default exactly v0.7 speed, persistence, no agent/simulation speed change, four edges/corners. No slider implementation delivered here.
3. Menu: three verified 30 FPS strategic screens already present in protected v0.7; do not alter frame cadence globally. Any further menu responsiveness changes require a distinct demonstrated issue and regression tests.
4. Saves: campaign-specific isolation (Original/American Revolt and future custom modes) and transactional save integrity are documented missing features; no save source changes without format/version/legacy migration validation.
5. Graphics: explicitly excluded; user will work separately in another chat. No cross-branch integration into graphics.

## Testing and release status

| Check | Result |
| --- | --- |
| GitHub branch compare/reconciliation | PASS (source metadata, not integration or game test) |
| Original engine source unchanged between v0.7 ref and pre-document main | PASS (file-level compare) |
| Historical v0.7 Windows CI | PASS (October 2026, not a new build) |
| Diagnostic source/CI files present and read back | PASS |
| Diagnostic Windows CI | IN PROGRESS at time of report, not claimed PASS |
| Diagnostic binary execution and output-file check | NOT TESTED |
| Explosion profiling and optimisation | NOT DONE |
| Camera slider or v0.8 regression repair | NOT DONE |
| Campaigns and saves regression, Mission Builder merge, graphics merge | NOT DONE |

## Handover

- Main engine code: unchanged; no release candidate promoted.
- Diagnostic source branch: `diagnostics/explosion-frame-timing-2026-11-07`; handover `docs/releases/v0.7-D1-diagnostics.md`.
- Workstream boundaries: leave `audit/graphics-assets-2026-11-01`, `feature/mission-builder-audit`, `fix/graphics-sdl2-scaler-isolated-2026-11-01` and historic broken v0.8 camera prototype untouched.
- Next practical dependency: verify diagnostic Windows CI; collect original-game Windows diagnostic logs; add full simulation/renderer/audio stage probes after compiling safely; then implement measured fixes in isolated branches. Do not claim a release before actual Windows and user gameplay test.
