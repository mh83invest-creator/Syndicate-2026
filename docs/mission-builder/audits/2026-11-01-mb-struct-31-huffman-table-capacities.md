# MB-STRUCT-31 — Static Huffman table allocation and write boundaries
Date: 2026-11-01 (Australia/Brisbane)
Status: Mission Builder static audit only. No implementation or tests.
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Direct source findings
- Symbols `unpackm1__raw_table` (0x5be30), `unpackm1__pos_table` (0x5beb0), and `unpackm1__len_table` (0x5bf30) each have `.fill 0x80`, exactly **128 bytes** of statically allocated storage (85164–85169). They are contiguous, followed by decoder length/counter/bit-buffer globals.
- `UnpackM1` invokes `make_huftable` three times per compressed block (62291–62297), passing each table address.
- `make_huftable` allocates 16 local bytes on the stack, reads a 5-bit symbol count (0–31), and writes one code-length byte per symbol without an observed count clamp before the temporary writes (62448–62465); see MB-STRUCT-30.
- During table construction, the destination advances through `stosw` writes, while another word is stored at `0x3c(%edi)` (62474–62504). The output allocation must therefore account for **both** sequential writes and displaced metadata writes; 128 allocated bytes cannot by itself establish the maximum valid input count.
- `input_value` searches generated table entries without an explicit table-entry bound visible in the loop (62369–62396).
- Decoder state globals are adjacent to the third table (85170 onward). A source-level off-table write could affect adjacent state; this has not been reproduced or proved reachable with genuine files.

## Bounded parser gates — not implemented
T01: Define a validated maximum symbol count and code-length range for the format before any writes.
T02: Allocate tables to an independently derived maximum and check each sequential and displaced metadata write.
T03: Validate canonical Huffman code completeness/prefix consistency and bound table search to recorded entry count.
T04: Reject duplicate/impossible code assignments, invalid counts and all out-of-range bitstream reads.
T05: Keep separate decode state per file (not shared mutable globals) and validate block-level reset.
T06: Enforce compressed-input, expanded-output and backreference limits from MB-STRUCT-27–30.
T07: Run synthetic overflow/invalid-table tests and authentic gameNN.dat fixtures before claiming compatibility.

## Outstanding
Precise maximum legal Huffman table entries and full format semantics need independent validation and sample files. No actual overrun or crash has been demonstrated.

## Next
MB-STRUCT-32: inspect decoder state reset and successive-file behaviour to specify reentrancy/non-contamination tests.

## Verification
GitHub assembly source inspection only. No source changes, fixture generation, tests, compilation or gameplay.
