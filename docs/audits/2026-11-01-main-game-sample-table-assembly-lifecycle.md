# Syndicate 2026 — Main Systems Audit: Original Game Sample-Table Assembly Lifecycle

**Date:** 2026-11-01 (Australia/Brisbane)  
**Branch:** `main`  
**Type:** source-backed core systems audit; documentation ONLY (no code, build, saves or runtime changes).  
**Previous checkpoint:** `docs/audits/2026-11-01-main-audio-state-and-sample-lifetime.md`, commit `cc2d7eca48ca2f8fd3c0aec56e879f1c3059002a`.

## Critical access improvement — large assembly text recovered

The GitHub `fetch_file` path gave empty content for the oversized `engine/src/syndre.sx`, but **GitHub `fetch_blob` by its verified blob SHA succeeded** in this session. The complete source was read: **85,803 assembly lines**, blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`. Exact relevant line spans are now newly inspected directly. Earlier handovers calling assembly inaccessible through the file reader remain historically accurate but are superseded **for this stage** by direct blob access.

Sources cross-checked directly:
- `engine/src/syndre.sx` on `main`, lines noted below.
- `engine/conf/wrappers_game.conf:153–154,248–255`.
- `engine/src/sound.c:73–99,149–169,172–257`; `engine/src/sound.h:36–71`.
- `engine/bflibrary/src/general/gdata.c` and `engine/bflibrary/include/bfdata.h`.

## Verified findings

### CORE-SMP-01 — Shared sample pointer symbols and original bank definitions

`syndre.sx:83887–83892` declares three exported 4-byte pointer slots: `smpdata`, `smptable`, `smptable_end`, not inline audio buffers. The original tactical bank descriptors at `82664–82674` load `sound/sound-0.dat` into `smpdata`, and `sound/sound-0.tab` into `smptable` with `smptable_end` as an end pointer. Menu bank descriptors at `85050–85061` load `sound/sound-1.dat` and `sound/sound-1.tab` into those **same** slots.

Therefore gameplay and menu banks are alternate contents of one shared pointer namespace. Asset format and loaded table sizes are **not verified** by these source declarations. Both file lists use the native `LbDataLoadAll` wrapper in the current build; the generic `sound.dat` bank in `bfsoundlib` is distinct, as established in the predecessor audit.

### CORE-SMP-02 — Loader and cleanup paths confirmed in original assembly

- `load_map_level` at `27901–27912` calls `ac_LbDataLoadAll(mission_load_files)`, tests `SoundAble` and, if set, calls `ac_LbDataLoadAll(sound_bank_files0)` followed by `ac_sound_bank_setup`. **Neither call checks the returned failed-entry count.**
- `free_map_level` at `27640–27656` frees mission files, conditionally calls `StopAllSounds` and `ac_LbDataFreeAll(sound_bank_files0)` when `SoundAble` is nonzero. This establishes a conditional cleanup path, not proof all transitions always execute it.
- `select` at `54156–54175` loads menu assets, tests `SoundAble` and the current value pointed to by the first menu bank descriptor's `smpdata` slot (via `data_5bb2c`); only if that value is zero does it load `sound_bank_files1` and call native `sound_bank_setup`.
- Menu cleanup at `54838–54848` frees menu graphics, and if `SoundAble` is nonzero, stops samples and frees `sound_bank_files1`.
- `level_finished` at `55037–55044` reloads `sound_bank_files1` and runs setup when `SoundAble` is set, without inspecting batch failure status.

Since `sound_bank_files0` and `sound_bank_files1` point to the same exported sample slots, phase-specific ordering and the exact values of `SoundAble` and `smpdata` matter. For example, a non-null `smpdata` causes the observed menu *load* branch to skip rather than proving that menu-bank-1 resources occupy the pointer. A stage transition with a false `SoundAble` can skip the observed sound-bank freeing branch. These are **conditional lifecycle questions**, NOT proof of a stale bank, leak or sound mismatch in normal play. Confirm complete state transitions before proposing a change.

### CORE-SMP-03 — Critical original-assembly versus current-native-C safeguard discrepancy

The original *assembly* routine `ASM_SetBFSampleStatus` at `55704–55716` **explicitly checks `smptable != 0` before dereferencing sample fields**, but still has no evident end-bound check in that local routine. In the actual source port, `engine/conf/wrappers_game.conf:252` maps `SetBFSampleStatus` to **native C**; `engine/src/sound.c:149–169` lacks the same `smptable` null check.

**This is a verified source-level loss of a local null guard in the C replacement, not a proven crash or historical regression in a Windows playtest.** Do not assume the guarded assembly implementation handles the `ac_SetBFSampleStatus` calls in the current native wrapper configuration. Other mapped routines include `sound_bank_setup` (native C, with a null-table check), `BFPlaySample` and `BFSonundUnkn1` (native C, lacking a local table/length check). Establish actual build/wrapper linkage and affected reachability with 32-bit Windows tests before any source patch.

### CORE-SMP-04 — Playback/queued-status consumers and size floor

The newly accessed assembly blob contains:
- **12** direct `call ac_SetBFSampleStatus` sites, including `level_failed` (line 466) and `level_complete` (line 484), as well as effects/draw paths.
- **13** direct `call ac_BFPlaySample` sites, including tactical and menu paths.
- **1** direct `call ac_BFSonundUnkn1` site, at line 234.
- **3** direct `call ac_sound_bank_setup` sites (27911, 54174, 55043).

Counts are *exact lexical direct call sites* in the inspected current blob, not dynamic execution frequency, comprehensive indirect-call analysis or reachability proof.

The native `BFSonundUnkn1` scans sample identifiers 1..255 (`engine/src/sound.c:216–238`). Native `SetBFSampleStatus` accepts an 8-bit ID (0..255) and native `BFPlaySample` similarly indexes its sample table. `BFSample` is 32 bytes in the packed 32-bit game ABI, so a fully addressable table for IDs 0..255 requires **256 × 32 = 8,192 bytes** of contiguous table data. This is an **addressability minimum for these consumers**, NOT a statement of retail `.tab` size, expected format or a verified number of valid samples. The 256-entry `sample_status` array is separately allocated statically in `syndre.sx:85638–85641` and is not a substitute for validating `smptable`.

### CORE-SMP-05 — Native loader does not enforce consumer's sample-count contract

`LbDataLoadAll` continues after per-entry failures and returns a failure count; the assembly callsites above do not immediately inspect it. `LbDataLoad` allocates based on the file/RNC-reported resource length, not a sample-table-specific minimum of 8,192 bytes. `sound_bank_setup` guards a null `smptable` but loops to `smptable_end`, without validating `smpdata` and sample offsets against allocated data. Native playback/status functions do not compare a sample ID to the loaded table's count. **If** input is truncated, omitted or undersized and execution reaches one of these consumers, the local checks may be insufficient; real file sizes and runtime reachability remain untested.

### CORE-SMP-06 — Do not treat original ASM routines as a test of the current C replacements

Original `ASM_sound_bank_setup` (`55637–55678`), `ASM_SetBFSampleStatus` (`55704–55735`), `ASM_BFPlaySample` (`55737–55770`), and `ASM_BFSonundUnkn1` (`55808+`) contain useful legacy control-flow evidence; they are distinct from current native C functions selected by `wrappers_game.conf:248–252`. No claim is made that these old routine bodies execute in the active Windows build.

## Prioritised next core-audit actions

1. Follow loader-path **failure propagation** for sound-0 and sound-1, especially when the `.dat` succeeds but `.tab` fails, when the reverse occurs, and when `SoundAble` changes before cleanup. The shared pointer slots and native batch-free semantics must be accounted for.
2. Check for pre-bank `SetBFSampleStatus` invocations and identify stable ordering from game/menu init through the first simulation/effect/audio tick, including failures; only then rank severity.
3. On an isolated lawful asset copy, measure actual decompressed `sound-0.tab`, `sound-1.tab` sizes and verify 32-byte descriptor counts, end pointers, and data offsets; do not upload copyrighted sounds.
4. Use a nonproduction Windows 32-bit test harness or disposable installation to test missing `.tab`, missing `.dat`, truncated headers and injected allocator failure. Preserve original sound effects, 16 FPS tactical pacing and the known-good v0.7 baseline.
5. Any proposed shared-loader patch must first be reviewed against Mission Builder and Graphics Audit requirements. Never alter their branches in this audit.

## Verification performed and not performed

**Performed:** GitHub live full assembly blob read and exact line inspection; four exact lexical direct-call counts; read native wrapper config and source; arithmetic derivation of 8,192-byte table addressability minimum; source-only findings and documented handover.  
**Not performed:** compiled wrapper linkage verification, Windows build, game launch, retail sound data parsing, media playback, memory instrumentation, runtime/negative-resource fault reproduction, runtime performance, gameplay, saves or code change.  
**Changed:** this documentation report only; separate handover-index updates are subsequent commits.  
**Baseline:** protected v0.7 untouched; Mission Builder and Graphics Audit remain independently owned; no mid-mission saving, campaign save isolation remains a future requirement.
