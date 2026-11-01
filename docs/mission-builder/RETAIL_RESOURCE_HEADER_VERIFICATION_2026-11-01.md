# Mission Builder — Authentic data RNC verification (2026-11-01)
Branch: feature/mission-builder-audit
Status: read-only fixture verification; no original commercial resources committed.

## Input provenance
User-supplied Syndicate_2026_v0.8_Camera_Speed_Prototype_Test(3).zip.
ZIP SHA-256: 9018872d38854bf367bad10a1ce2f68ecb8f072b0330598eb47f4f835854d559
game01.dat SHA-256: 959119f7b26efa2674516c386293a5fcf6923138521041ec34a18892932c1fbf.
This is a v0.8 prototype test archive containing data files; provenance of its commercial assets has not been independently established.

## Verified observations
- 60 mission entries named game01.dat through game50.dat and game90.dat through game99.dat.
- All 60 have RNC Method 1 signature, 18-byte header, expanded length 116010 bytes (0x1C52A).
- For all 60, physical file length equals 18 plus the header's big-endian packed length.
- Independently calculated RNC CRC-16 (polynomial 0xA001, initial zero) of packed payload matches each file's header CRC.
- ZIP integrity check completed successfully, no corrupt archive entries.
- The 116010-byte expansion target exactly matches assembly [level__Seed, level__End) calculated from 0x80108 to 0x9C632.
- A local read-only Python header/CRC inspector was created and executed against all 60 entries. Two negative tests (short/invalid signature and incomplete header) were run; neither was accepted.
- These are packed-content and header checks, NOT successful decompression, unpacked CRC verification, objective field validation, or Windows gameplay.

## Resolved and unresolved gates
R01 moves from missing authentic fixture to PARTIAL VERIFIED: real resources now available for isolated read-only tests and their reported level size and packed integrity match engine expectations. Still OPEN pending bounded decompression and structural reference verification.
R02 read-only bounded decompressor: OPEN.
R03 decoded objective inspector: OPEN.
R04 corruption fixture suite: only basic header negatives run; OPEN.
R05 type semantics: OPEN.
R06 American Revolt resource identity: game90–99 existence alone does not establish campaign mapping; OPEN.
R07–R11 routing, saves, packaging, Windows testing: OPEN.

## Preservation
No game executable, source, original data, saves or graphics modified. No commercial file bytes committed. Keep protected v0.7, 16 FPS game loop and original and expansion save compatibility untouched.
