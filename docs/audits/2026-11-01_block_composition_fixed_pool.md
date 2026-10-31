# Syndicate 2026 — Block Composition Pool and Bitplane Consumer Audit

Date: 2026-11-01 (Australia/Brisbane)
Status: focused original assembly source trace complete; dynamic bounds and visual semantics still unverified.
Branch: feature/v0.9-visual-showcase
Protected baseline: main v0.7 a22b0b9580a9710b4e693e88854351835ef1ee40
Evidence: original engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.
No engine, production features, original resources or workflows changed.

## Confirmed findings
- syndre.sx:71557–71560 shows func_48a3a resetting ECX to zero before a collection pass.
- syndre.sx:71585–71623 explicitly reserves **36 four-byte entries** (144 bytes) for data_48ad0.
- syndre.sx:71628–71739 and 72330–72400 have multiple component-collection paths that store newly rebased h_blocks component pointers at data_48ad0(ECX*4), then increment ECX. In the snippets checked, the index is not compared directly against 36 before each store. The overall maximum path length is unproven, so this is a **conditional capacity risk**, not a demonstrated overrun.
- syndre.sx:66362–66390 shows a consumer running the list in reverse, loading a component pointer and combining successive 32-bit words by AND/OR onto separated destination offsets. The observed destination offsets include 0x7d00, 0xfa00, 0x17700 and additional +0x50 variants. This is real multi-plane bitmask composition; it is not a simple per-pixel 24-bit image blit.
- An important consequence of these destination strides is that future colouring/glow changes cannot be assumed to act as ordinary RGB framebuffer overlays at this stage. The individual block component references are used to build 4-bit plane imagery before the later screen conversion and SDL presentation.

## What this stage does NOT prove
- That the 36-entry list ever overflows on original map content.
- The precise full bounds of each pointed-to component record or its graphics dimensions.
- Which block IDs correspond to lamps, windows, neon, occluders, or road surfaces.
- Any regression involving original camera timing, cursor, focus or explosion effects.

## Verification ledger
PASS: GitHub read of assembly list declaration, producer, reset and consumer; static confirmation of fixed storage and bitwise composition.
NOT RUN: compile/instrumentation, synthetic stress harness, original Windows gameplay, asset-specific max component counts, renderer screenshots and new lighting tests.

## Continuation
Trace every producer branch and loop exit to prove a hard list maximum or document a bounded test-only stress harness; sample legitimate maps privately to determine whether paths approach 36 components. Continue tracing block sprite projection and dirty-rectangle repaint. Do not add actual lighting or repair incidental risks until separately approved. Stable main untouched.
