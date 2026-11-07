# Syndicate 2026 — Main Systems Audit: Mixer Slot Reuse, Playback Rejection and Muted Cleanup

Date: 2026-11-07 (Australia/Brisbane)
Branch: main; documentation-only.
Status: source-level verification; no Windows sound test, ABI check or observed live bug.

## Scope and evidence
Predecessor: `docs/audits/2026-11-07-main-sound-priority-queue-mute-lifetime.md` (native game priority arbitration). Earlier main audio reports already covered menu/tactical bank transitions and missing table guards; these are not reclassified as new discoveries.

Read live GitHub files:
- `engine/bfsoundlib/src/ssampply.c` blob `6fa62ba0b0ba83ad50b7c8b692deddeeff9a8ccc`.
- `engine/bfsoundlib/src/variable.c` blob `f3e3cbf5e6fc2667a0b662261a674b463562b400`.
- `engine/bfsoundlib/src/ssamplst.c` blob `9bd4b13d45b72bcd10118492c629d442ff6fee29`.
- `engine/src/sound.c` blob `d2f5946d11e11cb123846ee5e54ee4065bf1f07d`.
- `engine/src/sound.h`, `engine/bfsoundlib/include/ssampply.h` and previous sound priority audit.

## CORE-MIX-01 — Playback depends on a completed sample slot

In `PlaySampleFromAddress`:
- The function first requires `SoundInstalled`, `SoundAble` and `SoundActive`.
- For `a7=1`, it picks the **first** `SampleInfo` whose `AIL_sample_status(handle)==SNDSMP_DONE`, scanning from `sample_id` through `end_sample_id`.
- For `a7=2`, it suppresses duplicate in-progress source/sample requests and otherwise seeks a done slot.
- For `a7=3`, it preferentially reuses a matching source/sample slot; otherwise it seeks a done slot.
- A missing slot or deliberately skipped repeat returns `NULL`.

The tactical scheduler `BFSonundUnkn1` and direct `BFPlaySample` both pass `a7=1` on the non-AIL2 path. Thus neither path forcibly pre-empts an in-progress sample when all available slots are busy. The scheduler clears its chosen pending status after calling playback, without examining whether `PlaySampleFromAddress` returned `NULL`. **Arbitration winner does not guarantee audible sample start, and failure is not deferred.** Whether/when a slot is done depends on the audio runtime; there are no measured counts.

The static backing array is declared `sample_id[32]` in `variable.c`. This is **not evidence that all 32 are active**: the scanned limit is `end_sample_id` and the default game option `audOpts.MaxSamples=10`; the effective slot count and initialization of the end pointer need a separate trace.

## CORE-MIX-02 — Null audio address is checked AFTER sample-handle initialisation

After selecting a slot, `PlaySampleFromAddress` invokes `AIL_init_sample(p_smpinf->SampleHandle)`, then tests `if (address == NULL) return NULL;`. As a result, null-address failure is not guaranteed side-effect-free; sample metadata assignment occurs later and no AIL start follows on this path. Whether resetting an already completed handle without audio data affects slot availability must be tested. This is a conditional API-ordering risk, not observed damage.

## CORE-MIX-03 — Muted cleanup does not execute the stop/reset path

`StopAllSamples` returns if any of `SoundInstalled`, `SoundAble`, or `SoundActive` is false. On its successful path it stops fade timers, ends every sample, zeros source/sample/fade bookkeeping, then calls `StopSampleQueueList`. During a mute state (`SoundActive=false`), this function does not perform those operations. The higher-level game priority scheduler also does not consume queued statuses while muted, as the preceding audit established. These are two **distinct** state lifetimes: pending priority requests and already allocated mixer handles.

The source does not prove that `StopAllSamples` is actually invoked during muted gameplay or that the device continues producing audible samples. Avoid claiming an observed mute bug. It does establish that **the stop call alone cannot be treated as unconditional resource cleanup**.

## CORE-MIX-04 — Separate sample-list queue is not the game's priority queue

`ssamplst.c` implements a dedicated `PlaySampleList` with up to eight IDs using an EOS callback on one queue handle. `StopSampleQueueList` stops and clears its user-data values; it is invoked as part of `StopAllSamples` only after the above ability/active guards. This is independent of the game's 256-entry `sample_status` array and should receive separate counters if diagnostic instrumentation is eventually approved.

## Proposed diagnostic gates (NOT implemented)

1. Confirm the exact Windows mixer library and where `end_sample_id` is assigned; measure actual active slot count rather than treating `sample_id[32]` as capacity.
2. Count per-tick event setters, selected winners, mixer starts versus NULL returns, free/done slots, active handles and mute transitions; keep audio logic and order unchanged.
3. Verify `SoundActive` toggle and resource cleanup with explicit no-audio, mute-during-sample, unmute, level exit, and no-slot-available scenarios in a disposable Windows diagnostic build.
4. Compare identical explosion scenarios against audio-disabled configuration without altering tactical effect counts, enemy behaviour, sound priority or original effects. Control for probe overhead.
5. Any change to stopping/muting/sample scheduling is gameplay/audio behaviour change and requires separate approval and cross-workstream review.

## Verification and constraints
Performed: current library and native game source reads, branch-condition tracing, distinctions between priority events and mixer slots.
Not performed: Windows compile, CI, runtime audio playback, debugger, active slot count determination, fault injection, profiler run, source edits, original asset inspection or branches changed.
Protected: v0.7 reference, 16 FPS tactical updates, original game sounds/music, explosion counts/visuals/damage, original and American Revolt content and all user saves.
Next source checkpoint: locate `end_sample_id` initialization and audio state transitions from actual call sites; investigate precise sample-list callback lifetime. Do not claim fully exhaustive sound audit without those checks.
