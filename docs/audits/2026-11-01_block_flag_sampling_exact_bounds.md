# Syndicate 2026 — Exact high-resolution block flag sampling audit

Date: 2026-11-01 (Australia/Brisbane)
Status: focused original-source inspection completed; no engine run or game modifications.
Audit branch: feature/v0.9-visual-showcase
Protected main: v0.7 at a22b0b9580a9710b4e693e88854351835ef1ee40
Source: engine/src/syndre.sx original blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Exact observations

At syndre.sx:79748–79774 the initializer iterates 256 IDs, with six successive four-byte relative pointers from each 24-byte high-resolution block descriptor. It computes h_blocks plus each relative pointer. It reads one 32-bit word, then advances 20 bytes and repeats until either a nonzero sampled word is found or 16 zero words have been examined. Six bits per block record are set only for the all-zero-sample case.

The sixteen sample loads begin at component offsets 0, 20, 40, ..., 300. The highest load ends at byte 303, so **304 component bytes** suffice for this specific sampling loop, assuming the beginning address is valid. This does not supersede the separately inspected planar composition consumer, which accesses through byte 319 and therefore needs **320 bytes** for that path.

The source does **not** check for a zero relative component pointer before sampling: a zero value results in reads from the base of the h_blocks allocation. Whether this is intentional sentinel handling cannot be resolved from these instructions alone. The init loop itself does not verify pointer bounds or resource-load success.

At syndre.sx:27901–27913, mission resource loading returns to the caller before init_hires_blocks runs; no local check of the count of resource-load failures is visible. The gdata.c function returns a failure count. A partially loaded mission could therefore cause unsafe pointer use; this is a conditional source-level concern, not a reproduced defect.

This report refines the earlier 'empty component flags' terminology: the computed bits mean sixteen specific *sampled words* are zero, not necessarily that the complete graphical component is empty, and certainly not that the component is a light.

## Verification ledger

PASS: read original assembly source and loader C source; check 16 sampled 32-bit words at stride 20 and the 304/320-byte distinction.
NOT RUN: actual Win32 game, compiled original source, memory sanitizers, fault injection, private component flag calculations, render performance.
NEXT: compare the exact six-bit flag computation against the existing decoded private hblk resource (offline), then investigate whether its observed flag values match block selection and max component list assumptions. Do not alter stable main or publish retail resource bytes.
