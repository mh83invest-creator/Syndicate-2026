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

## Sprite pointer setup source audit — 2026-11-01
- Report: `docs/audits/2026-11-01_sprite_pointer_setup_source.md`; report commit `7ba11371c089ea6641516f0e9539510b813e084f`.
- Confirmed from Bullfrog library C source: LbSpriteSetup performs offset-to-pointer adjustment for TbSprite.Data; reset can reverse that conversion. Sprite drawing preparation checks dimensions, encoding uses width/height and the data pointer.
- Crucial limit: no proof yet that private `hspr-0.tab` uses TbSprite or any particular entry stride. No direct assembly consumer analysis or newly decoded sprite data was performed.
- Verification: source reads and report commit. No executable build, game test, or modification to gameplay. Next inspect original assembly consumer and table layout.

## Original sprite descriptor assembly trace — 2026-11-01
- Report: `docs/audits/2026-11-01_original_sprite_table_assembly_trace.md`; commit `3b2a01f1a24ab365d8073d82f5be2f9b4e0ca445`.
- Full original `syndre.sx` blob successfully fetched, resolving the previous large-file access blocker. `init_level_data` calls `ac_LbSpriteSetup` with `h_sprites`, end pointer and `h_sprites_data`.
- Direct drawing routines read sprite descriptor 32-bit pointer at +0, dimensions at +4/+5; 10,812-byte decoded sprite table therefore has a 1,802-entry *candidate* six-byte interpretation, pending private data checking.
- Selected animation loops use 10-byte element indexing and sprite-table byte offsets. Exact valid element and sprite counts, rendering equivalence and lighting compatibility are unverified.
- Tests: assembly source read and arithmetic only; no new private decoding, compilation or Windows gameplay. Next: CRC-checked private 6-byte/10-byte record validation.

## Packed sprite descriptor ABI and asset validation gate — 2026-11-01
- Report: `docs/audits/2026-11-01_sprite_descriptor_abi_validation_gate.md` (commit `2b347433b42b194edceeb09bc255e93586693608`).
- Source cross-check: packed `TbSprite` in `engine/bflibrary/include/bfsprite.h` with pointer at +0 and, when `SPRITE_FORMAT_V2` is not selected, byte dimensions at +4/+5; direct assembly draw fields and `ac_LbSpriteSetup` align with this layout.
- 10,812 bytes / 6 = 1,802 potential entries. Asset-level record count, pointer ranges, empty entries and animation reference integrity remain **unverified** because private retail archives are not available in this session.
- Verification: GitHub header and assembly source read; no new data decode, build or Windows game test. Next: private archive availability and actual six-byte descriptor bounds validation.

## Animation frame/element source trace — 2026-11-01
- Report: `docs/audits/2026-11-01_animation_frame_element_source_trace.md`; commit `2024f0a681c1179a6c5c49d4307411f12226917c`.
- Source confirmed: frame lookup uses 8-byte stride; frame +0 references ten-byte animation elements; frame +4 participates in sound control; element +0 addresses a sprite by table-relative byte offset, +2/+4 position, +6 drawing parameter and +8 next element reference.
- Decoded file sizes imply **candidate**, not verified active, capacities: hfra-0.ani 8,948 frame slots; hele-0.ani 10,486 element slots; hspr-0.tab 1,802 sprite entries. hsta-0.ani semantics remain open.
- Tests: original assembly blob inspection and divisibility calculations only; no private data record validation, Windows rendering or game code changes. Next trace starts_ani and validate private animation link chains.

## Animation starts table audit — 2026-11-01
- Report: `docs/audits/2026-11-01_animation_starts_source_trace.md`; commit `9bc60d769ee93f2a7d437ed1e6855d8d06616d4e`.
- Source confirms `starts_ani` is addressed with 16-bit word entries. Ordinary scaled indices and fixed offsets, including 0x4b0 and 0xf3e, are used in distinct animation and drawing paths.
- Existing 3,940-byte decoded hsta-0.ani length is compatible with 1,970 candidate word entries, not 1,970 validated active sequences. Some offsets serve separate logical groups; their semantics and bounds require private asset validation.
- Tests: original assembly source read and size arithmetic only. No game-code modifications, private asset decode, build or Windows runtime tests. Next: inspect starts/frame/element cross-reference and termination rules.

## Animation progression and termination — 2026-11-01
- Report: `docs/audits/2026-11-01_animation_advancement_termination.md`; report commit `e7b60c330e21b0f9613d46617455fa2e8ab0ff75`.
- Original assembly source shows `frames[current]+6` as a next-frame state in examined routines, with `starts_ani` reset/default values. Element +8 provides subsequent ten-byte element index. Both types of transitions need private data validation for end bounds, loops, invalid links and sentinel behaviour.
- Verification: original assembly read. No new data decoding, compilation or Windows tests. No game files or other workstreams changed.

## Animation chain bounds / cycle audit — 2026-11-01
- Report: `docs/audits/2026-11-01_animation_link_bounds_cycle_audit.md`, commit `35e623072f3b80b8b28e286457f9914e5b05c6f7`.
- Inspected original assembly confirms zero-index element chaining terminates at the animation-elements base pointer, while positive element indices continue without a visible end-bound check or visited-node limit in the inspected loops. This identifies a conditional risk only, NOT corrupted source assets or a reproduced runtime failure.
- Frame progression may take frame+6 or reset from starts_ani depending on state; no universal end-frame convention has been proven.
- Next: private CRC-checked hfra/hele graph traversal with bounds, cycle checks and comparison to Windows playback. No code or gameplay changes; no runtime validation.

## Agent-specific animation sprite selection — 2026-11-01
- Report: `docs/audits/2026-11-01_agent_animation_sprite_selection.md`, commit `f35f1a0ac59a69897b869738c267912b7fcccdb2`.
- The original `draw_agent_element` path may replace an element's sprite offset using agent state and an alternate descriptor selection before calling `DrawASpriteHires16_A`. An offline viewer cannot assume the raw element +0 sprite is always the displayed agent sprite.
- Inspected links and descriptor selections only test against their respective *base* pointers, not observed end bounds. Conditional risk only; no malformed asset or crash established.
- Verification: source-only assembly inspection, no new asset decoding, build, Windows testing or gameplay modifications. Next trace alternate selection state and validate private sprite/animation record references.
