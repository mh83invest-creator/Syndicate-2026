# Graphics audit — player colour selection limits

Date: 2026-11-01. Documentation only. Branch: audit/graphics-assets-2026-11-01.
Original source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Verified
- At lines 28507–28529, network-player initialisation writes successive unsigned byte slot indices into data_5e4ab, with loop continuation dependent on Network__NumberOfSlots. It does not clamp the index to the static palette lookup length in this excerpt.
- At 10607–10643, a configuration/selection path compares against existing slot colours and searches other slots with %cl from zero while %cl < 8. That is an eight-slot search *in this routine only*, not proof of the maximum game slot count everywhere.
- At 84706–84709, player_colour_for_map contains ten bytes: 06,07,0E,03,0B,0C,0D,0F,06,00. The first eight are distinct; the ninth repeats the first, and the tenth is zero. Their meaning as colours depends on the active palette.
- Earlier inspected map rendering paths use data_5e4ab to index this lookup. The value influences map drawing, while a separate agent rendering path uses it in sprite variant selection.

## Limits
No universal proof that all runtime colour indices fall in 0..9, nor that the final two values represent sentinels. No RGB mapping established; a colour table byte is not an RGB triple. Do not automatically clamp, remap or replace colours based on this trace.

## Verification
PASS: direct original assembly read; exact data constants and loop comparisons reviewed.
NOT RUN: retail palette data lookup, native compilation, Windows gameplay, screenshot comparison, performance benchmark.
No game code or other branches modified. Next: verify palette selection and active indexed RGB mappings with private assets when available.
