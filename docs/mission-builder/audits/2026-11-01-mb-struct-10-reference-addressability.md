# MB-STRUCT-10 — 16-bit objective reference addressability
Date: 2026-11-01 (Australia/Brisbane)
Status: STATIC SOURCE AUDIT ONLY; not an implemented editor or runtime test
Branch: feature/mission-builder-audit
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf, objective handlers ~679–770 and declarations ~85710–85741.

## Addressability
The observed objective handlers load unsigned 16-bit reference words (for example `xor reg,reg; mov data_9be40(...),%ax/%bx`) and add `level__things_base` = 0x8810e. Representable base-relative offsets are 0..65535, yielding addresses 0x8810e..0x9810d inclusive.
- People start 0x88110 (offset 2); 256 x 92 bytes; region [0x88110,0x8dd10).
- Vehicles start 0x8dd10 (offset 23554); 64 x 42 bytes; region [0x8dd10,0x8e790).
- Objects start 0x8e790 (offset 26242); 400 x 30 bytes; region [0x8e790,0x91670).
- All three complete static regions fit into a 16-bit relative offset range. Thus the prior concern that 16-bit offsets necessarily cannot represent all People, Vehicles or Objects is RESOLVED for these three regions. This does not validate an arbitrary offset as a valid entity.
- The 16-bit span also reaches beyond these entity regions into other data, e.g. Weapons and a portion of later regions. A mere <65536 check is insufficient to validate entity category.
- The maximum representable offset is not sufficient to reach all remaining mission data. Never interpret an objective reference as a pointer to arbitrary level structures.

## Encoding and validation
Type 1 handler at lines ~681–704 resolves objective +6 as base-relative offset, then reads linked +0x20 word(s), conditionally following up to two links by the same base-relative arithmetic. Types 2/3 perform direct dereference at target +0x0b. Types 5 and 15 also follow type-specific linked words. The code provides no proven universal tag or pointer type.
A read-only inspector must validate reference origin, record-start alignment, required field extent and link target category per handler. Reject a base-relative address that happens to fall in a non-entity region. Do not equate a 16-bit word with a pointer for every field, as some words are coordinates/parameters.

## Unverified
Runtime entity type semantics, special zero/sentinel encoding, exact file serialization, link cycles, and retail asset validity remain unverified. The offset 0 is `level__things_base` itself, not the first Person record (offset 2); zero must not be blindly accepted as an entity.

## Next
MB-STRUCT-11: determine objective link sentinel/zero conventions, category restrictions and whether a compact read-only synthetic validator can safely distinguish invalid from unknown references. No code changes until that contract is checked.

## Tests
Arithmetic and assembly inspection only. No build, synthetic parser test, retail asset validation or Windows gameplay test. No changes to protected v0.7 or classic saving.
