# Graphics audit: animation starts table source trace

Date: 2026-11-01 (Australia/Brisbane)
Branch: audit/graphics-assets-2026-11-01
Status: focused original assembly audit complete; retail asset records and runtime untested.

## Verified source references
Source: engine/src/syndre.sx original blob c35e22621a93d1e07c44d40de8867446ff7c64bf.
- 82660–82662: data/hsta-0.ani loads to starts_ani independently of hfra-0.ani frames and hele-0.ani elements.
- 17665–17671, 21301–21306 and 37819–37824: routine selects a starts_ani table entry by indexing two-byte words, retrieves a word and stores it into a gameplay object's animation-related field (+0x10). In examined paths the input animation identifier is stored at object +0x12.
- 17685–17687: fixed byte offset 0x4b0 into starts_ani provides a word used for animation initialisation.
- 18792–18794 and 18812–18815: code indexes words into starts_ani at base offset 0xf3e plus a scaled index, supplying another animation selection/control state.
- 38751–38950: numerous fixed starts_ani word offsets (e.g. 0x242, 0x24a, 0x252) supply draw_element arguments in the original UI paths.
- Previously verified hfra-0.ani frame lookups use eight-byte record indexing and frame +0 points into ten-byte elements, while element +0 is a sprite-table byte offset.

## Bounded deductions
The examined starts_ani access patterns are 16-bit word entries. Previously verified unpacked length 3940 bytes would fit 1970 candidate words; that is a capacity calculation, not proof that all entries are valid animation starts.
Offset 0xf3e plus indexed access indicates the table has different logical ranges and should not be described as a single consecutive entity-animation list without classifying consumers.
A value retrieved from starts_ani is used in an animation control field or passed to drawing. Whether every such value is a frame index and which are sentinels, start codes or special selectors remains unverified.
Future parsing should check selected word bounds, references to frame capacity, frame-to-element links, cycles and termination. No retail data is available for this step.

## Verification
PASS: original full assembly blob inspection and byte offset/stride arithmetic.
NOT RUN: private hsta resource decoding, data-byte checks, rendering, Windows gameplay, timing or performance tests.
NO MODIFICATIONS: engine, build workflows, other branches, proprietary assets.
