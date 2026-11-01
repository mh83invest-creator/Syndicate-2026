# Syndicate 2026 — map loading callers and source data

Date: 2026-11-01. Source audit only.

Source: engine/src/syndre.sx, Git blob c35e22621a93d1e07c44d40de8867446ff7c64bf. Directly inspected lines 1240–1285, 25694–25716, 27712–27813, 27845–27933, 28480–28555, 79740–79781, 82620–82640, 83893–83905.

1. init_map_data converts 0x3000 entries starting 12 bytes into map_buf from offsets to pointers. It is called from init_level_data (27721) and separately from an earlier map-read sequence after a successful compressed read (1271–1274). These are distinct call paths. Do NOT assume that both call sites execute consecutively on the same buffer; verify the enclosing caller before alleging double relocation.
2. The earlier map-read sequence checks LbFileReadRNC against -1 and returns error 2 on failed map read before calling init_map_data. In contrast, the inspected load_map_level path itself does not visibly check the return of every compressed read or asset-load call. Different loaders may have different error contracts.
3. load_map_level can be invoked by process_action around 28498 or 28536. The action branch loads a level and invokes init_players_people, with separate logic depending on Network__NumberOfSlots. A gameplay action can therefore initiate a whole level reload and agent creation.
4. mission_load_files references an indexed map name under data/map, plus font, color, and hi-res block data. map_buf is a global pointer rather than a fixed compile-time inline array.
5. init_hires_blocks scans hi-res block records and writes availability flags. The scanned code is mode-specific preparation and must not be conflated with map offset relocation.

Cross-audit implications: Mission Builder must understand compressed file format and load/reload paths; Graphics Audit must preserve palette, block and screen resources; Main Systems must preserve player initialization and map lifetime. Static source evidence only. No game or workflow changes, compilation or gameplay testing. Full binary format and error recovery remain unverified.
