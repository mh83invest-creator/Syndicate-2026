# Syndicate 2026 — Map-to-Block Render Dependency Trace

Date: 2026-11-01 (Australia/Brisbane)
Stage: focused original assembly source audit COMPLETE; overall graphics feasibility and runtime testing INCOMPLETE.
Branch: feature/v0.9-visual-showcase
Protected baseline: main v0.7 at a22b0b9580a9710b4e693e88854351835ef1ee40
Source: engine/src/syndre.sx, original assembly blob c35e22621a93d1e07c44d40de8867446ff7c64bf.
Related: docs/audits/2026-11-01_rnc_full_decode_reconciliation.md and docs/audits/2026-11-01_primitive_graphics_resource_sizes.md.
Work performed: direct GitHub blob reads and source tracing; no gameplay source, resources, CI or engine code changed.

## Evidence-led findings
1. Map column lookup and drawing: syndre.sx:71385–71425 calculates a column index using half tile coordinates, a 128-column multiplier and four-byte pointer-table entries, then branches by tile-coordinate parity. A high-resolution draw branch multiplies a supplied argument by 0x500 and adjusts another argument by four. These are implementation details, not proven pixel pitch or world-to-screen projection constants.
2. Block descriptor stride: syndre.sx:71638–71649 loads a byte at offset +0x0b of a decoded map-column target, rejects values <=4, and multiplies an accepted index by **0x18 (24 bytes)** before adding h_blocks. It then reads the descriptor field at offset +0x0c as a *relative* block-data pointer and adds h_blocks again. This establishes an original render-time 24-byte indexed block-descriptor access, not a complete record layout.
3. The nearby second path at 71660–71669 uses the same 24-byte indexing but dereferences descriptor field +0x04, indicating multiple block-part pointers/branches. Both paths check the field against zero before rebasing.
4. An earlier drawing path at 66740–66763 similarly checks decoded column pointers, reads +0x0b, multiplies by 24, obtains field +0x0c and rebases against h_blocks. This is a useful independent code-region cross-check. h_blocks is referred to extensively in original assembly and therefore must not be assumed to be a mere decorative palette.
5. The block drawing logic bounds some map column-table-relative offsets against 0xc000 (49,152 bytes), matching 12,288 four-byte entries. This is a runtime table-offset guard in the inspected paths but does not prove every relative block-data pointer is range-checked.
6. The original hblk01.dat previously decoded to 321,664 bytes with matching RNC unpacked CRC, and col01.dat to 256 bytes. Those lengths are independent asset facts, not yet classification of roads, windows, neon or building faces.

## Inferences, deliberately not asserted as facts
- The byte at decoded column +11 plausibly identifies a block/type/index used for drawing; the name and full semantics require asset-level cross-check.
- 24-byte h_blocks records could contain multiple relative offsets, but only fields +4 and +12 are demonstrated by these inspected code paths. Do not invent the remaining field names.
- The existence of block and map references makes static object classification *more tractable*, but no world-space light-source inventory, correct occlusion or localized night-lighting insertion point has been demonstrated.

## Verification ledger
PASS: original assembly blob source read at the cited ranges; two render-region cross-checks; constant stride/offset arithmetic.
NOT RUN: C decoder executable, actual world-to-screen projection output, Windows render/camera/focus gameplay, night overlay visuals, performance profile.
OPEN: complete hblk record layout, resolved map-column +11 meaning, referenced graphics data bounds, object-to-sprite relationships, HUD/cursor masks.

## Next action
Use the previously verified decoded private hblk01.dat and map samples to correlate descriptor indices and fields against in-bounds relative target addresses. Inspect the remaining draw routines and map column structure. Only after source-and-asset proof, propose a *separately authorised* localized lighting test plan; do not modify original code or publish retail assets in this audit.

Rollback: documentation-only on development branch. Stable main unchanged.
