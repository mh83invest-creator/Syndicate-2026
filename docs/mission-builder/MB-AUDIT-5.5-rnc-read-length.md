# MB-AUDIT-5.5 — RNC read length comparison
Date: 2026-11-01. Static audit; no engine changes.
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Evidence
- In LbFileLengthRnc (14964–15041), header bytes 4 through 7 are combined to produce the returned length when RNC signature matches (15005–15017).
- In UnpackM1 (62225–62246), the first four header bytes are checked and the next four are stored in unpackm1__unpack_len, followed by four bytes stored in unpackm1__pack_len.
- Thus the length read from header offset 4 is the expected UNCOMPRESSED length, not the compressed payload length.
- Assembly LbFileReadRNC (14916–14960) uses the returned LbFileLengthRnc value as the LbFileRead_ byte count, without a visible comparison against packed size plus header or an explicit short-read count check.
- This may reflect assumptions used by original content and in-place unpacking, but runtime effects cannot be established without asset samples and tests.

## Open gates
- Verify actual compressed and uncompressed lengths using legal retail resource samples.
- Trace exact LbFileRead_ result and memory bounds with a disposable Windows test environment.
- Do not generate or load untrusted custom missions using this path until input validation has been designed and tested.

## Verification
Static assembly only; no code, assets, saves, builds or gameplay tests. Preserve protected baseline, 16 FPS, original campaigns and campaign save isolation.