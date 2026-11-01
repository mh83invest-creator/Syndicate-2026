# Syndicate 2026 — main display-flag state transitions

2026-11-01 source audit. No executable or branch modifications.

Directly inspected engine/src/main.c and full engine/src/syndre.sx.

The Windows entry point sets DrawFlags to high-resolution mode before reading command-line arguments; -L selects DrwF_Unkn04 and -H selects DrwF_ScreenVres16. The tactical loader checks equality with 2 or 4, not merely bit presence.

In assembly select, menu preparation (54239–54250) saves DrawFlags in OldDrawFlags and temporarily sets DrawFlags to 1 for AppScreenSetup. A selection exit (54838–54856) releases menu-specific resources and restores DrawFlags from OldDrawFlags. level_finished (55019–55023) also saves DrawFlags and sets 1 for the post-mission screen. Thus DrawFlags can be a temporary UI mode and must not be interpreted solely from its value during menus.

An assembly reset path at 31000–31022 unconditionally sets DrawFlags to 2, current level and map number to 1, and several global defaults. The exact calling context of that reset is not fully traced, so its impact on command-line mode persistence is uncertain.

The main tactical loop (199–260) executes selection, mission reset, player initialization, actions, AI, movement, campaign processing, objective evaluation, screen swap and scrolling. The mode-4 map reload question is not yet resolved by this state trace; need connect select exit, briefing read, command-line state and tactical first tick.

Preserve menu save/restore semantics, mode-dependent asset descriptors and map buffer lifetime. No runtime test, profiling, engine edit or other workstream modification.
