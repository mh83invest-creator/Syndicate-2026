# Syndicate 2026 — Menu Hitbox, Text and Strategy Timing Source Audit

**Date:** 2026-11-01 (Brisbane)
**Audit status:** Read-only static source confirmation; no asset screenshots, compile or playtest.
**Engine:** SyndicatFX retained; FreeSynd reference only. Baseline `main` v0.7, commit `a22b0b9580a9710b4e693e88854351835ef1ee40`.
**Source:** `engine/src/syndre.sx` SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf`; its source lines are 1-based, and user-facing menu graphics remain original retail files not in public GitHub.

## Exact observed menu hover region

`DrawMenuGadget` at **`syndre.sx:56074–56117`** compares the logical 640×400 mouse position to **four original assembly data values**:

| Coordinate bound | Data label and source | Byte contents (little-endian) | Decimal |
| --- | --- | --- | ---: |
| left x | `data_5b6d2`, line 84918 | `f4 01` | 500 |
| top y | `data_5b6d4`, line 84920 | `5a 01` | 346 |
| right x | `data_5b6d6`, line 84922 | `74 02` | 628 |
| bottom y | `data_5b6d8`, line 84924 | `74 01` | 372 |

The comparison at **lines 56078–56095** selects highlighted text colour index `0x166` when:
**500 < logical mouse X < 628 and 346 < logical mouse Y < 372**, otherwise colour index `0x1ef`. The comparisons are strict; precisely-on-edge coordinates are not counted as inside. These indices are passed into `DrawScreenGadget` and are **original drawing codes, not RGB/HTML colours**.

**Scope caveat:** This is one specific shared menu-gadget *hover* routine, **not a complete click-action hitbox inventory for all UI screens**. Other menu click actions and pointer state checks must be traced separately before altering coordinates.

`DrawMenuGadget` has **eight static call sites** in the assembly text (lines 1422, 10202, 12648, 21478, 30489, 31732, 33847, 51893), demonstrating that the routine is reused across briefing, configuration, map and other screens. Their exact runtime circumstances require further study.

## Menu typography is layout data, not just a replaceable image

- `DisplayScreenTitle` at lines **55935–55970** invokes `GetLineWidth` and computes a centered x position relative to an original **640 logical width**.
- `DrawScreenGadget` at **55974–56023** measures the text width and positions `new_show_text` according to original sprite font and source bounds.
- `DrawMenuGadget` at **56111–56114** uses `language` to choose `gui_strings_p258`; menu text can be language-dependent. Replacing font metrics or string lengths can change visual alignment and hit state.
- `draw_config_menu` at **10925+** dispatches through `jpt_18270` and mixes text, sprite, panel and menu-box functions. `configure_company` at **10069+** also loads original screen data and uses menu controls; graphics and input are not cleanly isolated.

**Consequence:** For v0.9A, keep existing action/hover bounds as contractual until a visual layout change can be paired with exact coordinate transformation and tested in each mode/language. Never assume a decorative border is independent of cursor state.

## Strategic map has update and campaign-state responsibilities

The strategic map's `map_selection` loop (starting **line 31709**) is not a purely static UI preview:

- Lines **31730–31738** call `LbCopyScreenFromBackMCGA`, `DrawMenuGadget`, and `process_day`.
- Subsequent source around **31749–31790** iterates prospective mission countries and calls `_draw_map_area` to change displayed map regions/colours.
- `ac_game_update_menu` occurs at assembly line **31936**, preserving the previously implemented 30 FPS target.
- `process_day` itself starts at **line 11450** and includes research, income, map-state and other game-time/campaign effects. Exact triggering conditions in the strategic loop still require a full branch analysis.

**Consequence:** Blocking, skipping or reordering the strategic loop for graphical effects risks altering campaign progression, even when no mission gameplay is active. Visual updates must not modify `process_day` call count, menu input dispatch or the strategic clock.

## Where mouse coordinates come from

`engine/src/mouse.c:31–45` maps SDL mouse positions into original logical coordinates: in 640×400 Vres16 it subtracts 40 from Y and clamps; in the alternative 320×200 mode it doubles coordinates. The Bullfrog SDL2 mouse backend `engine/bflibrary/src/x86-win-sdl2/smouse.cpp:260–318` also includes native scaling and optional auto-reset behaviour. The original menu code uses the **post-transform** position, not raw desktop pixels.

**Acceptance tests when an executable/data becomes available:** position exactly at each of the four boundaries, one pixel inside/outside, hover and click behaviour, both display modes, letterboxed/windowed/fullscreen states, alternate language strings, focus/Alt-Tab, and return paths between company, map and briefing. Test actual game actions, not only screenshots.

## Current feasible conclusion

**Confirmed:** There are concrete original menu text, hover, layout and render routines with a known 128×26 logical hover rectangle, and there are campaign-state updates in the strategy UI loop.

**Not established:** full per-screen clickable hitbox geometry, all action handlers, legality of replacing assets, full localisation variation, or a safe modern rendering API. No graphical facelift has been built or demonstrated.

**Implementation state:** no code or workflow changes. v0.7 unchanged. v0.9A feature still not implemented beyond the earlier native OS-window-title change. Changes require explicit scope, a separate build and actual Windows regression tests.
