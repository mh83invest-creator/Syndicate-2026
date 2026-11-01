# Syndicate 2026 — MAIN holistic audit handover

Date: 2026-11-01. User instruction: understand the whole game holistically before modifying anything; avoid repeated tiny 'continue' checkpoints.

## Deliverable
Consolidated audit `docs/audits/2026-11-01-main-consolidated-holistic-engine-architecture-audit.md` committed on `main` at `59d60993d16b73ef489176dfa11ebb41f055c5de`.

Reviewed source across C startup, event/pacing, display, sound, input, mouse scaling, player/game data, research, weapons, platform paths and full `syndre.sx` assembly symbol entry points. Consolidated lifecycle and ownership map: C setup → assembly selection/mission initialization → tactical player/AI/physics/effects/render/update → mission cleanup/transfer/progression → menu/campaign saves. Important confirmed save hazard: `save_player` reduces high credit balances before testing file-open success, with fixed-size binary campaign record order; runtime failure not reproduced. Mission states and audio/display/event dependencies are cross-cutting. The 16-FPS tactical and 30-FPS menu behaviour, original effects/audio, campaign separation, and protected v0.7 remain sacred.

## Completion honesty
This is a substantial **architecture map**, not exhaustive instruction-by-instruction verification of the whole 85k-line assembly. Needs deeper static tracing of mission transition branches, save call sites, data layouts, AI/network/UI/asset ownership, and runtime validation before modifications. Prior detailed OpenAL, collision and ABI audits remain separately available. No engine changes, Windows build, profiling or gameplay test.

## Next work (no repeated approvals for documentation)
Conduct extended subsystem deep dives as large coherent batches, consolidate evidence and a dependency/regression matrix; only seek approval before actual program/source/workflow changes, not for continued audit. Do not modify separate Graphics Audit or Mission Builder.
