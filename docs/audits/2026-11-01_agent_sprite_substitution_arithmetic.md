# Graphics audit — agent sprite substitution arithmetic

Date: 2026-11-01 (Australia/Brisbane)
Branch: audit/graphics-assets-2026-11-01
Status: focused original assembly trace; runtime and private-data equivalence untested.

## Verified assembly
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.
- Lines 19195–19212: draw_agent_element derives an index from the supplied people-object pointer relative to level__People using a 0x5c-byte person stride, then computes an index used to read data_5e4ab. Exact semantics of that table remain unidentified.
- Lines 19214–19228: each element's sprite byte offset at +0 is divided by six; the resulting quotient has 0x29 subtracted. Only values passing an unsigned comparison against eight enter the alternative-sprite branch. The source also loads an agent-specific byte from data_5e4ab and scales it by eight.
- Lines 19228–19238: in the alternative branch, the low three bits of the computed sprite group are combined with the agent state-derived value and a base index 0x461. The resulting index is multiplied by six to produce a sprite-table byte offset.
- Lines 19239–19243: when outside that eight-value case, the original element sprite byte offset is preserved. Both cases add h_sprites to resolve the selected descriptor.
- Lines 19244–19276: a selected sprite is drawn via DrawASpriteHires16_A, and element +8 supplies the next 10-byte linked element. Comparisons in this excerpt protect against pointers at/below table base, not demonstrated upper bounds.
- Lines 18353–18379: a separate drawing path also divides element sprite offset by six, subtracts 0x29, tests a range of eight, then computes an alternative six-byte descriptor offset; this corroborates variant substitution outside draw_agent_element.

## Consequence
The source proves a specialised substitution for an eight-entry family of base sprite IDs, with an agent-specific state byte changing the selected descriptor. It does not prove the semantic identity of the state byte (uniform, weapon, gender, animation orientation, etc.). Do not hardcode such labels or treat each raw animation sprite as the final rendered appearance.

## Verification
Pass: full original assembly blob inspected at cited lines, arithmetic and fallback paths traced.
Not run: private sprite-table record validation, decoded graphics inspection, build, Windows testing, visual equivalence or performance measurements.
No engine/gameplay, assets, CI or other branch changes. Next: trace the provenance and bounds of data_5e4ab and establish how index values map to the actual private sprite records.
