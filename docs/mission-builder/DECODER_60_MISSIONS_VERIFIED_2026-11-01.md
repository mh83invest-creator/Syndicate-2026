# Mission Builder — RNC1 decoder accepted against supplied fixtures (2026-11-01)
Branch: feature/mission-builder-audit
Status: local fixture decoder validation passed; no engine integration performed.

## Correction to earlier prototype report
DECODER_PROTOTYPE_TEST_2026-11-01.md recorded 32/60 with an overly restrictive zero-length terminal literal boundary check. The error was in the isolated Python implementation, not evidence of corrupt mission files. After correctly allowing a zero-length terminal literal without requiring an additional input byte, all 60 mission files decode. The previous failed result remains as historical test evidence; this report supersedes its present decoder status.

## Verified local tests
Input: user-supplied Syndicate_2026_v0.8_Camera_Speed_Prototype_Test(3).zip, kept out of GitHub.
- 60/60 gameNN.dat files decoded to exactly 116010 bytes.
- Every decoded payload matched its header uncompressed CRC16, in addition to header/packed payload length and packed CRC16.
- Six negative tests rejected: short header, invalid signature, truncated payload, mutated compressed payload, oversized output declaration, and zero block count.
- Decoder reads ZIP members into memory, does not modify original files, and caps expanded output.
- All 60 sample files have the expected level memory layout. Read-only preliminary scan of eight tactical objective type words per mission found type frequencies across 480 slots: 0=370, 16=30, 1=25, 2=22, 11=12, 14=8, 3=5, 5=4, 15=3, 10=1.
- These numeric types are source values, NOT verified human-facing mission objective descriptions. Zero type is common; its specific semantic status needs verification.

## GitHub changes
tools/mission_builder/rnc_readonly.py: read-only RNC1 decoder and optional JSON summary. Commit 367b33b986bfc736919a1436f47ca04bfa8266a3.
tools/mission_builder/test_rnc_readonly.py: fixture-dependent 60-file and six corruption regression checks. Commit dd5137fe2e4a9ad43e523931a56908aae6b3a786.
This report documents local testing of the source-matched version; the GitHub committed copies were not separately executed after upload. No game engine, assets, saves, original campaigns or release branch changed.

## Outstanding
Windows build/gameplay, independent decoder implementation comparison, fuzzing, entity reference validation, mission record semantics, and campaign/save integration remain open. Do not deploy this Python decoder as an arbitrary untrusted-file service solely on these tests.
