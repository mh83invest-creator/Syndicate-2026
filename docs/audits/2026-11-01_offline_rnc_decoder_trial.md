# Syndicate 2026 — Offline RNC1 Decoder Trial (Partial Success)

Date: 2026-11-01 (Australia/Brisbane)
Status: diagnostic trial completed; decoder validation FAILED for part of the private data set; do NOT treat all decoded output as authoritative.
Branch: feature/v0.9-visual-showcase
Protected baseline: main v0.7 a22b0b9580a9710b4e693e88854351835ef1ee40
Scope: read-only diagnostic Python RNC1 decoding experiment against the user's existing private v0.6 ZIP. No source, workflow, engine or game assets modified. The independent temporary Python decoder was created only in the private execution environment and is NOT production code or a validated decoder.

## Trial method and results
The Python diagnostic implements the RNC1 signature, big-endian header, packed CRC16, little-endian bit reading, Huffman tables, literal/backreference copying and unpacked CRC16, by reference to engine/bflibrary/src/general/rnc_1fm.c. It checks expected decoded length and unpacked CRC before accepting decoded data. The script is a non-authoritative research implementation and needs validation against the game's C decoder.

Across the 143 compressed private archive resources:
- 103 passed the temporary decoder's unpacked-length and CRC checks.
- 40 failed to decode using this implementation, with input-end or literal-bounds errors. **This is a decoder-trial failure, not evidence of corrupt retail assets.**
- The compressed payload lengths and CRC16 checks previously passed for all 143/143 resources, which strongly indicates the archive input itself is structurally intact.

Among the five high-resolution palette files:
- hpal01.dat: passes decoded size 768 and unpacked CRC, with 19 distinct RGB triplets.
- hpal02.dat: passes decoded size 768 and unpacked CRC, with 18 distinct RGB triplets.
- hpal03.dat: passes decoded size 768 and unpacked CRC, with 19 distinct RGB triplets.
- hpal04.dat and hpal05.dat: **not validated** by the temporary decoder. Do not describe their colour content or night-lighting potential as decoded.
- Decoded palette component values for the first three passing files are within 0–63, consistent with the 6-bit components used by the game's SDL palette setter. No actual game image has been rendered.

## Limitations and next action
The experimental decoder's 40 failures must be investigated against original C semantics before relying on it for any of the other assets. Run a build/test of the original rnc_1fm.c unpacker in an isolated harness, or correct the independent decoder and require **143/143** length/unpacked CRC passes. Do not change the protected game engine to hide decoder failures. Once decoded, assess all five palettes and sample map geometry, then conduct actual Windows visual tests separately.

No private original game files, raw decoded palettes or proprietary imagery have been published to GitHub.
