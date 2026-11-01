# Palette setting calls and fade implementation audit
Date: 2026-11-01 (Australia/Brisbane). Graphics branch documentation only.
Sources: engine/src/syndre.sx original blob c35e22621a93d1e07c44d40de8867446ff7c64bf; engine/bflibrary/src/general/gpalette.c.

## Directly verified
- syndre.sx:27779-27794 loads mission palette RNC bytes into GraphicsPalette, clears WScreen, then pushes WScreen to ac_LbPaletteSet. This call really does pass WScreen; it should not be silently rewritten in a handover to say GraphicsPalette. The reason is unknown.
- syndre.sx:244-249 restores the palette from GraphicsPalette in one branch of the main processing loop.
- syndre.sx:54973-54980 passes m_select_pal to ac_LbPaletteSet during a menu selection state change.
- syndre.sx:62012-62025 includes conditional low-level VGA palette port writes and the alternate call to ac_LbPaletteSet with the same pointer.
- gpalette.c:38-105 defines LbPaletteFade: interpolates each entry across PALETTE_8b_SIZE, reads the current palette with LbPaletteGet and writes via LbPaletteSet. The fade has two paths: a synchronous path with wait/sleep and screen swap, and a stepping path with palette updates. gpalette.c:108-112 resets fade_started.
- Search of original assembly blob found four direct ac_LbPaletteSet calls at 248, 27793, 54977, 62023. This is not an exhaustive interprocedural call graph and does not prove all palette effects.

## Implications / open issues
Palette updates are not solely a one-off mission load; a main-loop branch can restore GraphicsPalette and menu state can set another palette. A mission lighting implementation must preserve fades/restore semantics and avoid changing shared palette resources.
The call passing WScreen is a source fact, not an established bug; need to inspect symbol thunk/implementation and real Windows behaviour before asserting it is wrong.
No actual RGB values or palette images tested.

## Verification
PASS: direct reads of assembly and gpalette.c and exact caller inspection.
NOT RUN: private asset decode, compiled Windows runtime, screenshots, fade timing or gameplay performance tests.
No engine/gameplay, retail art, or other branches changed.
