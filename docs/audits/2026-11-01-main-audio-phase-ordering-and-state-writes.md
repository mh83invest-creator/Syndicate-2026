# Syndicate 2026 — Main Systems Audit: Audio Phase Ordering and State Writes

**Date:** 2026-11-01 (Australia/Brisbane)  
**Scope:** original assembly source-control-flow follow-up for MAIN SYSTEMS AUDIT. No gameplay/source modifications.  
**Predecessors:** `docs/audits/2026-11-01-main-game-sample-table-assembly-lifecycle.md` (commit `6cba1e776b1d8072f4b4f16448a8b62aa801c343`), `docs/audits/2026-11-01-main-audio-partial-load-state-matrix.md` (commit `87ab48b2d374fea7a77111fcf2a5f4f7fd3a06a6`).

## Directly inspected evidence

Read complete `engine/src/syndre.sx` through GitHub blob SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf` (85,803 lines) and inspected source around:
- `GLOBAL_FUNC(syndicate):199–303` and the one direct `ac_BFSonundUnkn1` site at line 234.
- `load_map_level:27901–27933`, `func_25a6f:28483–28550`, `free_map_level:27640–27656`.
- `select:54150–54175` and menu cleanup at `54838–54848`.
- `level_finished:54998–55044`.
- `process_day:11450–11473`.
- `SoundActive` writes at 25874 and 55017.
- `init_level_data` clearing queued sample status by `ac_ClearBFSampleStatus` at line 27809.
Also reread C `engine/src/sound.c`, `engine/bflibrary/src/general/gdata.c`, and `engine/conf/wrappers_game.conf` for native wrapper distinction.

## CORE-PHASE-01 — Main normal-flow ordering has been traced (source verified)

`syndicate` enters `select` (line 206). After a successful menu return it calls `reset_mission_info` (209) then `initialise_player` (210). During its active tactical loop it calls `move_it` (230), and its single `ac_BFSonundUnkn1` call (234) is behind `cmp $0x2,%ebx; jbe`, with `ebx` advanced to no more than 3 at lines 274–276. Upon leaving tactical play the routine calls `free_map_level` (285) before optionally calling `level_finished` (297) and returning to `select`. This ordering is source-confirmed **for the shown control flow**, not proof that all alternate failure/keyboard/network paths execute every stage. No gameplay tick frequency or execution timing was measured.

`load_map_level` is invoked at two direct assembly sites inside `func_25a6f` (28498, 28536), for branches that distinguish multiplayer state. Both follow the source-level tactical sound-bank-0 load/setup logic in `load_map_level`. A complete dynamic call graph back from every `initialise_player` path to this function is not established here.

## CORE-PHASE-02 — SoundActive is independently mutable (source verified)

`SoundActive` is a separate global from `SoundAble` and from sample-table resource validity:
- `process_day:11461–11472` contains an input-triggered toggle (`inc` then `and $1`) and writes the resulting value to `SoundActive`; its explicit polling loop is source visible. Key meaning and user-facing behaviour were not verified.
- `level_finished:55009–55020` writes value 1 to `SoundActive` before the conditional menu bank-1 reload at 55038–55043. This is not evidence that the bank loaded successfully.
- Initialisation path writes `SoundActive` at line 25874, but its upstream register provenance was not fully traced in this checkpoint.

Therefore `SoundActive` cannot be used as a proxy for sample bank integrity. The same is true of `SoundAble` per previous native C/state audits. A legitimate unmuted device can coexist with a failed resource load at the source-contract level; whether execution proceeds to an unsafe consumer remains to be demonstrated.

## CORE-PHASE-03 — Resource return checks and bank transitions remain a joint gate

Each of the three verified `ac_sound_bank_setup` callsites (27911, 54174, 55043) follows a `sound_bank_files0/1` batch load without checking the returned error count. Menu select entry can skip loading the menu bank when `smpdata` is already non-null (54168–54174). Normal tactical exit invokes `free_map_level` before `level_finished`, but its sound free is itself gated by `SoundAble` (27647–27655). These conditions should be tested *together*, including switching between `sound-0` and `sound-1` and resource-load failures, not treated as independent always-safe stages.

The source shows an actual local original-assembly null guard in `ASM_SetBFSampleStatus` that is missing from the native C replacement selected in `wrappers_game.conf`; see predecessor for precise references. No fix is implemented or authorised by this finding.

## Validation and next main audit gate

**Performed:** Full original assembly blob source read, exact control-flow inspection, cross-check against C wrappers, and source-based phase reasoning. The phase claims concern only observed explicit branches, not runtime evidence.

**Not performed:** 32-bit Windows build, gameplay/timing/audio tests, malformed or private retail data inspection, dynamic call tracing, runtime injection, memory profiling, modifications to source/saves, or other branch merges.

**Next checkpoint:** Investigate boundary behaviour when the tactical and menu sound banks partially fail and `SoundAble` is false during cleanup; verify build-selected wrapper linkage, source resource sizes and early `SetBFSampleStatus` reachability with a disposable Windows 32-bit harness. Preserve v0.7, 16 FPS, original/American Revolt and between-mission-only campaign-silo requirements. Coordinate any shared loader patch with Mission Builder and Graphics Audit before implementation.
