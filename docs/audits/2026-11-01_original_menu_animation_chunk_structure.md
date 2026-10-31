# Syndicate 2026 — Original Menu Animation Container/Chunk Audit

Date: 2026-11-01 (Australia/Brisbane)
Status: focused read-only source and six private asset samples audited; full animation decoding/visual tests still open.
Branch: feature/v0.9-visual-showcase. Baseline: main v0.7 a22b0b9580a9710b4e693e88854351835ef1ee40.
Evidence: engine/src/syndre.sx original blob c35e22621a93d1e07c44d40de8867446ff7c64bf (AniPlay, decode_FILE_HEADER, decode_FRAME_CHUNK, DoPalette); locally provided private v0.6 test archive. No original game bytes included in this public report.

## Source contract
- syndre.sx:61703–61844, AniPlay, reads a 4-byte record size and 2-byte record type. File header type 0xAF12 dispatches to decode_FILE_HEADER; frame type 0xF1FA dispatches to decode_FRAME_CHUNK.
- syndre.sx:61900–61918 reads the file-header payload and populates frame count, width and height.
- syndre.sx:61848–61896 reads the frame's subchunk count and 8-byte reserved portion; subchunks are processed by type: 7 (SS2), 4 (COLOUR256), 15 (BRUN), or unknown.
- syndre.sx:61943–62003 decodes palette subchunks into a phase-dependent scratch/palette buffer, not directly into a permanently static shared palette.
- AniPlay (non-DOS path) calls ac_swap_wscreen for frame display and ac_game_handle_sdl_events for input refresh; menu animation changes can therefore interact with original input/focus handling.

## Private sample structure verification
Read-only parse of six existing resource members, no pixel decoding:
| Resource | Reported header frames | Width x height | Frame records parsed using subchunk lengths |
|---|---:|---|---:|
| mtitle.dat | 15 | 320 x 200 | 15 |
| mselect.dat | 23 | 320 x 200 | 23 |
| mbrief.dat | 15 | 320 x 200 | 15 |
| moption.dat | 11 | 320 x 200 | 11 |
| mmap.dat | 28 | 320 x 200 | 28 |
| mconfup.dat | 9 | 320 x 200 | 9 |

Each sample begins with a 12-byte file-header record of type 0xAF12, followed by frame records of type 0xF1FA. All declared frame counts were traversed successfully by reading each frame's 16-byte prefix and the lengths of its declared subchunks; common types were 4, 7 and 15. No visual artwork or animation was rendered.

**Crucial compatibility finding:** In all six samples, the first frame's header-declared length differs from the actual combined lengths of its parsed frame header and subchunks. Example: mtitle.dat first frame header length 19,132 bytes while the subchunk-walked extent is 17,224 bytes. After the first frame, the following parsed frames' length fields agreed with their own subchunk extents in these samples. The original AniPlay loop reads subchunk payloads sequentially rather than seeking by the frame's declared length. Thus a replacement parser or menu-animation exporter that trusts the first frame length as its seek offset will misalign, even though the original engine can traverse the file. Why these first-frame lengths differ remains unverified; do not silently normalise the original data.

The files also contain bytes after the declared frame sequence; their meaning was not determined. This audit does not establish a safely editable new title or menu format.

## Verification ledger
PASS: Source tracing; header 0xAF12/frame 0xF1FA checks; six header frame counts; six subchunk-walked frame counts and first-frame length discrepancies.
NOT RUN: original decode_BRUN/decode_SS2 graphics output, animation timers, click navigation, language variants, mission gameplay, actual Windows build.
OPEN: exact palette chunk application, trailing data semantics, how frame animation affects menu screen timing and UI redraw.

## Continuation
Inspect original SS2, BRUN and colour-chunk implementation to establish output and scratch bounds; cross-check other menu animation files. Do not modify gameplay source, original assets, SDL input, menu hitboxes or the 30 FPS strategic screens during the audit. Main remains protected; rollback is documentation-only.
