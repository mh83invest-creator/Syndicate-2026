# Syndicate 2026 — Original Map Rebase and Multi-face Block Lookup

Date: 2026-11-01 (Australia/Brisbane)
Status: **focused static source audit complete**; renderer/lighting and all-game audit remain incomplete.
Repository: mh83invest-creator/Syndicate-2026
Audit branch: feature/v0.9-visual-showcase
Protected playable reference: main v0.7, a22b0b9580a9710b4e693e88854351835ef1ee40
Assembly source blob: c35e22621a93d1e07c44d40de8867446ff7c64bf (engine/src/syndre.sx).
No original files, game source, workflows, binary resources, FreeSynd implementation or stable main modified.

## Verified code paths

1. **Map offset rebasing:** `syndre.sx:25694–25716`, `init_map_data`, advances the decoded map buffer by 12 bytes and overwrites exactly 0x3000 (12,288) little-endian 32-bit relative offsets with process-address pointers equal to table base plus relative offset. It stores the table pointer in `data_55358`. **Critical:** Once this mutates the loaded map, its table entries cannot be interpreted as original file-relative offsets without accounting for rebasing. A static off-line parser must inspect pre-rebased bytes, or convert them back deliberately.
2. **High-resolution horizontal draw lookup:** `syndre.sx:71385–71430`, `draw_horizontal`, halves two map coordinates, multiplies one by 128, accesses four-byte rebased column pointers and branches on coordinate parity. It also applies high-resolution argument scaling using 0x500 and 4. Do not mislabel 0x500 as a universal SDL pitch without following its consumers.
3. **Block descriptor selection is multi-face, not single-sprite:** `syndre.sx:71628–71739` checks candidate table addresses against 0xC000 bytes (48 KiB), then reads decoded map column bytes at +0x0A or +0x0B; accepted block IDs above 4 are multiplied by 0x18 (24-byte stride) and used to index `h_blocks`. Candidate block component offsets include +0, +4, +12, +16 and +20 in distinct branches. Each nonzero component offset is converted to an absolute address by adding `h_blocks`. The component address is recorded in `data_48ad0` for later rendering. This expands the previously documented +4/+12 field evidence.
4. **Branch-specific flags are used during selection:** After adding a component, source tests individual bits of `data_5e39c` indexed by the block ID, including 0x01, 0x02, 0x08, 0x10 and 0x20 in these inspected paths. These flags affect branching, but their semantics (visibility, transparency, cover or something else) remain **unproven**. Do not name them as light/occlusion flags.
5. **Actual source resources:** `syndre.sx:82620–82645` loads separate `mapNN.dat`, `col01.dat`, `hblk01.dat`, `hspr-0.dat` and `hspr-0.tab` into distinct pointer slots. These cannot be treated as one monolithic decoded bitmap.

## Consequences for night-lighting feasibility

The renderer chooses block faces/component fragments from map-column bytes and multiple descriptor fields. A credible per-window or per-sign lighting method needs explicit identification of the relevant geometry, the map-to-screen projection, draw order and occlusion. A simple brightness adjustment of an entire palette or a single h_blocks descriptor is not proven sufficient.

The offline private-resource CRC checks in preceding audit reports support resource integrity, **not** geometry or visible object class labels. Classifying a numeric block index as a lamp/window without visual and source cross-check would be speculation.

## Verification ledger

PASS: GitHub read of original exact assembly blob, line-by-line inspection of map rebase, horizontal draw and block component branches.
PASS: Confirmed independent resource names/pointer slots in original mission load table.
NOT RUN: compiled original assembly/native game, surface or sprite tracing on Windows, alternate-mode geometry, object classification, lighting prototype or performance benchmark.
NOT PROVEN: full 24-byte descriptor layout, number of valid block records, h_blocks pointer target extents, data_5e39c flags semantics, candidate luminous objects and occlusion.

## Next audit stage

Trace `data_48ad0` consumers and remaining component fields/branches, then correlate with actual decoded block records and sample maps. Add a read-only table/coordinate diagnostic before selecting any renderer insertion point. No engineering changes are authorised by this source audit. Stable main remains untouched.
