# Syndicate 2026 — Main Systems Audit: Partial Sound-Resource Load State Matrix

**Date:** 2026-11-01 (Australia/Brisbane)  
**Status:** Static C/assembly path analysis, NOT runtime fault reproduction. Main/core audit documentation only.  
**Predecessor:** `docs/audits/2026-11-01-main-game-sample-table-assembly-lifecycle.md` (commit `6cba1e776b1d8072f4b4f16448a8b62aa801c343`).

## Source-verified control flow

1. `engine/src/syndre.sx` blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`, read directly via GitHub blob API, defines separate `sound-0.dat/.tab` and `sound-1.dat/.tab` descriptors using common exported `smpdata`, `smptable`, `smptable_end` slots. See lines 82664–82674, 85050–85061, 83887–83891.
2. `engine/bflibrary/src/general/gdata.c:105–132`: `LbDataLoadAll` first frees all entries, then attempts every entry; it does NOT roll back all successfully loaded entries if another fails. Failed-entry count is returned. In `LbDataLoad` (65–103), an absent/invalid file leaves the corresponding pointer absent; a load-size mismatch zeros `Start`/`SEnd` (but loses the allocated pointer as noted in core audit).
3. `syndre.sx:27903–27911`, `54163–54174`, and `55040–55043` do not check the return count of these sound-bank batch loads before `ac_sound_bank_setup` or continued menu/mission execution. The current wrapper maps `sound_bank_setup`, `SetBFSampleStatus`, `BFPlaySample` and `BFSonundUnkn1` to native C, not their historical `ASM_*` bodies.
4. `engine/src/sound.c:77–99` `sound_bank_setup` rejects a NULL `smptable`, but not a NULL `smpdata`, short table, invalid `smptable_end` or invalid sample offsets. `SetBFSampleStatus` and `BFPlaySample` lack local table validity checks; `BFSonundUnkn1` iterates IDs 1..255.
5. `syndre.sx:54168–54174` uses pointer-slot `data_5bb2c` (whose descriptor value is `&smpdata`) and checks `*smpdata_slot == 0` to decide whether to load menu sound bank 1. The check is on `smpdata` alone, not `smptable` or a bank identity tag.

## Conditional state matrix — NOT tested executable outcomes

| After a two-entry sound-bank batch load | Observable C pointer state (absent entry => NULL) | Setup behaviour | Unverified downstream hazard |
| --- | --- | --- | --- |
| DAT succeeded, TAB succeeded | `smpdata != NULL`, `smptable != NULL`, `smptable_end` set to table end | Enters conversion loop if table has entries | Table size/offsets still unvalidated; success is not necessarily complete asset integrity |
| DAT succeeded, TAB failed | `smpdata != NULL`, `smptable == NULL` | Setup's NULL-table guard returns | Native status/play consumers may dereference absent table if reached; menu's DAT-only non-null test can skip bank-1 load |
| DAT failed, TAB succeeded | `smpdata == NULL`, `smptable != NULL` | Potentially attempts pointer conversion using missing data base when records exist | Invalid sample addresses if consumers reached; menu DAT-only test can retry loading bank-1 |
| DAT failed, TAB failed | Both data and table NULL | Setup returns | Native status/play consumers still lack their own NULL-table guard if reached; menu DAT-only test can retry |

**Qualifications:** Table states describe a *fresh*, completed batch load in the native `LbDataLoadAll` implementation, assuming the listed per-entry success/failure. They do not prove any case occurs with the original retail files or reaches a consumer. `SoundAble` is checked at the assembly call sites, but its value indicates audio availability, **not** successful loading of both game resource files; see prior audio-state audit. A native `LbDataLoadAll` batch failure is not equivalent to changing `SoundAble` to false. The exact effect of pointer arithmetic with absent `smpdata` is not reproduced or classified as a specific platform crash.

## Lifecycle qualifications

- A normal `free_map_level` path frees the tactical sound bank only while `SoundAble` is true (`syndre.sx:27640–27656`); menu `select` cleanup likewise conditionally frees sound-bank-1 (`54838–54848`). This leaves a conditional stale-pointer/retained-allocation question if `SoundAble` changes before the corresponding cleanup. It is NOT proof that this sequence happens.
- Because both descriptors alias the same exported `Start` slots, `LbDataLoadAll` on one bank can free resources loaded by the other bank before replacing them. Avoid any code change that assumes distinct simultaneous banks.
- On the `LbFileLoadAt` size-mismatch branch the native loader zeros a descriptor's `Start` without freeing the just-allocated block. This separate conditional allocation-loss path is already documented; the matrix is about consumer readiness, not proof of a runtime leak.

## Static acceptance gates before a code patch

1. **Reconstruct full phase ordering** from first `select`, mission entry, sound setup, status update, mission exit and level result return; identify SoundAble state changes and whether every path frees or reloads banks.
2. **Validate source sizes** of `sound-0.tab`, `sound-1.tab`, `sound-0.dat` and `sound-1.dat` privately, including the 8,192-byte minimum descriptor addressability for all 256 indexed IDs and every table offset; this minimum is derived from the C consumer, not proven as a required original retail table size.
3. **Test synthetic, non-retail negative cases** on disposable Windows 32-bit execution: four matrix states, short RNC read, bad table pointer/end, invalid sample offset, interrupted load, and sound-disabled cleanup. Capture actual return values and fault location.
4. **Do not** modify the loader, source code, FPS, original/expansion behaviour, mid-mission saves, campaign save namespaces, Graphics Audit or Mission Builder before review. Keep v0.7 as rollback.

## Verification register

**Performed:** complete original assembly blob re-read, native C loader/read checks, descriptor-pointer tracing, four-case static state derivation and report creation.  
**Not performed:** CI/native compilation, runtime Windows/retail execution, real sound data measurements, fault injection, address sanitizer, gameplay, audio playback, any source or save file writes.  
**Finding type:** conditional, evidence-backed consumer-validation exposure; **no reproduced crash or defect claim**.  
**Next checkpoint:** phase ordering, sound status processing and load/free transitions, followed by isolated runtime tests.
