# Syndicate 2026 — network exchange error-path audit

2026-11-01. Source only, no changes.

Source: engine/src/syndre.sx Git blob c35e22621a93d1e07c44d40de8867446ff7c64bf, verified lines 33983–34093 and 28106–28213.

1. multi_play first calls ExchangeNetwork_Packet, then processes each slot through process_action, with no immediate test of an exchange return value at the call site.
2. ExchangeNetwork_Packet invokes StartNetwork then walks Network__NumberOfSlots. For local slot, it iterates peers whose Network__Status equals 1 and sends a 14-byte packet using net_send. If the return in AX is nonzero, it marks the peer status zero, resets a field in NetUserNcb and invokes __NETListen__. This is a source-visible send-error handling branch.
3. For a nonlocal slot whose Network__Status equals 1, it repeatedly calls net_receive and jumps backwards as long as signed AX is <=0 (lines 34060–34079). No timeout/event-pumping exit is visible **within this particular receive loop**. The implementation of net_receive, OS blocking and wider control flow must be checked before alleging a guaranteed hang; however this is a concrete potential unbounded-wait risk.
4. ExchangeNetwork_Packet returns after iterating slots; it does not expose an explicit return-status contract in the inspected epilogue. Packet exchange ordering and action interpretation must be maintained for multiplayer.
5. ExchangeNetwork_PlayerInfo, beginning line 34097, separately sends player records and has an explicit error-return path on unsuccessful SendNetwork at 34149–34158. It is distinct from per-frame packet exchange.

Regression criteria: dropped peer, disconnected peer, send failure, empty receive, delayed receive, two and many players, synchronization of level seed/person count, proper input ordering, and no stalling of display/audio servicing. Static audit cannot prove runtime recovery.

No program, workflow or branch changes; no multiplayer execution.
