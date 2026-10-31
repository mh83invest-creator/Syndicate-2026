# Syndicate 2026 — Private block component extent check

Date: 2026-11-01. Source-only feature branch; protected v0.7 main unchanged.

The original engine/src/syndre.sx assembly (blob c35e22621a93d1e07c44d40de8867446ff7c64bf) uses 24-byte h_blocks descriptor entries and reads 320-byte component regions in planar composition.

In a read-only Python test using the supplied private game archive, hblk01.dat was decoded to 321664 bytes with its expected length and unpacked CRC. The previously recorded experimental decoder was modified **in memory only** to allow 512 zero-valued bytes of input lookahead. Its production source and private original assets were not changed.

The first 256 candidate 24-byte records were examined. Six little-endian pointer fields at byte offsets 0, 4, 8, 12, 16 and 20 contained respectively 222, 241, 229, 223, 241 and 230 nonzero values. For all 1386 nonzero values, pointer + 320 was at most 321664, and no start offset was outside the buffer. Maximum field offsets were respectively 319744, 320064, 320384, 320704, 321024 and 321344. The last maximum leaves exactly 320 bytes.

PASS: private decoder length/CRC, sampled 256 x 6 field extraction and 320-byte extent checks. NOT RUN: original compiled RNC C decoder, Windows gameplay, full valid descriptor ID range, all draw paths, list maximum, lighting or performance tests.

This does not prove that all six fields always refer to identical 320-byte blocks, nor identify any streetlight/window objects. Continue identifying which descriptors are reachable in actual maps. No gameplay source, workflow or protected baseline changes.
