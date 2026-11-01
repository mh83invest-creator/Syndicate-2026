# Graphics audit — block face and sprite resource boundary cross-check

Date: 2026-11-01. Branch: audit/graphics-assets-2026-11-01.
Status: source-to-existing-audit synthesis complete; rendering semantics not verified in Windows.

## Evidence examined
- docs/audits/2026-11-01_block_faces_and_map_rebase.md
- docs/audits/2026-11-01_map_block_render_dependency_trace.md
- docs/audits/2026-11-01_private_block_descriptor_sample_validation.md
- docs/audits/2026-11-01_private_block_flag_reconstruction.md
- docs/audits/2026-11-01_primitive_graphics_resource_sizes.md
- docs/audits/2026-11-01_sprite_rendering_followup.md

## Cross-check results
1. Original map columns are accessed through 12,288 rebased 32-bit entries. The loader mutates offsets into process pointers. Offline parsers must work from pre-rebased data or explicitly undo the transformation.
2. The draw paths use map-column bytes +10 and +11, choose a 24-byte block descriptor, and access component offsets +0,+4,+12,+16,+20 depending on branch. The sixth descriptor field at +8 was inspected by the block-flag reconstruction, but this does NOT prove a draw-call consumer for it.
3. The block-flag reconstruction samples six component offsets for each of 256 descriptor IDs, with 16 samples per component at a 20-byte step. Its resulting bits 1 and 4 are indicators from that specific sampled-zero predicate, not material, lighting or occlusion classifications.
4. Zero component offsets occur in the prior inspection. In the audited initializer they may still be sampled against the start of the decoded allocation. A future graphics inspector must distinguish zero-as-absent from source's actual rebased sampling behaviour and must not silently call every zero-offset component transparent.
5. Map/block faces and mobile sprites are distinct resource families. Decoded hblk01.dat and hspr-0.dat lengths establish resource integrity, not visual object labels. The renderer's face-selection logic does not establish a direct index from road signs, windows or vehicles to emitter types.
6. Mission graphics night prototypes cannot be specified as merely replacing one shared palette, or recolouring the entire final framebuffer: shared palette IDs, HUD/pointer overlays and phase-specific screen buffers remain verified constraints.

## Next experiment and acceptance gates
- Derive a non-destructive diagnostic table for map tile coordinates, map-column bytes +10/+11, descriptor ID, selected component offset and component byte span, including zero offsets and relative pointer bounds.
- Independently identify sprite-table record stride and animation-index consumers in original assembly before any sprite identification.
- Cross-check selected source-derived entries with private decoded assets and private screenshots. Never commit original commercial art.
- Run actual Windows original mission/menus to verify draw order, palette evolution, UI overlays and performance before approving lighting.

## Verification and changes
Existing dated reports read and reconciled; arithmetic and field-offset relationships compared across the reports. This stage did NOT independently decode private assets or execute original assembly.
No engine changes, builds, runtime screenshots, performance measurements or pixel equivalence tests.
This report is documentation only; the protected gameplay baseline and other workstream branches are unchanged.
