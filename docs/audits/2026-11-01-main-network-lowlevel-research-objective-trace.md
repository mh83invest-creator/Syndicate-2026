# Syndicate 2026 — consolidated deeper network, research and objective trace

Date: 2026-11-01. Main systems source audit. No engine modifications.

Directly inspected engine/src/syndre.sx Git blob c35e22621a93d1e07c44d40de8867446ff7c64bf, lines 35733–35990, 33983–34093, 51948–52028, 37914–37960, 473–510, 7604–7990, 28106–28213 and objective handlers 640–1130.

## Networking
The per-tick ExchangeNetwork_Packet routine sends a 14-byte packet to eligible peers and treats nonzero net_send result as failure, marking peer status zero (lines 34003–34049). On receive, it repeats while returned AX is <= 0 without a timeout or SDL-event pump visible in that loop (34060–34079). The lower-level net_receive (35807–35890) invokes __NETBios__, waits for status byte at NCB offset 0x31 to change from 0xff in its synchronous path, copies the response and returns positive length on success or a negative error code. It also has an alternate immediate return when a stack argument is nonzero. The net_send routine (35733–35803) invokes __NETBios__ and similarly spins on NCB status 0x31 until it changes. These nested wait loops are concrete source-level unbounded-wait risks; whether the underlying NETBios implementation enforces timeouts or OS-level scheduling is not established. This is not a runtime-confirmed hang.

## Research
DoResearch begins by checking enabled selection data_55595 and current selection index data_55597, computes a cost from data_5b95c and data_5559b, compares with the current player's money and deducts that cost if sufficient (51948–51983). It then traverses indexed research-status tables. The source confirms that research progression has a direct funds dependency, and process_day invokes DoResearch during its elapsed-time progression. Full completion formulas and item unlocking remain to be decoded; do not claim those were exhaustively verified.

## Mission outcome
level_complete (473 onward) gates on mission-state bits, calls set_mission_complete, sets completion bit 0x2 in data_60afc and schedules a sound status; the companion level_failed routine sets bit 0x4, with other side effects. The set_mission_complete and set_mission_failed functions at 37914–37945 populate translated GUI panel strings, not all post-mission state. check_end_level dispatches objective categories via vtable_10550 and in other paths vtable_10594; mission-specific case handling is still incomplete.

## AI
CPDoTurn is objective-state-driven and has branches for combat target visibility and fire, relaxation, and new objective setup; prior detail is in docs/audits/2026-11-01-main-decision-branch-deep-trace-v2.md. No runtime behaviour tested.

## Implications and remaining limits
Potential multiplayer indefinite waiting now has stronger nested-call evidence. Campaign research cost, mission-result flags, and objective states have cross-system implications for pacing, save integrity and UI. A full semantic truth table for every mission type and AI state, and network runtime failure tests, are still missing. Static audit only; no build, benchmark, patch, or branch change to Graphics/Mission Builder.
