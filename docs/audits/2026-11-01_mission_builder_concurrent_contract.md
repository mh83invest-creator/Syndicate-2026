# Syndicate 2026 — Engine Audit and Mission Builder Coordination

Date: 2026-11-01 (Australia/Brisbane)
Status: cross-branch audit handover; source reading only. No gameplay or workflow modifications.
Engine branch: feature/v0.9-visual-showcase. Mission Builder branch: feature/mission-builder-audit.
Main coordination: docs/SHARED_DEVELOPMENT_HANDOVER.md on main.
Mission Builder handover: docs/mission-builder/HANDOVER.md on its own branch (currently MB-AUDIT-0.3).
Protected source baseline: v0.7; main has subsequently advanced so compare before integrating.

## Source findings to share
- Original engine/src/syndre.sx assembly is available by GitHub blob SHA c35e22621a93d1e07c44d40de8867446ff7c64bf (85,803 lines); a connector per-line fetch returning blank does not mean the source is missing.
- syndre.sx:25694–25716 init_map_data converts 12,288 four-byte map offsets to process-relative pointers in place. Do not treat an initialized map buffer as pristine disk data.
- syndre.sx:82620–82645 mission_load_files selects mapNN.dat, col01.dat, hblk01.dat, hspr-0.dat and hspr-0.tab as separate resources. The builder must validate this legacy loading contract.
- syndre.sx:27886–27913 computes map filename digits and calls ac_LbDataLoadAll for mission files, then optionally loads sound, and calls init_hires_blocks; immediate loader return is not checked.
- syndre.sx:79740–79781 init_hires_blocks derives a per-block six-bit sample-mask table from block graphics. It is not a lighting or mission-objective interface.
- syndre.sx:71385–71425 and 71628–71739 map columns point into 24-byte block descriptors and planar multi-face composition.
- syndre.sx:31003–31010 and 31238–31253 use DrawFlags to select graphics resource modes; do not confuse this with a selectable campaign mode.

## Workstream ownership and integration boundary
Engine audit: SDL surface/palette lifecycle, renderer, graphics data, camera, profiling and visual tests.
Mission Builder audit: campaign menu choices, briefing, objective types, mission definitions, revolt selection, save schema and World Domination state.
Joint review required before touching mission loading, menu dispatch, objective data, saves, map/agent initialization, state transitions, global buffers or build workflow.
Do not add new code, merge branches, alter protected main, or import FreeSynd. The original campaign and American Revolt must remain unchanged. The Mission Builder handover's conceptual APIs are not established source functions.

## Verification
PASS: read shared main coordination note, concurrent Mission Builder handover, compared branches and read original source assembly.
NOT RUN: Mission Builder compilation, Windows gameplay, cross-branch integration, saves, original-asset loading under new mode.

Next: Mission Builder investigators should use original assembly blob fetch and exact source lines above, then define a reviewable integration contract; engine audit continues independently. Retail resources remain private. Rollback: documentation only.