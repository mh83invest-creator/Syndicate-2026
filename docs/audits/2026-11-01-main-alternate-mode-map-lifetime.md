# Main Systems: alternate display mode map lifetime

Date 2026-11-01. Source-only audit, no game or workflow edits.

Evidence: engine/src/syndre.sx assembly blob c35e22621a93d1e07c44d40de8867446ff7c64bf, lines 27845–27933 and 82620–82677; engine/bflibrary/src/general/gdata.c blob 497816319c0e438076e90b7166e8677f41657138.

The tactical loader's DrawFlags == 2 branch loads mission_load_files, whose descriptor includes map_buf. LbDataLoadAll frees and reallocates that descriptor on reload, so a successful fresh read replaces the briefing buffer before init_level_data relocates the map pointers.

The DrawFlags == 4 branch instead invokes LbDataLoadAll(unkn1_empty_load_files). Its assembly declaration at 82675 contains only 0x2c zero bytes, a descriptor with null Start terminating the list immediately under the library's sentinel rule. Therefore this branch does not load or replace map_buf through that list. It still reads the mission file into level__Seed and calls init_level_data, which invokes init_map_data(map_buf).

Consequently an important mode-specific dependence remains: the alternate-mode path relies on map_buf having been initialized elsewhere. A previous mission_briefing call can populate and relocate that shared buffer, but the exact path into mode 4 and its invariant are not proved. Repeated offset-to-pointer relocation is a genuine conditional hazard to evaluate, not a demonstrated defect. The assembled source alone does not prove normal v0.7 operation reaches this legacy mode.

The default mission list has a map resource entry and fixed assets; mode 4's empty list is not equivalent to mission_load_files. No error check or fallback reallocation appears in the inspected mode-4 branch.

Next source-only questions: identify which DrawFlags combinations are reachable in the current port, trace menu/briefing to tactical mode transitions, locate map_buf allocation independent of mission_load_files, and inspect LbFileReadRNC bounds. No compilation, gameplay tests, profiling or other audit-branch changes.
