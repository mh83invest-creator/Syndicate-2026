# Syndicate 2026 — Graphics Audit Continuity Index

Updated: 2026-11-01 (Australia/Brisbane)
Active isolated branch: `audit/graphics-assets-2026-11-01`
Forked from: `feature/v0.9-visual-showcase` at `d42db5258f2786ea26a8e6e3ee235e09fa404cf6`
Protected original playable-source baseline: `main` at `a22b0b9580a9710b4e693e88854351835ef1ee40`

## Read in this order
1. `docs/HANDOVER_POLICY.md`
2. `docs/GRAPHICS_AUDIT_CONTINUITY.md` (this file)
3. `docs/audits/2026-11-01_graphics_asset_parallel_audit.md`
4. `docs/CONTINUITY_INDEX.md` for the concurrent original audit's snapshot at fork.
5. `docs/releases/v0.9.md`, `docs/SOURCE_AUDIT_PALETTE_GROUPS_AND_BUILD_2026-11-01.md`, and existing original graphics/menu asset analyses.

## Stage 1: graphics asset evidence reconciliation
Status: COMPLETE as a documentation-only audit stage. No new visual inspection, frame decoding, or engine test is implied.
Handover: `docs/audits/2026-11-01_graphics_asset_parallel_audit.md`
Exact scope: reconcile already independently documented original asset sizes, palette sharing, menu animation findings and source rendering limitations.
Engine modifications: none.
Tests: GitHub access and report read-back; no native compile, Windows game playtest or graphics equivalence tests.

## Parallel development policy
- This branch: read-only graphics asset analysis and dated handover documentation only.
- `feature/v0.9-visual-showcase`: separate source/engine/visual development audit workstream.
- `feature/mission-builder-audit`: mission builder workstream.
- `main`: protected v0.7 reference. No direct changes.
- On publication, compare branches, resolve docs conflicts intentionally, and review before merging.
- Do not put commercial game data or reconstructed copyrighted frames in public GitHub.

## Next stage
Trace hspr-0.tab against original assembly drawing code and record verified metadata boundaries. Continue with tile/block resource mapping, sprite animation classes, explosion frames, then per-mission palette and lighting feasibility. Add a dated report to `docs/audits/` for each completed stage and append the result here; distinguish asset parsing, automated checks and real Windows rendering.

## Parallel graphics audit coordination — 2026-11-01

A third documentation-only workstream has been established:
- Branch: `audit/graphics-assets-2026-11-01`, forked from `feature/v0.9-visual-showcase` at commit `d42db5258f2786ea26a8e6e3ee235e09fa404cf6`.
- Continuity: `docs/GRAPHICS_AUDIT_CONTINUITY.md` on the graphics branch (commit `82c0b4c4e54c05ddf5a99f666b1ba1a1523a766d`).
- Stage-1 evidence handover: `docs/audits/2026-11-01_graphics_asset_parallel_audit.md` on the graphics branch (commit `a012ee8e32b8c68c62a9c0fa6d5ae6fd63026880`).
- Scope: inventory and source-to-asset correlation of original tiles, building faces, sprites, animations, effects, palettes, menu art and constraints relevant to proposed night lighting. Stage 1 reconciled existing evidence only; it is **not** a new pixel-level audit, renderer patch, CI build or Windows test.
- Ownership: core source and v0.9 graphics implementation remain with `feature/v0.9-visual-showcase`; mission definitions and save-mode interfaces remain with `feature/mission-builder-audit`; graphics workstream records read-only asset findings and proposals, with no gameplay edits.
- Integration contract: any proposed modifications to tile indices, palette selection, sprites, mission resource filenames, asset loaders, render buffers, map/object representation or screen overlays must be reviewed across all three workstreams before implementation. Keep the original and American Revolt modes, campaign-siloed save requirements, and protected v0.7 reference intact.
- Confidentiality: original retail assets and reconstructed copyrighted frames must not be committed publicly.
- Synchronisation: each branch is independent; the presence of this link does not mean another branch has received or merged its commits. Updates are not automatic; new findings need an explicit dated cross-branch handover.
- Verification: GitHub documentation changes only. No source build, Windows gameplay test, image equivalence test or new graphics feature.

## Graphics sprite rendering follow-up — 2026-11-01
- Report: `docs/audits/2026-11-01_sprite_rendering_followup.md`.
- Verified: C display and sprite remap paths; 16-colour and MCGA rendering paths must not be conflated.
- Unresolved: the original assembly sprite-table entry layout and rendering consumer; oversized assembly source line queries returned empty data.
- Status: source documentation only; no new decoded art, engine build, Windows visual test or game-code modification.
- Next: source extract and private sprite-table record cross-check. Other audit branches unchanged.

## Graphics block-face cross-check — 2026-11-01
- Report: `docs/audits/2026-11-01_graphics_block_face_crosscheck.md` (commit `36ea7097aefa9be777a13d6cc5ea1d4862df5db5`).
- Scope: independently reconcile existing map-column rebase, block face index selection, descriptor component and zero-offset sampling findings.
- Confirmed as existing evidence: 12,288 map pointers; selected block IDs index 24-byte records, with draw-path component fields +0/+4/+12/+16/+20. Flag-test semantics do NOT establish illumination or geometry classifications.
- Not performed: new binary decoding, compilation, Windows visual/performance testing or any engine change.
- Next: original sprite-table assembly consumers; then private resource-to-pixel validation and scoped night-lighting feasibility.
