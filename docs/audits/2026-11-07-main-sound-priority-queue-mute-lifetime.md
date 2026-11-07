# Syndicate 2026 — Main Systems Audit: Sound Priority Queue and Muted-Event Lifetime

**Date:** 2026-11-07 (Australia/Brisbane)
**Branch:** main — documentation-only
**Status:** static C and wrapper audit; no Windows audio runtime test or explosion event counts.

## Scope and previous evidence

Prior main reports already established tactical/menu sound-bank resource lifetime, native-versus-assembly null guards, and the existence of the tactical `BFSonundUnkn1` call behind a startup gate:
- `docs/audits/2026-11-01-main-audio-failure-reachability-and-guards.md`
- `docs/audits/2026-11-01-main-audio-phase-ordering-and-state-writes.md`
- `docs/audits/2026-11-01-main-explosion-spatial-collision-and-frame-pacing.md`

This report makes **no claim to rediscover those results**. The new investigation is the native queued-event arbitration algorithm and its relationship to mute and mixer requests.

Directly examined:
- `engine/src/sound.c`: `ClearBFSampleStatus`, `SetBFSampleStatus`, `BFSonundUnkn1`, `BFPlaySample`.
- `engine/src/sound.h`: `BFSampleStatus` with two `ubyte` fields; `sample_status[256]` and table pointer declarations.
- `engine/conf/wrappers_game.conf`: mapping of these four functions to native C.
- `engine/bfsoundlib/src/variable.c`: `SoundActive` independent from `SoundAble`; `GetSoundActive` returns that flag.
- `engine/bfsoundlib/src/ssampply.c`: `PlaySampleFromAddress` audio-device flag gate, slot availability and sample-start behaviour.
- `engine/bfsoundlib/src/ssamplst.c`: separate sample-list playback API; do not conflate it with the `sample_status` game priority array.

## CORE-SNDQ-01 — Each queued sample ID has one pending flag and one strength field

`ClearBFSampleStatus` sets both bytes of all 256 entries to zero. `SetBFSampleStatus(id, stat)` checks the sample's metadata priority `field_1E` and rejects metadata entries with value zero. If the entry's `field_0` (pending) is zero, it saves `stat` to `field_1` and sets `field_0=1`. If already pending, only a strictly greater `stat` replaces the stored strength; the flag remains 1. Therefore multiple same-ID requests between scheduler passes coalesce into a **single pending entry**, retaining the largest `stat`, conditional on a valid sample table.

This setter does **not** check `SoundActive`. It dereferences `smptable[id]` without a null or length check as previously reported; muted does not mean safe if the bank is missing.

## CORE-SNDQ-02 — One queued request is selected; other pending requests expire in the inspected pass

Native `BFSonundUnkn1` loops through sample IDs 1..255. It checks `sample_status[id].field_0`; if active, it decrements it, then uses sample metadata priority `field_1E`, tie flag `field_1F` and queued `field_1` to choose `sel_smp_id`. After scanning all IDs it has **one** conditional `PlaySampleFromAddress` call (or the AIL2 VOC equivalent), not one call for each pending entry.

Since the setter writes pending=1, every queued entry scanned during one eligible scheduler invocation is normally decremented to zero, including nonwinners. The selected entry is also cleared after playback request. Losers are not simply deferred for later scheduler passes: one-shot requests are consumed by arbitration. This does not cover new requests arriving concurrently or other setters outside the observed source.

The ordinary non-AIL2 build resets `byte_5BBE8=0` each eligible pass (comment: mixer allows multiple samples); AIL2 has conditional existing-playback priority state. The exact ordering of equal-priority winners depends on the shown comparison logic, metadata and queue-strength fields. Do not claim a universal last-ID or first-ID tie rule.

## CORE-SNDQ-03 — Muted/off-capability scheduler passes do not age queued requests

`BFSonundUnkn1` returns immediately when `GetSoundAble()` or `GetSoundActive()` is false, **before** the loop that decrements pending status. `SetBFSampleStatus` does not test active/mute. Thus if the bank is valid and `SoundActive=false` while game logic successfully queues an event, it can remain pending across muted scheduler invocations. When sound becomes active, the queued request may enter the first eligible arbitration pass. This is a conditional stale-event/re-enable mechanism, not a demonstrated audible bug in real gameplay. Mission transitions also invoke `ClearBFSampleStatus` in already documented paths, preventing a blanket claim that all pending events survive changes of mode or bank.

## CORE-SNDQ-04 — Priority layer differs from mixer capacity

`BFPlaySample(id)` calls playback directly after checking sound-capability and active flags; it bypasses `sample_status` selection. The queued scheduler calls `PlaySampleFromAddress` at most once per invocation, but that audio library API uses existing `SampleInfo` slots and returns NULL if no suitable completed slot is found (or sound is unavailable). Therefore an arbitrated sound **may fail to actually start** even after its status is consumed. The queue is not a guaranteed delivery or retention mechanism.

Other API `PlaySampleList` in `ssamplst.c` has its own eight-item EOS-callback queue and must not be mistaken for this priority algorithm.

## Impact on explosion profiling

Prior assembly review established effect-related sample-status callsites but not exact dynamic counts per explosion. A later approved diagnostic test should record sample-status setter count, distinct IDs queued between eligible passes, coalesced requests, arbitrated winner, dropped nonwinners, muted pending entries, and actual successful starts. Compare to effect simulation cost without changing requested sample priorities or the sound mix. Do not infer audio-mixer overload from C scheduler loop length alone.

## Verification, limits, handover

**Performed:** current native audio source examination and wrapper cross-check; state-machine reasoning of pending flag and selection; separate mixer/list API comparison; predecessor report review.

**Not performed:** full assembly blob reinspection for new dynamic callers, executable symbol verification, Windows build, playback test, sound-bank size test, fault injection, mission profiling, source edits, visual/audio modification or branch merges.

**Protected:** v0.7 baseline, 16 FPS tactical pacing, original sound selection and music, explosion effects and damage, original and American Revolt campaigns, graphics and Mission Builder parallel branches.

**Next checkpoint:** inspect exact audio activation transitions against pending-event lifetime and whether the mixer allocation can cause dropped winner requests; any actual fix or instrumentation needs separately approved diagnostic implementation and runtime controls.
