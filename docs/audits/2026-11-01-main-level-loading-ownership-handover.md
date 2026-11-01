# Main audit handover: level-loading and ownership

Date 2026-11-01. Source-only.

Audit report: docs/audits/2026-11-01-main-level-loading-ownership-audit.md; commit 0d3b06705eae21041997b5a0172da2f1bb2630a8.

Fresh direct inspection of full syndre.sx demonstrates that load_map_level links compressed level data, renderer-specific buffer layout, mission resources, sound-bank loading and init_level_data. set_structure_ends establishes active people/vehicle/object bounds with type-specific strides. initialise_player and set_network_player link network setup, copied player records, packet state and current level.

Critical interfaces to be reconciled only after separate Graphics and Mission Builder audits complete: file formats, buffer ownership, object schema, mission progression, network/player state. Outstanding unknowns include complete compressed data schema, map initialization and error behaviour. No game changes, compilation, profiling or playtesting. Preserve original v0.7 and other workstreams.
