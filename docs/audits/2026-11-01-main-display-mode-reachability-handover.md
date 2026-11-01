# Main audit handover: display mode reachability

Source audit docs/audits/2026-11-01-main-display-mode-reachability.md, commit 55e422971963dd7dbe388ba07fae45840ad392fd.

main.c defaults DrawFlags to high-resolution mode; -L explicitly selects alternate MCGA mode, and -H reselects high-resolution mode. Thus mode 4 is selectable, not necessarily dead code. Tactical mode 4 skips the mission_load_files map reload but initializes the existing map_buf. Intermediate briefing and DrawFlags transitions remain unverified and no runtime bug is claimed. No game code, tests or other audit branches modified.
