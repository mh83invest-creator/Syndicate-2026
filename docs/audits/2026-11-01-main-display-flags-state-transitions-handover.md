# Handover: display flags and mission transitions

2026-11-01. New audit docs/audits/2026-11-01-main-display-flags-state-transitions.md, commit bdcf2aa799196e0106e6236cf3b74fefa72fbff8.

Verified source: entry point sets initial graphics mode, selectable via -L/-H. Assembly menu setup saves flags and temporarily uses mode 1, then restores OldDrawFlags on menu exit. Level finish also changes mode for results display. Separate reset path forces mode 2; its reachability remains unresolved. Tactical mode-4 map freshness is not yet proven. Preserve v0.7, Graphics and Mission Builder workstreams. No code modifications, builds, profiling or playtesting.
