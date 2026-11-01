# MB-STRUCT-26 — Native mission fixture evidence and acceptance requirements
Date: 2026-11-01 (Australia/Brisbane)
Workstream: Mission Builder; source audit/documentation only.

## Reconciled evidence
- MB-STRUCT-6 establishes the *memory-image-relative* player objective table: level__Seed 0x80108, objectives 0x9be3a; offset 0x1bd32 (113970), eight entries of 14 bytes and a minimum *candidate* memory extent 114082 bytes. Level clear span is 116010 bytes; neither number is proven to be the packed file size or mandatory RNC output length.
- MB-STRUCT-14 traces native LbFileReadRNC/UnpackM1 and its missing observed output-capacity/status enforcement. Do not feed arbitrary fixtures through the native loader.
- MB-STRUCT-15 confirmed tactical `data/gameNN.dat` from `level_fname` caller tracing, superseding only MB-STRUCT-14's historical uncertainty about the filename; it did **not** verify the compressed payload layout.
- MB-STRUCT-20 finds installer-listed `game01.dat` through `game50.dat`, plus `game90.dat` through `game99.dat`. Listing does not establish which files a specific owned edition contains.
- MB-STRUCT-13 defines synthetic F0–F7 test fixtures but no fixture or parser has been implemented.

## Required authorised samples — requested for eventual validation, not obtained
1. **Mission container samples:** At least two different authentic `data/gameNN.dat` files from one legitimately obtained compatible edition (prefer `game01.dat` and a different listed mission if available), read from a disposable copy without altering originals.
2. **Variant check:** An authentic mission from any additional edition/campaign mode intended for support, when available. Do not assume American Revolt assets have the same numbering, encoding or layout.
3. **Associated context:** Corresponding `data/mapNN.dat` and mission text files only when source-backed mapping makes them relevant; record filename, source edition, file sizes and SHA-256 hashes, but **do not assume matching numeric suffixes imply association**.
4. **Known-good behaviour:** Screenshot or tested description of the mission goal and in-game completion where possible, to help validate objective semantics. No request to modify or publish copyrighted resources.
5. **Format controls:** Independently derived short-header, bad-magic, truncated and oversize synthetic containers, explicitly labelled synthetic; these are generated only after a bounded parser exists.

## Acceptance checks — NOT RUN
A01: collect filename, edition/provenance, exact byte length and hash for each authentic sample.
A02: inspect first 18+ container bytes in a read-only bounded reader, document RNC magic/version and header big-endian expanded/packed length fields using source-confirmed definitions; reject short/invalid samples.
A03: establish actual output size under bounded independent decompression; do not infer output length from clear region or table offset.
A04: check image origin/layout against level__Seed and objective table end, including endian and eight records; reject insufficient extent.
A05: verify entity region alignment and source handler references, with type 16 cross-objective dependencies, against authentic samples; classify unknowns rather than fabricating semantics.
A06: compare synthetic F0–F7 cases to validated structural expectations, including source non-mutation, stable diagnostics and no execution of native mission handlers.
A07: cross-compare two real mission files for consistent format; do not generalize to American Revolt or other editions without samples.
A08: record reproducible evidence and test results; no authoring/export/editor claim until all needed read-only checks pass.

## Unresolved
- Physical file packing, container consistency, expanded byte counts and actual type/offset semantics have not been validated on retail resources.
- No original game assets are supplied/accessible in this audit; no automatic asset retrieval or invented byte checks.
- RNC decompressor and inspector remain unimplemented. Custom campaigns, save isolation and packaging are separate outstanding integration gates.

## Next
MB-STRUCT-27: source-review exact RNC header field layout and decompressor failure paths sufficient to specify a bounded header-only validator, still without executing untrusted content.

## Verification
Cross-read audits MB-STRUCT-6/13/14 and later source-backed filename/installer audits; committed documentation only. No asset access, fixtures generated, code changes, automated tests, builds or gameplay.
