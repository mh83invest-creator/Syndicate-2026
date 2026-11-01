# MB-STRUCT-30 — Huffman table, bit reads and copy-back bounds
Date: 2026-11-01 (Australia/Brisbane)
Scope: Mission Builder source audit only; no code or decoder changes.
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Inspected source facts
- `make_huftable` (62448–62522) reserves 0x10=16 bytes of local stack workspace (62452–62453), reads a **5-bit** count using `input_bits` (62454–62457), and writes one byte per counted symbol using `mov %al,(%edi); inc %edi; loop` (62459–62465). A 5-bit count permits 0..31; no visible clamp to 16 occurs before this local write. Thus counts of 17..31 are a **conditional local-buffer overwrite risk** on malformed input; not a reproduced failure or proof authentic assets contain such values.
- `make_huftable` generates code data into caller-supplied output tables, including writes at offsets +0x3c relative to the current output pointer (62474–62504). The inspected function has no explicit destination table capacity argument. Safe table capacity and code-count constraints must be specified and proven separately.
- `input_bits` (62399–62444) draws from the bit buffer, advances `%esi` by 2 and dereferences `(%esi)` (62424–62425) without an explicit input-end argument. It is a low-level bit helper, not a bounded input-stream validation interface.
- `input_value` (62369–62396) scans entries until a matching masked code is found (62374–62380); no explicit table-length termination is visible in this loop. An invalid Huffman table therefore needs fail-closed matching rather than unbounded scanning.
- `UnpackM1` raw copies and back-references use `rep movsb` (62312–62321) with source based on `destination - decoded offset`; no explicit input/output bounds are passed to those operations. The surrounding low-level routine provides no sufficient untrusted-input safety contract.
- Source evidence is conditional: actual table allocations, stack corruption behaviour and the full range of valid RNC method-1 coding constraints were not runtime-verified.

## Proposed independent decoder acceptance gates (not implemented)
D01: Reject Huffman symbol counts above a verified safe maximum, never write into an undersized temporary workspace.
D02: Require bounded table allocation, valid code lengths/prefix properties and a terminable lookup for every symbol.
D03: Enforce bitstream read limits at each operation, including word refills; reject truncated input deterministically.
D04: Limit output to declared unpacked length and configured hard cap; validate literal count before copying.
D05: Validate backward distances as strictly positive and within already produced output; reject impossible references.
D06: Bound the number of blocks, runs and table rebuilds, including invalid zero block counts and exhausted input.
D07: Require consumed/produced byte counts and (when established) CRC agreement, independently of legacy carry flag/handle.
D08: Never invoke unbounded native `UnpackM1` on arbitrary Mission Builder input.
D09: Preserve game assets, original loader and between-mission save-only behaviour; no writer/editor route until real fixtures and tests validate decoding.

## Next
MB-STRUCT-31: inspect static Huffman table allocations and the precise output-record capacity relative to dynamic Huffman codes; write source-backed limits without pretending retail-file tests have passed.

## Verification
GitHub assembly inspection and documentation only. No source modification, assets, test execution, compiler or gameplay.
