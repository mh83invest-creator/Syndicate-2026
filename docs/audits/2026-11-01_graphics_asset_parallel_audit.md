# Syndicate 2026 — Graphics Asset Audit: Independent Workstream Handover

Date: 2026-11-01 (Australia/Brisbane)
Status: INITIAL GRAPHICS AUDIT STAGE COMPLETE — cross-reference of existing verified asset reports. Full sprite/tile visual audit remains IN PROGRESS.
Repository: mh83invest-creator/Syndicate-2026
Branch: audit/graphics-assets-2026-11-01
Branch point: d42db5258f2786ea26a8e6e3ee235e09fa404cf6 (feature/v0.9-visual-showcase)
Protected baseline: main v0.7 at a22b0b9580a9710b4e693e88854351835ef1ee40

## Purpose and cross-team boundaries
Run original game graphics/asset investigation separately from core engine/forensic audit and feature/mission-builder-audit. Do not edit engine/gameplay/mission-builder code, modify commercial retail assets, or merge either workstream without review. Share factual interface discoveries via this document and a subsequent reviewed pull request. Parallel work does not mean an autonomous background process; continuation occurs during active sessions.

## Existing evidence independently reviewed in this stage
1. docs/audits/2026-11-01_primitive_graphics_resource_sizes.md
2. docs/audits/2026-11-01_private_menu_frame_visual_review.md
3. docs/SOURCE_AUDIT_PALETTE_GROUPS_AND_BUILD_2026-11-01.md
4. docs/releases/v0.9.md

## Findings grounded in existing validated checks
- Original RNC1 resource audit reported 143/143 compressed resources decoding with expected length and CRC using an offline experimental decoder. This is not equivalent to Windows-engine image validation.
- Sample decoded resources: hblk01.dat 321,664 bytes; hspr-0.dat 355,356 bytes; hspr-0.tab 10,812 bytes; hele-0.ani 104,860 bytes; hfra-0.ani 71,584 bytes; hsta-0.ani 3,940 bytes. Record layouts and semantic sprite identity are not established from lengths alone.
- Original hpal04.dat and hpal05.dat both decode to 768 bytes. The source's level_palettes table repeats palette IDs 1–5, meaning palette files are shared across multiple levels; replacing one in place is NOT a safe way to make a particular mission night-time.
- Original menu first frames for mtitle.dat, mselect.dat, and mbrief.dat have been reconstructed as indexed 320x200 artwork and privately inspected. They are not in-game captures. Original artwork cannot be committed to public repository.
- The mission renderer is an original planar/SDL pipeline with phase-dependent WScreen, VScreen, UScreen and VGABuffer roles; no existing dynamic streetlight, headlight, window light, or neon emission layer has been demonstrated.
- LbSpriteDrawRemap exists for 8-bit Bullfrog sprites but is not proven to operate directly in original four-plane mission drawing. Avoid assuming simple sprite recolouring works in tactical missions.
- The only preexisting v0.9 source implementation noted in the verified roadmap is an uncompiled native window title string; no graphical improvements have passed Windows gameplay verification.

## Graphics audit matrix (state at branch creation)
| Area | Known | Still to audit |
| --- | --- | --- |
| Buildings/terrain | hblk data decoded; original face-offset source trace documented | Identify actual tile/face imagery, transparency, orientation, palette indices, object semantics |
| Sprites/vehicles/pedestrians | hspr and animation assets decode; sprite table available | Determine exact sprite index/record structure, render layering, animation sequencing and classification |
| Explosions | Existing effects source and performance audit available | Crosswalk effect frames to sprite records, sprite overdraw and FPS/performance sample |
| Menus | Three first frames visually reconstructed; complete original SS2 sequence reported decoded | Text/cursor overlay sequencing, palette transitions, hitbox alignment and Windows capture |
| Palettes | Five shared level-palette identifiers demonstrated; 768-byte resource samples | Identify actual colours/roles, mission-specific palette use and safe isolated night prototype |
| Lighting | No native emission/occlusion implementation demonstrated | Screen buffer hooks, palette preserving HUD colours, geometry and light masking feasibility |

## Next discrete audit steps
1. Trace hspr-0.tab record stride, sprite entry counts, drawing consumers and boundary checks in engine/src/syndre.sx.
2. Trace tile/block art decode and palette mapping, identifying precisely how rendered street, wall, roof and object faces are selected.
3. Trace effect animation file records and their draw calls, distinguishing asset complexity from actual explosion CPU cost.
4. Correlate original menu animation colours/palette changes with runtime overlay order and click regions.
5. Publish a discrete dated findings file per completed stage and update the graphics audit index. Share any interface change proposal with the core/mission-builder branches, but do not implement without approved scope.

## Verification and honest limitations
PASS: GitHub branch established from named exact commit; four existing technical source reports read and correlated; original validated numerical results cited above.
NOT RUN: fresh private game asset extraction in this session; new binary or pixel parsing; compiler, automated game tests, Windows display, runtime FPS, original/comparative screenshots; lighting or sprite modifications.
FILES CHANGED: this audit handover plus branch-local graphics index only (documentation). No game code or assets.
ROLLBACK: delete this independent documentation branch; protected main and other workstream branches unaffected.
