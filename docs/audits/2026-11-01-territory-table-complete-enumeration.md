# Syndicate 2026 — Complete static territory adjacency table enumeration

Date: 2026-11-01. Source-only verification.

## Method
Read engine/src/syndre.sx embedded bytes between map_countries (assembly source line 84528, address 0x5b060) and data_5b416 (line 84692). Decoded all `.ascii` hexadecimal escapes and `.fill` zero runs. The resulting block is exactly 950 bytes, corresponding to 50 records of 19 bytes. Each record's eight adjacency values are bytes at offsets 9–16 inclusive; this matches the index calculation in source lines 32682–32695 and 32720–32729.

## Results
400 entries examined (50 records times eight). Minimum value 0; maximum value 50. Entries above 50: zero. Zero-valued entries: 310. For this built-in table only, the caller checks zero before subtracting one, and values 1–50 become indices 0–49. Therefore the specific suspected out-of-range adjacency-index defect is not supported for the compiled-in static table.

## Limits and next work
The result does not guarantee safety for corrupted or user-authored territory tables, nor validate resource byte length or destination screen bounds. The `cheat_worldmap` path bypasses playability conditions; custom campaign input should be validated before use. Next audit: drawing destination offset arithmetic, resource byte-capacity checks, and Mission Builder integration test requirements.

Verification was static byte enumeration and assembly review only. No Windows build, runtime test, code change, saves or game assets altered. Protected v0.7 and separate Mission Builder work unchanged.
