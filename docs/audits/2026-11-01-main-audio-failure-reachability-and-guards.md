# Syndicate 2026 — Main Systems Audit: Audio Failure Reachability and Guard Cross-Check

Date: 2026-11-01 (Australia/Brisbane)
Branch: `main`
Status: source-only review and test design. No source modifications or runtime claims.
Predecessors: `2026-11-01-main-audio-partial-load-state-matrix.md` and `2026-11-01-main-audio-phase-ordering-and-state-writes.md`.

## Direct evidence and audit method

Inspected entire 85,803-line `engine/src/syndre.sx` via GitHub `fetch_blob`, SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf`, and compared with the actual `main` C sources:
- `engine/conf/wrappers_game.conf:245–255` and `engine/src/Makefile.am:44–47,74–91`, plus `engine/util/mkwrappers`;
- `engine/src/sound.c:73–99,149–169,172–257` and `engine/src/sound.h`;
- `engine/bflibrary/src/general/gdata.c:31–130`;
- `engine/bfsoundlib/src/variable.c:60–83`, `engine/bfsoundlib/src/ssampply.c:136–173`.
The wrapper config and build generation rules support the native C dispatch interpretation. Compiled binary symbol/relocation inspection has NOT been performed.

## MAIN-AUD-01 — Menu sound effect before a verified audio-bank success status

In original assembly `select` (`syndre.sx:54156–54175`), if `SoundAble` is set and `smpdata` is null, the menu loads `sound_bank_files1` then invokes `ac_sound_bank_setup`. Neither call checks `ac_LbDataLoadAll`'s returned failed-entry count. **A non-null `smpdata` alone prevents the initial menu bank reload; it does not verify `smptable`, `smptable_end` or which bank occupies the pointers.** The list's `.dat` and `.tab` descriptors use shared pointer slots also used by the tactical bank (`82664–82674`, `85050–85061`).

After this menu setup, assembly contains direct `ac_BFPlaySample` calls at 54319, 54345, 54507, 54556, and 54609 on observed animation/selection paths. These caller excerpts have no **local** post-load table check. The current native C `BFPlaySample` (`sound.c:172–191`) checks only `GetSoundAble()` and `GetSoundActive()` *before dereferencing* `smptable[smp_id]`. Because `SoundAble` is an audio capability flag, **if** the `.tab` load fails while `SoundAble` remains true and the menu user reaches one of these paths, the inspected C path has no local protection against a missing table. This is conditional reachability supported by the source, **not a reproduced crash**.

## MAIN-AUD-02 — Tactical status events can precede playback validation

Assembly `level_failed` (450–469) and `level_complete` (473–489) call `ac_SetBFSampleStatus` without an adjacent sound-bank readiness check. Additional direct caller examples include effect-related code around lines 17146, 18253, 29816. The tactical `load_map_level` (`27901–27930`) continues toward `init_level_data` even if `sound_bank_files0` returns a nonzero failure count; the count is not checked in that branch. `init_level_data` resets the fixed 256-item status array with `ac_ClearBFSampleStatus` at 27809, but that does not validate the dynamic sample-table pointers. Thus a *status setter* is an earlier consumer at risk, before any `PlaySampleFromAddress` playback guard could apply.

The original assembly `ASM_SetBFSampleStatus` has a null check at `55711–55713`. The wrapper config maps `SetBFSampleStatus` to native C; `sound.c:149–169` does **not** carry that null check or a table-length check. This is a specific, source-confirmed safeguard discrepancy; do not ascribe runtime faults to it without a compiled/runtime test.

## MAIN-AUD-03 — Native priority loop narrows, but does not remove, potential failure conditions

The only direct `ac_BFSonundUnkn1` call in the inspected assembly is in the tactical loop at line 234, behind a startup counter gate (`231–235`). Native C `BFSonundUnkn1` (`sound.c:195–257`) runs only when `GetSoundAble` and `GetSoundActive` return true. The function calculates `&smptable[smp_id]` for each ID 1..255, but the first actual metadata dereference in that iteration occurs **after** `sample_status[smp_id].field_0` is confirmed nonzero (`222–229`). The loop therefore does not establish a guaranteed crash just because `smptable` is absent when every status is clear. When a status is queued, an invalid table pointer or insufficient table size becomes directly relevant. A status setter would itself already have dereferenced `smptable` in the current C implementation.

The source-derived 256 × 32 = 8192-byte figure in predecessor reports is the capacity to address **all** possible 8-bit sample IDs, not a verified retail `.tab` length or proof that all IDs are used. This audit does not change that status.

## MAIN-AUD-04 — Sound disabling does not imply a loaded-resource leak

The observed `SoundAble` checks gate the *loading* of both sound-bank lists as well as their normal cleanup (`free_map_level:27647–27655`, menu exit 54840–54847). When sound is disabled throughout a normal stage there is no reason from this code alone to infer that the stage allocated a sound bank which was left unfreed. The more specific test condition is **SoundAble state changing between a successful bank load and the corresponding free**; none of the cited ordinary menu/tactical code proves that transition occurs. `SoundActive` mute toggles are separate and do not control bank allocation/free. Library code may update `SoundAble` on audio state transitions; the full cross-file dynamic lifetime is not established.

## MAIN-AUD-05 — Loader failure states and pointer-table readiness

Native `LbDataLoadAll` frees existing entries at batch start, attempts all records, counts failures, and leaves successfully loaded entries allocated. For sound banks, four *source-possible* states exist (DAT/TAB success or failure independently). Native `sound_bank_setup` returns early if `smptable == NULL` but otherwise uses `smpdata + (uint)p_smp->data_start` without data-pointer validity or bounds verification. It does not return a success status to the assembly caller. Therefore neither returning from `sound_bank_setup` nor checking audio capability establishes a consistent, validated bank.

## Required nonproduction test matrix

| Case | Fault stimulus (copy/synthetic only) | Required observations |
|---|---|---|
| A | Baseline valid bank 0 and 1 | Menu, tactical, return-menu, sample output, pointer/status and allocation transitions |
| B | Menu sound-1 TAB missing with DAT present | Batch failed count, `smptable`, first menu sample call, clean application error vs memory access |
| C | Menu sound-1 DAT missing with TAB present | Setup handling of missing `smpdata`; pointer offset conversion and sample call |
| D | Tactical sound-0 TAB missing with DAT present | Mission start, status setter/mission complete or fail, clean error vs invalid table access |
| E | Both files missing for one bank | Load result, gating and safe exit |
| F | Truncated or short positive-read TAB/DAT | Actual read length, full-load validation, destination allocation and error handling |
| G | Sound disabled from startup | No bank load or early consumer access, no bogus leak classification |
| H | Forced sound-state transition after successful load | Cleanup behaviour and ownership before mode switch |
| I | Valid files with intentionally short TAB | In-bounds sample indices, validation of referenced IDs without assuming normal asset size |

Do not conduct this matrix on user saves, installed retail resources or the protected v0.7 reference. Test 32-bit Windows in a disposable copy and preserve sound effects, original pacing and campaign modes. A separate approved implementation request would be needed before changing any engine code.

## Verification

Performed: full assembly blob read; direct menu/tactical caller/source guard comparison; native wrapper and loader source read; conditional reachability reasoning; test matrix design.
Not performed: native compilation or symbol inspection, Windows/gameplay/audio tests, memory instrumentation, retail DAT/TAB size analysis, actual failure reproduction, code edits or branch merges.
Workstream rule: `main` core audit only; Mission Builder and Graphics Audit have separate owners. Campaign silo saves are between missions only; v0.7 and original 16 FPS tactical simulation remain protected.
