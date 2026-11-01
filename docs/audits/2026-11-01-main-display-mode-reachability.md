# Main Systems audit: reachability of alternate map loading mode

2026-11-01. Source-only. No changes to game or other audits.

Directly inspected engine/src/main.c (blob e9621c6594173569e78366553ddc2de820e407f8) and engine/src/syndre.sx (blob c35e22621a93d1e07c44d40de8867446ff7c64bf).

Main initializes DrawFlags to DrwF_ScreenVres16 at line 273. Command-line processing accepts -L, setting DrwF_Unkn04 at line 211, and -H, restoring DrwF_ScreenVres16 at line 215. At lines 311–319 it loads distinct initial resources for these two flags, including load_files_mcga for the alternate mode. Therefore mode 4 is explicitly selectable in the current C entry point; it is not necessarily unreachable legacy code.

The assembly tactical load_map_level at 27845–27933 uses DrawFlags equality checks for 2 and 4: mode 2 loads mission_load_files (which includes map_buf), whereas mode 4 invokes an empty list and proceeds into init_level_data, which relocates map_buf. The alternate path's dependence on prior map preparation is real at source level. However, the command-line path alone does not establish that a player can reach tactical gameplay with uninitialized or previously relocated map_buf; intervening initialization, selection, and briefing still matter.

Additional assembly assignments of DrawFlags around 31010, 54249, 54850 and 55022 mean command-line selection is not the only potential write. Those transitions should be traced before any repair proposal. Do not claim a runtime crash or confirmed double relocation.

Potential tests remain deferred until all audits conclude. Protected v0.7 and Graphics/Mission Builder branches unchanged.
