# Syndicate 2026 — Player-state provenance of agent sprite variants

Date: 2026-11-01 (Australia/Brisbane)
Branch: audit/graphics-assets-2026-11-01
Status: focused original assembly source audit complete. No graphics byte analysis or Windows testing.

## Evidence
Original assembly: engine/src/syndre.sx, blob c35e22621a93d1e07c44d40de8867446ff7c64bf.
- data_5e4ab occurs at line 85352 as a byte located next to data_5e4ac.
- Lines 28508-28529 initialise data_5e4ab for network slots, writing %bl into the slot-dependent indexed location in an iteration capped by Network__NumberOfSlots. The loop's actual progression is to be understood before claiming a complete range invariant.
- Lines 25978-25985 write the same initial byte into data_5e4ac and data_5e4ab in a player-initialisation path.
- Lines 10573-10576 copy values from menu/network state into both byte fields; lines 10634-10643 compare and conditionally replace the first across slot-related records.
- Lines 16735-16756 read data_5e4ab from a slot-indexed record and use its value as index into player_colour_for_map. Another path at lines 31756-31779 does likewise when drawing a map area.
- Lines 19195-19238 use a person pointer relative to level__People, divide by the person 0x5c stride and further group the result before indexing data_5e4ab; then use the byte to select a different member of an eight-entry sprite descriptor family. Lines 18353-18379 corroborate alternative sprite selection at a separate draw call.

## Interpretation and limits
This is evidence that the byte is part of a player/slot-associated graphical identity or colour selection, not merely a random per-animation byte. It is used for player_colour_for_map and for the agent sprite variant. However, this source trace does NOT prove that it represents team colour, costume, weapon type, faction, or player ownership in every mode.
The index arithmetic in multiple slot paths is complex and needs a separate full trace before asserting array bounds or safe values. Neither original graphics palette indices nor actual colour values were checked here.
Do not bake inferred cosmetic labels into a new sprite viewer or night-lighting implementation.

## Verification
PASS: direct full-assembly source read; references to data_5e4ab enumerated and named caller paths inspected.
NOT RUN: private resource analysis, pixel colour check, Windows run, compilation, benchmark.
Changes: only this graphics-branch report and its continuity index; no code, original assets or other branch modifications.
Next: correlate player_colour_for_map table entries and original sprite variants; inspect all known data_5e4ab writers and mode-specific state values.
