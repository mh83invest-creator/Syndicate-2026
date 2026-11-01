# Syndicate 2026 — Main Systems Audit: Mission Load Failure to Initialisation

**Date:** 2026-11-01 (Australia/Brisbane)  
**Workstream/branch:** MAIN/core audit on `main`; documentation only.  
**Source basis:** complete original `engine/src/syndre.sx` blob SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf` (85,803 lines) plus current native Bullfrog library source, read directly from GitHub.  
**Predecessor:** `docs/audits/2026-11-01-main-animation-loader-extent-provenance.md`, commit `866f37ffa99c8f58397d6603fada17aa57b21dd0`.  
**Status:** **static source-confirmed validation gaps**, not compiled or tested crashes; no source/gameplay changes.

## Findings

### MAIN-ENTRY-01 — Mission-load failure return is dropped before first dependent consumers

At `syndre.sx:27845–27933`, `load_map_level` selects resources based on `DrawFlags`:

- On the observed `DrawFlags == 2` branch (`27866–27869`), it reads a level file into `level__Seed` using `LbFileReadRNC` (`27873–27880`) but does **not** test the file-read return status, then builds the two-digit map filename and calls `ac_LbDataLoadAll(mission_load_files)` at `27901–27903`.
- The returned failure count from `ac_LbDataLoadAll` is **not tested** before it conditionally attempts a separate sound-bank-0 batch load/setup (`27904–27911`).
- It then **unconditionally calls `init_hires_blocks`** at line 27913 on the `DrawFlags == 2` branch and proceeds to `init_level_data` at `27930`. Source contains no intervening all-resources-present / length / per-entry-success gate.
- The `DrawFlags == 4` branch reads a level file and calls `ac_LbDataLoadAll(unkn1_empty_load_files)` (`27915–27928`), whose listed descriptor is the *empty terminal* 44-byte element at `82675–82676`, not the full mission resource list. The same `init_level_data` call then follows. This is an **independent mode with different expected resource provenance**, not evidence that full mission assets were improperly skipped. If `DrawFlags` is neither 2 nor 4, the observed function still calls `init_level_data`, but the assumed pointer state/other mode setup is not established.

**The direct observed failure-chain claim is therefore specifically for DrawFlags=2 and a failed `mission_load_files` batch.** No Windows mode selection/reachability or legal retail asset failure has been reproduced.

### MAIN-ENTRY-02 — A missing h_blocks table reaches direct dereferences during block initialisation

`mission_load_files` includes `data/hblk01.dat` → `h_blocks` (`syndre.sx:82633–82640`). `init_hires_blocks` (`79740–79781`) reads `h_blocks` at 79748, then again at 79755, and performs the **direct pointer dereference** `(%esi)` at 79756, followed by a dereference of the resulting block record pointer at 79760. No local NULL or decoded-table extent validation precedes these reads.

Its source loops run across up to 256 outer block IDs, six categories per ID, and inspect up to 16 block records in the observed inner loop depending on the data. These are **static loop bounds, not measured operations**.

Native `LbDataLoadAll` in `engine/bflibrary/src/general/gdata.c:105–130` frees existing entries, attempts each and counts failures without transactional rollback. With `hblk01.dat` missing, `h_blocks` can remain NULL and the assembly proceeds to `init_hires_blocks` without checking the reported failure count; that gives a concrete **conditional invalid-dereference exposure**. Do not infer the specific Windows exception or presence of a failure in authentic resources without a test.

### MAIN-ENTRY-03 — A missing map_buf reaches an unguarded 12,288-entry relocation pass

`mission_load_files` declares the dynamic `data/map??.dat` destination `map_buf` at `syndre.sx:82625–82632`. `init_level_data` begins (`27712–27722`) by passing `map_buf` to `init_map_data`.

`init_map_data:25694–25716` immediately advances the pointer **12 bytes**, then loops `0x3000` (= 12,288) times, reading each four-byte entry and replacing a relative pointer with an adjusted pointer. No local NULL/length check precedes the first indexed dereference at line 25707. Source-addressability for that relocation table alone is **12 + (12,288 × 4) = 49,164 bytes**, **not** a verified full valid original map-file length or correct decoded map format. This establishes a second earlier consumer path on a failed or too-short load. A partially successful batch can load other mission files while leaving this pointer absent.

The relocation **mutates** the loaded map table in place. Any future test/validator must not accidentally invoke it twice against the same buffer, since second-pass behaviour is not equivalent to the first relative-pointer conversion. No engine change is proposed.

### MAIN-ENTRY-04 — Sprite table setup is another distinct consumer, not a successful-load validator

`init_level_data:27723–27737` calls native `ac_LbSpriteSetup` for `h_sprites`/`h_sprites_end`/`h_sprites_data`, then another set of sprite pointers. The actual wrapper maps `LbSpriteSetup` to C (`engine/conf/wrappers_game.conf:212`). Native `engine/bflibrary/src/general/gsprite.c:27–45` starts at `start`, iterates while `sprt < end`, and relocates pointer-like `Data` fields by adding `data` when a comparison passes. The function does **not** validate buffer sizes, allocation ownership, whether `start <= end` belongs to one allocation, or whether the `data` pointer is valid.

This is *not* equivalent to saying the routine always dereferences a NULL sprite table: if pointers are both NULL, its loop comparison can skip iteration. Actual pointer ordering and failure states matter. However, even a successful function return is not a sufficient validation of the loaded sprite data or all subsequent draw calls. `LbSpriteSetupAll` in the same file checks the **slots** `Start` and `End` for NULL, not the loaded values; this routine is not the one directly called at `init_level_data:27729`. Avoid conflating these two functions.

### MAIN-ENTRY-05 — First normal effect/frame/audio consumers are later than the loading hazards

`init_level_data` also calls `set_structure_ends` then `ac_ClearBFSampleStatus` at `27808–27809`, after map/sprite initialisation. That clears queued sample-status state but does not itself validate the game sound bank or `.ani` tables. Once gameplay/drawing proceeds, the separately audited `frames`, `elements_ani`, `starts_ani` consumers and frame sound-trigger calls have additional pointer/index validation gaps. Those are downstream; **missing map and block resources may reach an earlier failure first**, so any fault-injection test should record the first failing stage rather than ascribe all errors to a later animation/sound consumer.

The direct menu/tactical and asset-mode order should be verified against an actual 32-bit build before any error-handling patch.

## Source-backed acceptance matrix — NOT executed

| Missing / malformed input in a disposable copy | Earliest source-observed consumer | Test observation required |
| --- | --- | --- |
| `hblk01.dat` missing (DrawFlags 2) | `init_hires_blocks` → `h_blocks` dereference | Batch failure return, pointer state, first invalid access / graceful abort |
| `map??.dat` missing (DrawFlags 2) | `init_level_data → init_map_data` | Missing map pointer, 12,288-entry relocation entrance, safe abort vs invalid read |
| Map payload shorter than 49,164 bytes | `init_map_data` relocation pass | Actual decoded byte extent and checked table indexes |
| `hspr-0.tab` missing but `.dat` loaded | `LbSpriteSetup`, subsequent draw | `start/end/data` pointer states, whether setup skips or accesses, subsequent render readiness |
| `hfra-0.ani` missing | Frame consumers in render/animation | First reachable frame read and failure state |
| `hele-0.ani` missing | Element link traversal in render | First valid/invalid 10-byte record and pointer state |
| `hsta-0.ani` missing | Start selector lookup on entity/effect/draw path | First reached selector, per-entry load state, controlled failure |
| `sound-0.tab` missing with DAT present | Native status setter/playback | Audio availability vs bank readiness and first unsafe call |
| `DrawFlags == 4` mode | Separate file/empty descriptor path | Record expected pointer ownership and mode transition; do NOT assume it must load mission files |

A later *separately approved* harness must run one fault scenario at a time, log stage entry and exit (without altering the original order), validate original asset extent privately, use synthetic negative cases, and compare against untouched v0.7. Do not ship proprietary resources, merge the parallel audit branches, alter 16 FPS/tactical simulation or add mid-mission saving.

## Verification and unresolved work

**Performed:** exact native C and full original 85,803-line assembly source inspection, mode conditional and caller tracing, direct first-dereference line identification, source arithmetic, and creation of this documentation report (read-back verified separately).  
**Not performed:** reproduction of exceptions, game compilation/CI, 32-bit Windows launch, original retail asset measurements, file corruption tests, actual sound/visual output, runtime profiling, saved-game inspection or modification, code patch, Mission Builder/Graphics branch changes or merges.

**Next MAIN checkpoint:** determine non-destructive mission and animation loader failure boundary (including early level RNC direct read and display-mode pointer provenance), record a reproducible diagnostic/fault-injection gate, and coordinate any shared loader patch with separate workstreams before implementation. **Protected v0.7 remains unchanged.** Original/American Revolt and between-mission-only siloed saves remain protected.
