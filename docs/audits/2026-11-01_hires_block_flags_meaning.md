# Syndicate 2026 — Original high-resolution block-flag initialization

Date: 2026-11-01. Focused static-source audit COMPLETE, Windows runtime NOT RUN. Protected main v0.7 unchanged. Original engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

At lines 79739–79781, `init_hires_blocks` initializes the 256-byte `data_5e39c` table (allocation at 85338–85339). Each block-ID entry begins as zero, then the code iterates six component fields of the 24-byte block descriptor. For each component, it visits successive data words separated by 0x14 (20 bytes), testing 16 entries (initial CH=15 with loop while nonnegative). If all 16 tested words are zero, it sets the corresponding one of six bits in `data_5e39c`. These bits therefore represent **precomputed empty-component conditions** under that source test, rather than lighting flags. The source repeatedly tests mask values 1,2,4,8,16,32 during block composition; branch reachability is still to be verified in actual gameplay.

The function is called at assembly line 27913 as part of level initialization. This reconciles the earlier reports that treated the bits as unknown and narrows the apparent component-selection rules. It does not prove which block IDs are windows or lamps, nor whether a light layer is feasible.

PASS: source inspection of initialization loop, 256-byte table, descriptor stride and six bit masks. NOT RUN: runtime initialization and byte-for-byte flag comparison against actual game, renderer capture, camera/focus tests, effect profiling. OPEN: exact six-bit mapping to component fields and whether 16-word emptiness predicate matches the 320-byte planar consumer bounds for every asset. No engine or proprietary resource changes.
