# Syndicate 2026 — Fade caller survey: engine/src and selected general files

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics source audit only; no source modifications.

## Search performed
- Read all 17 C files under `engine/src/` identified by GitHub recursive tree for main and fetched each directly from the graphics audit branch: `display.c`, `dos.c`, `game.c`, `game_data.c`, `guitext.c`, `keyboard.c`, `main.c`, `mouse.c`, `osunix.c`, `oswindws.c`, `player.c`, `research.c`, `sound.c`, `timer.c`, `util.c`, `weapon.c`, `wrcountry.c`. Searched every line for `LbPaletteFade\s*\(`.
- **No direct `LbPaletteFade(...)` calls found in those 17 application engine source files.**
- Additionally inspected 12 selected files in `engine/bflibrary/src/general/`: `ganywnd.c`, `gbox.c`, `gflicply.c`, `gflicrec.c`, `gpalcrss.c`, `gpalette.c`, `gscreen.c`, `gdata.c`, `gutility.c`, `gini.c`, `gmouse.c`, `gsprite.c`. Only matching occurrence was the function **definition** in `gpalette.c:38`.
- This was a deliberately bounded search after prior tool-call cap; **NOT** a complete repository-wide symbol/call graph or scan of every general-library and assembly source. Other call sites, function pointers, preprocessor aliases or generated sources have not been ruled out.

## Interpretation
`engine/bflibrary/src/general/gpalette.c:38-106` still has divisions by `fade_to` without a local zero check and ignores palette-related return codes. The observed application C files do not directly call that routine. **No evidence in this search establishes that Syndicate gameplay passes `fade_to=0` or invokes the function at all.** Accordingly do not assert a gameplay divide-by-zero crash or propose a change solely on this result.

## Test ledger
PASS: GitHub recursive tree for file names; exact-branch fetch and scan of 17 application .c and 12 selected shared-library .c files; only shared-library function definition matched.
NOT DONE: full repository scan, runtime fade test, Windows SDL backend verification, executable or screenshot testing.
Changed: this report plus graphics audit continuity index only. Protected v0.7 and other audit/development branches untouched.
NEXT: scan remaining shared-library general sources in bounded batches and check possible alternate wrappers or direct palette-update paths; do not treat unverified reachability as a defect.
