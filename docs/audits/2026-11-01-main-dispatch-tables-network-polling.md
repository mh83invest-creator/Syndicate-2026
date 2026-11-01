# Source audit: objective and AI dispatch tables

Syndicate 2026, 2026-11-01. Read directly from engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

The first objective table has 17 entries, of which indices 4, 6–9 and 12–13 point directly to common exit jump_10d3d. A second 17-entry table has specialised handlers only at indices 1 and 5. check_end_level scans eight objective records before calling level_complete on eligible outcomes. level_complete and level_failed set distinct state bits and block duplicate termination.

The AI turn state dispatch has 13 entries with defaults for 3, 5 and 7. A secondary table has eight entries, all sharing one handler except index 3. These are literal branch maps, not decoded behavioural semantics.

The per-frame multiplayer exchange retries nonpositive net_receive results. Lower-level net_send and net_receive poll an NCB completion byte until it changes. __NETBios__ submits through ac_dos_int386x; the port implementation and OS completion semantics remain unverified. This is a possible waiting hazard, not a demonstrated runtime hang.

No changes to program, workflows or parallel branches; no build or gameplay test. Further source and runtime verification required.
