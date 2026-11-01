# Syndicate 2026 — Main Systems Audit: Audio State, Sample-Lifetime and Playback Gates

**Date:** 2026-11-01 (Australia/Brisbane)  
**Workstream:** MAIN SYSTEMS AUDIT on `main`  
**Status:** Static source inspection only; no engine modification, build, audio playback or gameplay test.  
**Predecessor:** `docs/audits/2026-11-01-main-sound-bank-consumer-guards.md`.

## Purpose

Complete the next requested main-engine checkpoint: distinguish audio device availability, sample-bank readiness, per-mission sound tables and actual playback call guards. Avoid conflating native Bullfrog sound-library `sound.dat` bank buffers with the original game's `smptable` / `smpdata` fields. All findings are conditional source observations, not demonstrated Windows crashes.

## Source evidence freshly inspected in GitHub (`main`)

- `engine/bfsoundlib/src/variable.c:32-82` — sound state defaults and getters.
- `engine/bfsoundlib/src/bfaudio.c:45-57,118-163,187-209` — `SoundInstalled`, `SoundAble`, options and initialisation.
- `engine/bfsoundlib/src/init_snd.c:49-59,173-219,260-282,356-460` — allocation, driver/sample lifecycle and failure handling.
- `engine/bfsoundlib/src/ssamplst.c:126-257` — separate generic `sound.dat` bank parsing, `Sfx`/`SfxData`, `LoadSounds`.
- `engine/bfsoundlib/src/ssampply.c:114-189` — final playback API checks.
- `engine/src/sound.c:73-99,102-169,172-257` — application sample table setup, status, play and priority selection.
- `engine/src/sound.h:36-71` — packed `BFSample` fields, `sample_status[256]`, game `smptable`, `smptable_end`, `smpdata` declarations.
- `engine/bfsoundlib/Makefile.am`, `engine/conf/wrappers_game.conf` — participating native source and wrapper mapping.

