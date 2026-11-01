# Syndicate 2026 — Main Systems: deep behavioural contracts

Date: 2026-11-01 (Brisbane). Source-only; no game changes.

## Direct source
`engine/src/syndre.sx` full Git blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`, freshly inspected assembly lines 28450–28495; 28115–28147; 7531–7572; 11540–11645; 620–680; 55130–55245. Prior integrated architecture report: `docs/audits/2026-11-01-main-consolidated-holistic-engine-architecture-audit.md`.

## Verified expanded contracts

### Player action dispatch
`process_action:28450–28480` derives offsets into `packets` and `players` based on indexed slot, reads an action code at offset 0x0d of the packet, guards values above 0x3a, and jumps through `jpt_25A67` (indirect dispatch). Therefore action semantics comprise a dispatch table and multiple handler branches, not a single operation. The earlier `single_play` loop calls `process_action` eight times per update for indices 0–7; this pass did not enumerate every table target and handler effect.

### Multiplayer sequence
`multi_play:28115–28144` populates per-slot fields including `level__Seed` and person count, calls `NetworkPlayersCount` and `ExchangeNetwork_Packet`, then loops `process_action` for `Network__NumberOfSlots`. Network packet exchange precedes action processing in this path. This confirms coordination between network state, simulation inputs, and per-slot action order; correctness under latency, divergent state, and packet loss is not established by static code.

### Computer AI
`process_computer_players:7493–7572` uses interval-gated processing; its nested loop computes a `computer_players` entry and calls `CPDoTurn` with five stack arguments at 7554 unless a tested state byte matches 0x0d or 0x05. The C model of AI is not standalone: meaningful decision logic resides in assembly `CPDoTurn`, whose complete branches still require coverage. This confirms a per-computer/per-team iteration and a conditional turn dispatch.

### Research, economic simulation and map AI
`process_day:11540–11645` derives elapsed/day state from `players` and `game_speed`; conditionally calls `DoResearch` and updates the exported `research` object, increments calendar-like fields, iterates indexed areas calling `AdjustStateHappiness` and `GetRevenueFromCity`, adds revenue to a player record, draws random values and calls `ComputerPlaysMap`. This establishes a broad **time-driven campaign update** coupling research, happiness, revenue, random state and strategic map AI. It does not establish the exact chronological rate or all formulas. Changing game-speed or call cadence can change campaign economics and research even if tactical output looks fine.

### Objective and defeat logic
`check_end_level:620–680` traverses relevant agent records, compares counter state, can invoke `level_failed`, then reads objective type fields and dispatches through `vtable_10550` with an explicit upper-bound guard. Objective results have multiple handler paths; do not substitute a simple success flag or merge objective types without a full case matrix.

### Mission outcome / campaign reset
`level_finished:55130–55245` checks mission-state bits; selects end-of-game animation names, loops across 0x12 entries of a campaign-related table, and in a loss path invokes `set_default_player`. It also restores UI interaction state and stops MIDI. Along with previously verified changes to `country_states` and sound/display setup, outcome handling is a cross-system state transition with possible broad campaign reset. Exact meanings of all bits, records and outcomes remain to be independently established.

## Critical dependencies
Tactical action processing → network state/AI decisions → simulation/order → objective dispatch → country ownership and campaign reset; elapsed game time → research, revenue, happiness and strategic computer turns. These relationships make FPS, game-speed and save changes especially sensitive.

## Status
Verified source control flow and call relationships as described. Not an exhaustive instruction-level analysis or tested runtime specification. No GitHub Actions runs, game changes, Windows compile, multiplayer playtesting or measurable performance observations. Protected v0.7 and other branches unchanged.
