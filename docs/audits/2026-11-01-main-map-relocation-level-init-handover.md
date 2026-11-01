# Main Systems audit handover: map relocation

Date: 2026-11-01. New report docs/audits/2026-11-01-main-map-relocation-level-init-cross-system.md, commit 267df938b5891169def3c2d112b3f5f3be5d3874.

Directly inspected full syndre.sx. clear_savegame zeros tactical level memory, not demonstrated campaign slot deletion. init_map_data rewrites 12,288 map entries from relative offsets to pointers in place. init_level_data also sets up sprites, single-player AI, vehicles, screen, palette, mouse and sound status. This is an important file format and subsystem boundary for reconciliation after Graphics and Mission Builder audits. No changes to their branches.

Open evidence: map format details, pointer-size compatibility, offset validation, all binary mission schemas and malformed-file behaviour. Static audit only; no compilation, profiling or playtesting. Protected v0.7 unchanged.
