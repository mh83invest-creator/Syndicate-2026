# Graphics audit — player map-colour lookup table

Date: 2026-11-01 (Australia/Brisbane)
Branch: audit/graphics-assets-2026-11-01
Status: original assembly static-data/source review. No runtime palette or private asset validation.

## Source evidence
Original engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.
- 84706–84709 defines the literal player_colour_for_map bytes: 06, 07, 0e, 03, 0b, 0c, 0d, 0f, 06, 00 (hex). Exactly ten bytes appear between the symbol and the following data_5b474 label.
- 16750–16756: player/slot byte from data_5e4ab indexes this lookup; its result is stored into a prepared colour-related buffer.
- 31767–31779, 32256–32263: data_5e4ab also indexes this table to pass colour byte into map drawing calls.
- 32710–32716, 32775–32782: analogous lookup is conditioned by a state bit; in the latter branch the selected colour can be overwritten with zero.
- 11344–11356: a further renderer path uses the lookup and writes into WScreen; this caller takes an independent index, not conclusively data_5e4ab.
- Previous source trace 19195–19243: data_5e4ab influences sprite descriptor variant selection. The palette lookup is related to, but not identical to, sprite graphic substitution.

## Inference limits
The source proves a ten-byte static lookup, not ten available players or valid indexes. A ten-byte lookup by an unchecked byte can be out-of-bounds if unexpected state values occur; evidence does not show such values arise in retail gameplay. Hex values must not be labelled as final RGB colours without applying the relevant in-game palette; they are 8-bit indexed colour values or lookup outputs in the inspected paths.
The final 00 entry may have special meaning, but no interpretation is established.

## Verification
PASS: full original assembly GitHub blob fetched; constant data and all matching symbol references enumerated.
NOT RUN: private RGB palette cross-check, graphical screenshot validation, compiled game, Windows runtime, performance benchmarking.
No engine, original game assets, CI or other branch changes. Next: trace expected player-state indices and palette resolution across modes.
