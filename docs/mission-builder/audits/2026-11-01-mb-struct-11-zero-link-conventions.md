# MB-STRUCT-11 — Zero-value and sentinel conventions in player objective references
Date: 2026-11-01 (Australia/Brisbane)
Status: STATIC SOURCE AUDIT ONLY — no implementation, synthetic testing or gameplay verification
Branch: feature/mission-builder-audit
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf (lines 677–802, 911–950), plus the symbol layout audited in MB-STRUCT-10.

## Confirmed distinctions
1. Type 1 (`func_10790`, lines 679–704): objective field +6 is converted to an unsigned offset relative to `level__things_base`, with **no zero check before** dereferencing status byte +0x0b and modifying byte +0x1c. This primary reference cannot safely be assumed optional. Afterward the first linked word at entity +0x20 **is** explicitly checked against zero; zero causes the routine to stop following links. A subsequent +0x20 word is similarly checked; zero means retain the current linked entity, not create another one.
2. Type 5 (`func_108b8`, lines 765–779): similarly resolves objective +6 unconditionally and reads the primary entity +0x20. If that first link is zero, the handler stops; second linked word zero means use the first link's entity.
3. Types 2/3 (`func_10855`, `func_10889`, 731–755) resolve field +6 and read +0x0b without a zero-reference guard. Do not treat zero as universally valid or universally optional.
4. Type 15 (`func_10ac8`, 915–949) resolves field +6 and reads its +0x1c **word**; zero skips the ownership-completion check but the handler still checks the primary entity +0x0b for failure. Unlike type 1, entity +0x1c is not merely a universal pointer field: type 1 modifies its low byte as flags. Its interpretation is entity/handler-dependent.
5. `level__things_base` = 0x8810e and `level__People` = 0x88110: a raw offset zero resolves to the base header preceding the first Person, rather than a validated person record. Therefore a read-only inspector must flag a primary objective reference of zero as **unresolved/unsafe**, not silently substitute the first person.
6. A nonzero linked reference may still be invalid, misaligned, wrong category or outside active data. Zero checks in assembly are *control flow semantics*, not sufficient validation of malformed custom content.

## Proposed validator dispositions (not implemented)
- Distinguish `required-primary`, `optional-link` and `contextual-word` fields for each objective type; do not impose a generic zero-is-null rule on all 16-bit words.
- For type 1 and 5 primary references: reject zero or non-entity values for authored content before any dereference. For observed zero secondary +0x20 links, classify as 'end of link chain' only in those handlers and positions.
- For type 15 +0x1c zero, report 'no completion link' as a raw structural observation; do not promise mission authorability or eventual completion.
- Flag unknown/special sentinel values (including 0xffff) as **unverified**, not automatically absent or valid; require a per-handler source/fixture basis before classification.
- Preserve source-verified objective conditions and separate AI-objective data. No write/serialize facility, new objective naming or gameplay rules approved.

## Next audit
MB-STRUCT-12: examine literal 0xffff and other sentinel comparisons in mission-objective and referenced entity-link consumers, and produce a table of per-handler reference checks for a read-only inspector prototype.

## Verification
GitHub source inspected statically. No code edits, automated tests, build, real RNC input validation or Windows gameplay testing; original mission and between-mission saving mechanics untouched.
