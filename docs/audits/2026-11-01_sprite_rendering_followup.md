# Graphics sprite rendering follow-up

Date: 2026-11-01. Documentation-only review.

Source inspection: engine/src/display.c lines 244-275 selects rendering paths by display mode and provides an MCGA sprite adapter. engine/bflibrary/src/general/gspr_map.c lines 418-435 selects distinct remapping paths for transparent, flipped and ordinary sprites.

The 10,812-byte decompressed sprite table reported in the earlier resource inventory has not been structurally mapped to assembly references. Requests for portions of the large assembly file returned empty text. This is a tooling limitation, not evidence of absent code.

Next: establish sprite table entry layout using a local or smaller assembly extract, then cross-check private decoded assets. No game code or copyrighted art modified. No build, runtime or image tests performed.
