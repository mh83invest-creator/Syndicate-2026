# Syndicate 2026 — Main Systems Audit: Sound-Bank Consumer Guards

Date: 2026-11-01 (Australia/Brisbane)
Branch/workstream: `main` / core engine; documentation-only source audit.
Predecessor: `docs/audits/2026-11-01-main-resource-consumer-failure-chain.md`.

## Scope and sources directly read
- `engine/src/sound.c`, particularly `sound_bank_setup` at lines 73–99, `BFPlaySample` at 172–193, and `BFSonundUnkn1` at 195–257.
- `engine/src/sound.h` pointer declarations and sample/status structure.
- `engine/conf/wrappers_game.conf`, where `sound_bank_setup` is a native-C `void` wrapper.
- `engine/bflibrary/src/general/gdata.c` / `bfdata.h` batch-loader return/failure contract.
- Prior GitHub audit `docs/audits/2026-11-01-resource-loader-eight-caller-handover.md` on `feature/v0.9-visual-showcase` documents three sound-bank resource-load-to-setup sites in the shared original assembly blob. The oversized assembly text could not be re-extracted in the current session; those assembly line references are **historical audited evidence**, not freshly read lines.

## Verified findings
**CORE-SND-01 — An actual downstream null guard exists.** `sound_bank_setup` checks `smptable == NULL` and logs/returns before iterating when the sample table was not loaded. It independently checks `!GetSoundAble()`. This means an unchecked upstream `LbDataLoadAll` error count does NOT, by itself, establish that `sound_bank_setup` dereferences a NULL sample-table pointer. Earlier summaries describing *all* consumers as unguarded require this qualification.

**CORE-SND-02 — Guard is not full resource validation.** After those guards, `sound_bank_setup` uses `p_smp = &smptable[1]`, iterates until `smptable_end`, and computes `voc_data = smpdata + (uint)p_smp->data_start`. The function has no visible validation that `smptable_end` denotes a correctly sized table, that `smpdata` is loaded, or that sample offsets reside inside the sample-data allocation. These are conditional risks if partial or malformed resources can reach setup; no bad asset/runtime outcome demonstrated.

**CORE-SND-03 — Playback paths use different gates.** `BFPlaySample` checks `GetSoundAble()` and `GetSoundActive()`, then indexes `smptable[smp_id]` without a separate `smptable` null/extent guard. `BFSonundUnkn1` uses the same two ability/active checks and loops sample identifiers 1–255, referencing `smptable[smp_id]` and `sample_status[smp_id]`; it does not itself check that the loaded sample table contains 256 entries. It remains unverified whether global sound activation state always excludes execution with absent/short sample data. Do NOT classify these as reproduced crashes.

**CORE-SND-04 — No success status from setup.** `sound_bank_setup` returns `void`; when it logs and returns due to a missing table or disabled sound it does not report an error to the assembly caller. This must be considered separately from the native batch loader's numeric failure count.

## Impact / constraints
- Unlike an unguarded map buffer path, sound setup includes a useful NULL-table early return, reducing the specific risk of dereferencing `smptable` *inside setup* when absent.
- Retain existing sound behaviour and original sample pacing; no audio backend replacement, feature change or source patch is authorised by this audit.
- Mission Builder and Graphics Audit remain separate, consulted only if a later shared loader change is proposed. Preserve original/American Revolt resources, 16 FPS tactical simulation, v0.7 baseline and between-mission-only campaign save rules.

## Tests performed
PASS: live GitHub source read of the C functions, wrapper signature, pointer declarations and native loader contract. Findings explicitly distinguish present guards from absent resource-length checks.
NOT PERFORMED: assembly text re-extraction, runtime sound-enabled/disabled tests, short/missing sample resource injection, 32-bit Windows compile, profiling, gameplay tests or code changes.

## Next main-audit checkpoint
1. Trace initialization and lifetime of `GetSoundAble` / `GetSoundActive` and the sample-table `smptable`, `smptable_end`, `smpdata` definitions, including allocation size, before judging playback reachability under partial loads.
2. Reconcile callsites with full extracted assembly when available and record whether any callers verify the batch load count or offer another independent guard.
3. Build isolated, disposable missing/truncated-sample test cases and check Windows runtime behaviour before a repair proposal.
Completed: native sound consumer-guard source inspection and handover. Unverified: runtime crash or safety, executable tests, audio asset integrity.

## Follow-up: audio state and sample-pointer lifetime (2026-11-01)
See successor source audit `docs/audits/2026-11-01-main-audio-state-and-sample-lifetime.md`, commit `cc2d7eca48ca2f8fd3c0aec56e879f1c3059002a` on `main`. It verifies that `GetSoundAble` and `GetSoundActive` return global device-state flags, not game `smptable` pointer/count checks, and that `SoundInstalled` is checked in the downstream playback API **after** `BFPlaySample` has indexed `smptable`. `SetBFSampleStatus` is an additional unguarded direct table consumer. It also separates generic `sound.dat` (`Sfx`/`SfxData`) from game-specific `smptable`/`smpdata`. Static C source evidence only; no crash demonstrated, original assembly allocation/callsite reachability, Windows tests and asset sizes remain unverified. No source modifications.
