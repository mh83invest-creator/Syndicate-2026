# MB-AUDIT-5.4 — RNC Method 1 resource contract
Date: 2026-11-01. Branch: feature/mission-builder-audit. Static audit only.

Evidence: engine/src/syndre.sx original blob c35e22621a93d1e07c44d40de8867446ff7c64bf, lines 14916–15041 and 62225–62356.

## Findings
- UnpackM1 verifies the RNC Method 1 signature at 62237–62242, then reads unpack_len and pack_len at 62243–62246, and block count at 62247–62249.
- The decompressor includes an overlapping-input relocation path at 62250–62284 and uses table-driven block decoding at 62285–62341.
- Its normal completion sets EAX to unpackm1__unpack_len and clears carry; invalid signature sets EAX to zero and carry (62342–62349). Full malformed-data behaviour is not established.
- Assembly LbFileReadRNC calls LbFileLengthRnc, LbFileRead_, and UnpackM1 but returns the retained handle-related value, rather than propagating the decoded length (14916–14960).
- LbFileLengthRnc examines the RNC header and extracts a four-byte length at 15005–15017, with a non-RNC file-length fallback at 15019–15023.
- No comprehensive decoded-size validation was identified at the tactical load call boundary. Source-level objective offsets remain memory-relative, not verified compressed-file positions.

## Open verification
- Inspect lawful source mission files to compare compressed length, uncompressed length, and the memory region occupied after load.
- Verify loader read count and error outcomes using an isolated test harness before proposing any mission writer or validator.
- Check target buffer bounds, linked entity offsets and objective initialisation after decode.

## Protection and tests
Original and American Revolt, protected v0.7, fixed 16 FPS and campaign save isolation unchanged. Documentation only; no engine modifications, builds, Windows playtests, asset tests or save edits.