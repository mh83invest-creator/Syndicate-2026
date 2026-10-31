# Syndicate 2026 — Private Data Payload Parity Between Historical Test Archives

Date: 2026-11-01 (Australia/Brisbane)
Status: completed offline file-hash comparison; not a gameplay or full-package validation.
Branch: feature/v0.9-visual-showcase. Protected baseline: main v0.7 a22b0b9580a9710b4e693e88854351835ef1ee40.

## What was compared
User-uploaded archive Syndicate_2026_v0.6_Two_Menus_30FPS_Test(1).zip against Syndicate_2026_v0.8_Camera_Speed_Prototype_Test(1).zip. For each archive, files under the archive's data/ directory were keyed by relative filename and SHA-256 of the uncompressed ZIP member bytes. This is an archive-member integrity comparison, NOT an RNC asset decompression and NOT a check of executable or DLL parity.

Results:
- 437 data files in each archive.
- 437 matching relative names.
- 437 of 437 file-content SHA-256 digests match.
- Zero missing, extra or content-different data members between these two archives.

## Consequences
The uploaded v0.6 private asset set can serve as the **same exact asset-data reference** as the uploaded v0.8 prototype asset set for offline parsing and rendering-format research. This does not make the v0.8 executable good: its camera and focus playtest failures remain documented. Nor does matching v0.8 data prove v0.7 historical package data equality (the v0.7 package is not among these two archives).

Do NOT commit the proprietary game data, the member contents, entire hashes of every file, or commercial artwork to the public repo. Do not mix unverified executable and DLL sets for gameplay conclusions. All historical build versions and user test statuses remain unchanged.

## Verification
PASS: Python standard-library zipfile member enumeration, SHA-256 digest comparison, identical relative filename sets, 437/437 matches.
NOT RUN: runtime startup, game data load through engine, real palette decoding, sprite/map rendering, Windows gameplay, original v0.7 package comparison.

## Next step
Proceed with offline RNC decompressor/source audit and palette/map decoding from existing private local ZIP; keep produced asset samples in the private execution environment, not GitHub. Preserve original engine and protected main unchanged. GitHub handover and index to be verified after commit.
