# Syndicate 2026 — Map position block-ID audit

Date 2026-11-01. Static private asset check, not a game test. Protected main v0.7 unchanged. Original source syndre.sx:71638-71739 reads column bytes +10/+11 as candidate block descriptors.

Using existing private v0.6 ZIP and experimental read-only RNC1 decoder (in-memory 512-byte zero lookahead, CRC-checked), decoded all 59 maps and hblk01.dat. All 724992 twelve-byte column records were within decoded-file bounds. Position +10 has 115 distinct values; +11 has 92. Union of values above 4 contains 125 distinct candidate descriptor IDs, range 6-255.

For each of 125 candidate IDs, checked six 32-bit relative fields at byte offsets 0,4,8,12,16,20 within the 24-byte hblk descriptor. Nonzero counts: 97,115,107,99,116,108 (642 total). All 642 nonzero offsets satisfy offset+320 <= decoded hblk length 321664.

PASS: decoded CRCs, 724992 column record bounds, 642 sampled component extents. NOT RUN: native C decoder, actual reachable draw branches, 36-entry list stress, Windows build/play, lighting or explosion benchmarks. Candidate-ID set over-approximates branches; these are not proven light objects. No source code or commercial game data committed.
