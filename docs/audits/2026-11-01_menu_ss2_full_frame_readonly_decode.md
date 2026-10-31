# Syndicate 2026 — Private Menu Animation Full-Sequence SS2/BRUN Read-only Audit

Date: 2026-11-01 (Australia/Brisbane)
Status: focused offline animation-frame reconstruction completed for six existing private files; Windows engine output NOT verified.
Branch: feature/v0.9-visual-showcase; protected baseline main v0.7 a22b0b9580a9710b4e693e88854351835ef1ee40.
Evidence: original assembly syndre.sx:61703–62178 and six private v0.6 game-resource members. No commercial bytes or imagery committed.

## Method
Expanded the temporary read-only Python animation parser from the earlier first-frame audit. It reads the original file and frame headers, follows sequential subchunk lengths, reconstructs the BRUN initial 320x200 frame and implements SS2 delta packets according to the assembly's signed row-skip, optional last-pixel and signed word-run/literal behaviour. Every requested output update is bounded to a 64,000-byte indexed pixel buffer. Type-4 palette chunks were detected but palette evolution/colour conversion was **not** incorporated into this indexed-frame sequence.

## Results
| Asset | Declared frames | Indexed frames reconstructed | Observed chunk type counts |
|---|---:|---:|---|
| mtitle.dat | 15 | 15 | PAL=1 BRUN=1 SS2=14 |
| mselect.dat | 23 | 23 | PAL=1 BRUN=1 SS2=20 |
| mbrief.dat | 15 | 15 | PAL=1 BRUN=1 SS2=13 |
| moption.dat | 11 | 11 | PAL=1 BRUN=1 SS2=9 |
| mmap.dat | 28 | 28 | PAL=1 BRUN=1 SS2=27 |
| mconfup.dat | 9 | 9 | PAL=1 BRUN=1 SS2=8 |

All six data sets traversed their declared frames without out-of-bounds pixel writes or invalid SS2 packet locations in this isolated decoder. The first-frame record-length discrepancy already documented remains in all six; parser follows subchunk extents as original engine does. Some assets have additional trailing bytes after their declared frame sequence and a few initial BRUN chunks have a single unused payload byte. Neither condition has been established as corrupt or gameplay-relevant.

## Important limits
- This test validates **one independent software interpretation** of original frame-delta assembly against the private resources. It does not demonstrate visual equivalence to engine output, cursor/UI layering, audio synchronisation, input timing, animation looping or SDL focus.
- Full dynamic palette updates, title screen composition with menu text/buttons, and in-game screenshots are not yet tested.
- No game code, existing 16/30 FPS cadence, engine resource format, UI hitboxes, workflow or protected main changed.

## Next process
Review DoPalette and palette packet semantics across multiple frames, then cross-check frame 1 and later frames against actual Windows captures before authorising a v0.9A menu redesign. Preserve private resources outside GitHub and record separately any native build or runtime regression results.
