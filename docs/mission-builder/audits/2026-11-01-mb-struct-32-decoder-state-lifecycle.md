# MB-STRUCT-32 — RNC decoder state lifecycle and reentrancy

Date: 2026-11-01 (Australia/Brisbane)
Branch: feature/mission-builder-audit
Status: static source audit, no implementation or executable testing.
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Source observations
1. `UnpackM1` reads header magic before setting its decode globals (62225–62249). A bad magic/version exits at 62345–62356; these early-return paths do not reset prior global decode state.
2. On a recognized header, `unpackm1__unpack_len`, `unpackm1__pack_len` and `unpackm1__blocks` are assigned (62243–62249). At the start of decoding it sets `unpackm1__bit_count` to zero and initializes `unpackm1__bit_buffl` (62285–62290).
3. `unpackm1__bit_buffh` is read in `input_bits` (62404) but an explicit reset at the entry to `UnpackM1` is **not visible** in this routine. The first `input_bits(2)` call may use this inherited high buffer when its bit-count logic takes that route; exact effect on valid inputs is untested. Do not assert a reproduced cross-file decoding fault.
4. Huffman tables are global adjacent 128-byte buffers (85164–85183). `make_huftable` fills them per block (62291–62297), but has no whole-table zeroing or explicit logical-count boundary (62448–62522). Unwritten entries can retain previous-call contents; whether valid tables ever read such entries remains unverified.
5. `unpackm1__counts` is overwritten at the start of each block (62298–62300); `unpackm1__bit_buffh` is assigned after literal chunks (62322–62336) and in `input_bits` (62439). Globals are shared across calls, not per-instance objects.
6. Source has no synchronization or independent decoder context for concurrent invocations. This is evidence that thread-safe reentrancy is not established, **not** proof the original game invokes the decoder concurrently.
7. Additional MB-STRUCT-8 correction: `can_i_persuad_you` saves `last_person` in EBP at 43479, then writes the same EBP value back at 43658/43730/43744/43758/43772/43781. Those six stores are NOT evidence that the boundary was changed. The prior dynamic-boundary claim is overstated; actual boundary mutation elsewhere remains to be established. `set_structure_ends` remains the verified initialization writer (27660–27705).

## Proposed test contract — NOT IMPLEMENTED
S01: Decode fixture A, then B, then A again; require first and third outputs identical.
S02: Decode valid A following invalid header, truncated input and malformed Huffman inputs; require no state contamination.
S03: Decode identical fixtures in independent fresh contexts; compare output, consumed bytes and error codes.
S04: Do not share Huffman tables or bit buffers across concurrent instances; add multithreaded interleave tests only to a new safe decoder.
S05: Require explicit initialization of every buffer/count on all paths, including failure, with bounded cleanup.
S06: Validate zero-block, zero-run and malformed-code rejection deterministically without infinite loops or out-of-bounds access.
S07: Test authentic compressed missions and synthetic edge cases; none were executed here.

## Decision
No Mission Builder writer or use of legacy `UnpackM1` with arbitrary files approved. An independent bounded decoder with owned state remains required. Preserve original campaign, graphics, engine and between-mission-only saves.

## Next
MB-STRUCT-33 — audit exact bit-buffer initialization on the first symbol and block boundaries, including behavior of successive valid/invalid invocations.

## Verification
GitHub source inspected; no code, resource, asset, build or gameplay changes. No runtime, synthetic or Windows tests.
