# Syndicate 2026 — Main Systems Audit: Resource-Consumer Failure Chain

Date: 2026-11-01 (Australia/Brisbane)
Workstream: MAIN SYSTEMS / CORE ENGINE, on `main`.
Status: source-level review and handover only. No implementation or executable verification.

## Baseline and scope
- Preserve protected Windows-playtested v0.7 behaviour, original/American Revolt campaigns, original tactical 16 FPS, and mouse/keyboard input.
- Save mechanics are between-mission only; campaign save isolation remains a requirement, **not an implemented feature**. No mid-mission saving, Escape save or auto tactical checkpoints.
- This document is **not** a Graphics Audit or Mission Builder audit. Cross-workstream resource contracts are referenced only where engine-level integration requires it.
- Retail assets, original saves and executable/source code were not modified.

## Evidence freshly read on main
- `engine/bflibrary/src/general/gdata.c` (native `LbDataFree`, `LbDataLoad`, `LbDataLoadAll`).
- `engine/bflibrary/src/general/gfile.c` (`LbFileLengthRnc`, `LbFileLoadAt`).
- `engine/bflibrary/src/general/gsprite.c` (`LbSpriteSetup`, `LbSpriteSetupAll`).
- `engine/bflibrary/include/bfdata.h` (packed 44-byte `TbLoadFiles`, return contract for `LbDataLoadAll`).
- `engine/bflibrary/include/bfsprite.h` (sprite and setup-list pointer fields).
- `engine/conf/wrappers_game.conf` (C mappings for `LbDataLoadAll`, `LbDataFreeAll`, `LbSpriteSetup`, `sound_bank_setup`).
- Assembly blob identity for `engine/src/syndre.sx` on main is `c35e22621a93d1e07c44d40de8867446ff7c64bf`. This session's GitHub file reader returned **empty text** for the oversized assembly file, including line-range requests. The assembly call-chain observations below are therefore taken from *existing GitHub audit handovers for that same blob*, not claimed as fresh assembly line reads.

## Findings and confidence
**CORE-RC-01 — Confirmed native batch contract.** `LbDataLoadAll` begins by freeing the list, then attempts each item and accumulates `-100` allocation or `-101` load failures. It proceeds to subsequent entries and returns the number failed; success is zero. The return contract is explicit in `bfdata.h:55–60`. Consequently a failed batch can leave a partially populated resource set rather than an atomic success or automatic rollback. Source confirmed; no runtime outcome claimed.

**CORE-RC-02 — Confirmed failure cleanup gap.** In `gdata.c`, if `LbFileLoadAt` returns a count unequal to `SLength`, the allocated `Start` pointer is set to 0 (and `SEnd` / `SLength` reset) without a preceding `LbMemoryFree` of the allocation made in that call. This is a conditional lost-allocation / leak path when the load fails after allocation; allocation size and runtime frequency have not been measured. The first `LbDataFree` on a later call cannot recover the discarded pointer. The separate allocation-failure branch returns `-100` and does not carry this same post-allocation leak claim.

**CORE-RC-03 — Confirmed sprite setup does not validate pointee buffers.** `LbSpriteSetupAll` checks that the **addresses of the Start and End pointer slots** are non-NULL, but it does not check the pointer *values* `*Start`, `*End`, `*Data` or the allocated table range before delegating. `LbSpriteSetup` traverses `sprt < end` and accesses `sprt->Data`, with no explicit NULL, count or allocation-extent check. Invalid or partially loaded sprite tables therefore present a conditional unsafe access risk, not an observed crash. Do not infer an end-to-end renderer failure solely from this C function.

**CORE-RC-04 — Confirmed file read-size weakness in current native wrapper.** `gfile.c:31–55` accepts any read result other than `-1` for the 18-byte RNC header; `gfile.c:58–90` likewise does not compare the actual read count with its expected value before `UnpackM1`. For RNC files the expected size originates from the *declared unpacked* length, not independently verified physical compressed input length. Related unpack buffer-size/NULL-check-order concerns are detailed in earlier main audit reports; no duplicate fix or exploit claim is made here.

**CORE-RC-05 — Previously verified assembly consumption, not newly extracted.** Prior main/feature audit documents for assembly blob `c35e22621a93d1e07c44d40de8867446ff7c64bf` list eight direct `ac_LbDataLoadAll` callsites and no immediate return-count checks. These feed mission, graphics, menu, world map and sound setup. Previous follow-ups trace `init_level_data -> init_map_data` and sprite consumers without a visible local buffer validation in the inspected paths. The relevant existing handovers are:
- `docs/audits/2026-11-01-resource-loader-eight-caller-handover.md`
- `docs/audits/2026-11-01-partial-resource-consumer-audit.md`
- `docs/audits/2026-11-01-map-consumer-null-guard-audit.md`
- `docs/audits/2026-11-01-sprite-consumer-safeguards.md`
These are currently on `feature/v0.9-visual-showcase`, NOT automatically merged into `main`. Their evidence cannot substitute for a direct current assembly extraction or Windows test.

## Risk prioritisation
1. **Priority for controlled reproduction:** failed/truncated native resource load followed by map or sprite consumer execution; distinguish missing file, short nonnegative read, corrupted RNC, and allocation failure.
2. **Memory-lifetime check:** post-allocation load failure loses its memory pointer before cleanup, a conditional leak. Use a disposable test environment and memory instrumentation.
3. **Shared API gate:** establish physical compressed input bytes, declared output length, destination capacity, pointer validity and error propagation before proposing any engine changes. Be mindful of mission-briefer's distinct direct RNC path and Mission Builder resource constraints.

No gameplay balance, render changes, campaign selection, save path, or source patch is authorised or attempted by this audit.

## Verification performed
PASS: GitHub live read of the five native C/header files and wrapper config; compared native behaviour against earlier source-only audit notes; confirmed main assembly blob identity (not its text) via GitHub metadata.
NOT PERFORMED: full assembly re-extraction this session, repository-wide independent grep, 32-bit Windows compilation, unit/instrumented test harness, malformed-asset reproduction, actual game launch, performance profiling, rendering test, save operation, or branch merge.

## Handover to next Main Systems Audit checkpoint
1. Obtain a reliable complete `syndre.sx` source text extract or local checkout; do not treat empty GitHub line-range responses as negative evidence.
2. Audit *consumer-by-consumer* error exits and pointer checks for the eight batch-loader callsites, beginning with mission map, menu sprite tables, world-map rendering and sound-bank setup; record exact branch conditions and only then rank crash likelihood.
3. Design nonproduction synthetic error tests, then run 32-bit Windows builds and real-resource regressions on copies before considering any fix.
4. Keep Graphics Audit and Mission Builder handovers separate; send any shared resource contract changes for review, with no silent branch merge.

Completed: native caller/failure-path source cross-check and documentation.
Incomplete: runtime validation, full current assembly consumer enumeration, any code change.
Next: source extraction / consumer-by-consumer guard verification.