**Access limitation:** `engine/src/syndre.sx` is identified by blob SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf`, but GitHub file-content requests in this session returned empty text for this oversized assembly source. Prior assembly callsite reports exist; no new claim of complete assembly search or successful source extraction is made. This pass cannot establish exactly where and how the original game's `smptable`/`smpdata` are allocated or freed.

## Findings

### CORE-AUD-01 — Global getter flags are not sample-table validity checks (confirmed)

`variable.c` defines `SoundActive = true`, and `bfaudio.c` defines `SoundAble = true`, `SoundInstalled = false` initially. `GetSoundAble()` returns `SoundAble`; `GetSoundActive()` returns `SoundActive` with no check of game `smptable`, its end pointer, sample count or `smpdata`. These flags are not equivalent to a successful original game sample-table load. Device/driver initialization can change the flags; their presence alone cannot validate game sample pointers.

### CORE-AUD-02 — Audio init can be independent of loading generic sound.dat banks (confirmed, careful scope)

`engine/src/sound.c:102–115` sets `audOpts.DisableLoadSounds = 1` by default. `InitAudio` passes this to `DisableLoadSounds`; `InitSound` then skips `DetermineSoundType()` when that option is set but can still initialise an audio driver, allocate playback handles and set `SoundInstalled = 1`. The *separate* `LoadSounds` implementation in `ssamplst.c` declines bank loading when `DisableLoadSounds` is true.

**Do not infer that the original Syndicate per-mission `smptable` and `smpdata` are automatically disabled**: those are game-layer pointers declared in `engine/src/sound.h`, distinct from generic Bullfrog-library `Sfx`, `SfxData` and `EndSfxs` allocated from `sound.dat`. Source of game-layer pointers remains pending original assembly/descriptor cross-check.

### CORE-AUD-03 — Final playback API is guarded, but game-layer indexing occurs first (confirmed)

`engine/bfsoundlib/src/ssampply.c:136–145` returns NULL from `PlaySampleFromAddress` when `!SoundInstalled || !SoundAble || !SoundActive`. It also checks the supplied `address == NULL` at line 170, *after* initialising a sample handle. This is a meaningful downstream guard.

However, application `BFPlaySample` at `engine/src/sound.c:172–191` gates only `GetSoundAble()` and `GetSoundActive()`, then evaluates `&smptable[smp_id]` and `p_smp->data_start` *before* the downstream API can reject the address or test `SoundInstalled`. A downstream guard cannot retrospectively protect an earlier failed table dereference. Sound hardware and original game table readiness are separate acceptance conditions.

### CORE-AUD-04 — SetBFSampleStatus has no local availability/extent guard (confirmed)

`engine/src/sound.c:149–169` takes an 8-bit sample identifier (so index range is 0..255), then immediately evaluates `&smptable[smp_id]` and reads `p_smp->field_1E`. Unlike `sound_bank_setup`, it does **not** locally check `smptable` or sound state. `sample_status[256]` itself has a matching 256-entry declared extent, but the **loaded game sample-table extent** is not established. This is a potentially earlier unsafe consumer under missing/truncated resource conditions; actual callsite reachability under failure remains unverified.

### CORE-AUD-05 — The status-selection loop assumes a full sample table (confirmed)

`BFSonundUnkn1` in `sound.c:195–257` tests the sound-able/active flags and then iterates identifiers 1 through 255; it indexes `smptable[smp_id]`. `ClearBFSampleStatus` clears all 256 status slots in its static array, which is distinct from validating `smptable`. No local null or length check on the game sample table appears in the priority loop. Whether the routine runs after a failed load or whether all entries are valid in retail resources is not yet known.

### CORE-AUD-06 — One audio-driver failure return does not explicitly clear SoundAble/Active (conditional source concern)

`InitSound` in `init_snd.c:405–422` checks whether platform driver setup returned 1 and, on other returns, logs and returns without visibly setting `SoundAble` or `SoundActive` false on that specific path. Other paths (for example, absent SoundType and failed bit/rate selection) explicitly clear these flags, and `DoFreeSound` clears them. Consequently, a failed driver initialisation should not be assumed to make the **game-layer** `GetSoundAble`/`GetSoundActive` checks fail. The final `PlaySampleFromAddress` still checks `SoundInstalled`, but game table dereferencing is earlier. The reachable state must be checked in a disposable Windows test, not asserted from a single branch.

### CORE-AUD-07 — Generic sound.dat parser has distinct read-length/extent questions (observed; separate test gate)

`AllocateSoundBankMemory` uses a footer of `sound.dat` to select `DatSize` and `TabSize` and allocate `SfxData`/`Sfx`; `load_sound_bank` reads bank headers and data/metadata with `LbFileRead` but does not visibly insist that each read returned the requested count or that per-bank declared extents fit those allocations. It calls `UnpackM1` without inspecting its return result, and `format_sounds` relocates data pointers without checking their offsets against buffer extents. These are *separate library bank-input validation concerns*, not proof that the original mission bank is malformed. Treat them as a distinct synthetic-corpus and extent-verification task before proposing shared loader fixes.

## Important correction to earlier risk statements

There are **real guards** in `sound_bank_setup` (NULL game table + sound-able status), and in `PlaySampleFromAddress` (sound installed/able/active + provided address). Other game-layer functions `SetBFSampleStatus`, `BFPlaySample`, and `BFSonundUnkn1` do not validate game table pointer + extent before indexing. The accurate conclusion is a *conditional chain of incomplete validation*, NOT an unconditional dereference or observed crash.

## Next priority and acceptance gates

1. **Main audit next:** Obtain reliable original assembly text (or a local working source checkout) to find definitions, allocation, loading, freeing, zeroing and callers of `smptable`, `smptable_end`, `smpdata`, and sample-status update / `SetBFSampleStatus` and `BFPlaySample`. Record whether error branches can enter the consumers.
2. Check declared retail game sample table length and each `BFSample` data offset against actual allocated resource extents on a lawful, isolated copy. `BFSample` is 32 bytes in the packed 32-bit ABI, but no original table count has been proven.
3. Separate test cases: (a) sound driver disabled, (b) no game mission sample table, (c) truncated/invalid table, (d) invalid `sound.dat`, (e) sample status queued before load, (f) sample table freed before replay. Instrument before/after with 32-bit Windows runtime, without using live saves or retail installs.
4. Before proposing a fix, coordinate any *shared resource-loader* interface with Mission Builder and Graphics Audit; neither workstream's source or files should be modified by this audit.

## Verification / change control

**Performed:** Live GitHub C/header/Makefile reads, function/call-boundary inspection and interpretation with the existing handover; documentation commit and subsequent read-back to be recorded at end of task.  
**Not performed:** Full assembly retrieval, complete repo grep, compilation, CI execution, Windows runtime, asset decoding, audio audition, sanitiser/fuzzer execution or gameplay regression.  
**Code changed:** none. **Saved game files changed:** none. **Concurrent workstreams changed:** none. **Protected v0.7 baseline:** untouched.

No runtime vulnerability or gameplay defect is claimed as reproduced.
