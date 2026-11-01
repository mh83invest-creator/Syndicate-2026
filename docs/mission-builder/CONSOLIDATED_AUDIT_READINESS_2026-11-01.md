# Mission Builder — Consolidated Source Audit and Integration Readiness Report
Date: 2026-11-01 (Australia/Brisbane)
Branch: feature/mission-builder-audit
Status: READ-ONLY STATIC AUDIT CONSOLIDATION; NOT IMPLEMENTED; NOT RELEASE READY
Scope: Syndicate 2026 Mission Builder, World Domination, Custom Campaign integration, and interfaces to classic campaigns and shared engine.
Sources: engine/src/syndre.sx Git blob c35e22621a93d1e07c44d40de8867446ff7c64bf (85,803 lines); engine/src/{game_data.c,game_data.h,main.c,game.c,dos.c,wrcountry.h}; engine/bflibrary/src/general/{gfile.c,rnc_1fm.c}; engine/bflibrary/include/rnc_1fm.h; engine/util/install; docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md (main); docs/SHARED_DEVELOPMENT_HANDOVER.md (main); feature-branch docs/mission-builder/HANDOVER.md, MB-AUDIT-0.1–6.1 and MB-STRUCT-2–32.

## 1. Executive determination
The native engine has identifiable paths for mission selection, map/briefing resources, tactical level loading, tactical win/loss, computer-player objectives, territorial consequences and existing save screens. These are source-backed integration *investigation points*. They do NOT establish a plug-in Mission Builder, safe writable binary format, separate American Revolt campaign selector or safe custom campaign resource loading. The minimum viable implementation is NOT ready for merge. Preserve v0.7 and the original 16 FPS game loop.

## 2. Architecture and startup
- engine/src/main.c calls imported assembly syndicate() after game_initialise. Current CLI parser has no verified Original/American/World/Custom campaign switch (MB-AUDIT-2.0).
- main game implementation includes native C + legacy syndre.sx assembly with generated Watcom-to-C wrappers from engine/conf/wrappers_game.conf and other mappings.
- Mission Builder should integrate with that engine; no separate incompatible standalone game is permitted. The proposed 4-way campaign selector remains a requirement, not present implementation.
- Source-level American Revolt edition/resource selection is still NOT established. Do not infer its installed presence from source installer filenames.

## 3. World-map, mission identity, and dispatch
- map_selection at syndre.sx 31709+, territory mapping via data_5b071 to current_levno ~32017–32022 / 32062–32066; can_i_play_this_city at 32871+ gates eligibility and influences map rendering.
- process_action branch ~28531–28542 calls load_map_level(current_levno, level_fname) then init_players_people; process_action begins ~28450.
- load_map_level at 27845–27933 formats data/gameNN.dat, decodes into level__Seed, obtains level__MapNumber and loads map resources. Mission ID and map ID are different values.
- mission_briefing at 1207+ independently reads gameNN.dat and mapNN.dat before tactical entry. Main menu invokes briefing ~54661, while load_text at 55249+ builds mission-text names data/missNN.dat or localized equivalents. Both briefing and tactical paths need a future mode-aware *validated* resource resolver; global path remapping is not approved.
- Legacy two-digit filename and installer-listed game01–50/game90–99 do NOT prove arbitrary custom ID support, nor determine campaign identity.

## 4. Tactical objectives and results
- level__Seed=0x80108; level__Objectives=0x9BE3A, relative 0x1BD32, eight 14-byte entries; level__CPObjectives=0x9BEB2, relative 0x1BDAA, 128 entries x15 bytes; level__End=0x9C632, level region 0x1C52A=116010 bytes. These are **memory offsets**, not compressed-file offsets or proof of decoded file length.
- check_end_level at 531–1136 processes eight tactical objectives; vtable_10550 at 491–508 handles 0–16, with per-type entity state, person state, relationship and proximity checks. Every status dword must be 1 for ordinary victory, with a cheat shortcut. Some handlers invoke level_failed independently. Numeric codes are NOT validated author-facing objective names.
- CPSetupNewObjective at 9773+, CPSetupDoMissionObjective at 9600+, CPGetObjective at 9364+, CPCanAgentDoMission at 9498+ and CPCompletedObjectiveStep at 9031+ implement computer-agent logic. The 15-byte CP objectives and 14-byte tactical objective records are *distinct*; their fields and action types must not be conflated.
- CPCompletedObjectiveStep: for ActionType=1, Action 3 checks an agent action/status byte, Action 4 compares a referenced thing's three coordinate/field values, Action 9 compares a person field to objective TargetOffs, and action codes 11–14,20,25 have a direct completion branch (9039–9104); other ActionType values 2–10 dispatch through vtable_16a44 (8999–9008) to per-agent, mission objective and target-status predicates. vtable_16a68 (9010–9027) is the nested 0–16 tactical condition dispatcher. These are *branch observations*, not proof of full story-facing meanings.
- When an objective step is complete CPCompletedObjectiveStep advances the agent's tactical subobjective index and calls CPSetupDoMissionObjective if next entry is nonzero (9237–9256). Once a linked chain is exhausted, it resets agent state, flags the selected objective and traversed children with 0x10, and may update per-team assignment state (9260–9298). CPDoTurn calls it at ~7758 and ~7834.
- CP initialisation at 7052–7088 clears flag 0x10 for all 128 records and copies computer team configuration; runtime logic later sets it. CPGetObjective can manufacture a fallback record and mutate target/action fields. Therefore authoring data and runtime state must be independently modelled.
- level_complete sets terminal bit 0x2; level_failed bit 0x4 (451–489). level_finished at 54998+ processes success into country ownership/state and has win/loss/terminal paths. Original terminal flags and campaign endgame must remain protected.

## 5. Strategic state, rivals and revolts
- country_states[50] packed 10-byte records, with Happiness, OwningPlayer, Revenue, TaxPercent, Population in engine/src/wrcountry.h.
- Strategic tick ~11603–11640 updates happiness, revenue and rival expansion; ComputerPlaysMap ~11896–11997 does strategic opponent selection/ownership changes; tax edit ~33082+ and delayed propagation were traced in MB-AUDIT-1.7/1.8.
- Source suggests owned-territory special replay when Happiness=0xFF and a particular level_finished result branch, BUT interpreting 0xFF as a tax-driven revolt was explicitly corrected in MB-AUDIT-1.6. No proven low-happiness rebellion trigger, randomized revolt mission generator or independent World Domination simulator exists.
- Original 50-country campaign win condition and eligibility logic must not change for classic modes. World Domination's distinct strategic/revolt mechanics require separate approved design and runtime validation.

## 6. Level loading, resource and decompression integrity
- clear_savegame zeros [level__Seed, level__End); load_map_level reads RNC data directly into that memory and invokes init_level_data. Single-player init_level_data invokes init_computer_players; multiplayer skips that specific call.
- Legacy LbFileReadRNC (syndre.sx 14916–14960) uses LbFileLengthRnc then calls LbFileRead_ / ac_read, calls assembly UnpackM1 and finally returns a handle-related value rather than decoded size. No visible complete-read comparison or destination-capacity argument at that boundary.
- RNC1 header: 18 bytes; signature RNC byte 01; big-endian expanded length at +4 and packed length at +8. The legacy reader requests the expanded length as an input read count. Legacy UnpackM1 has global decoder state, unbounded-looking Huffman and backreference paths; malformed-input safety is unproven (MB-STRUCT-27–32).
- C gfile.c: LbFileLoadAt has different return/error semantics but checks only read_status==-1, not exact returned size. C rnc_1fm.c rnc_unpack has CRC checks and some bounds guards, yet its wrapper UnpackM1 derives allocation from expanded-size header and copies before NULL check. It is not an established safe replacement for arbitrary user data.
- No verified retail mission bytes, exact expanded sizes, binary version, true entity-reference validity or end-to-end parser tests. **Do not treat a numeric memory offset as directly writable within compressed gameNN.dat.**

## 7. Entity pools, AI, navigation and spatial validation
- level__things_base begins at assembly 0x8810e, with people, vehicles, objects, weapons and linked thing offsets traversed in objective handlers; set_structure_ends at 27660–27705 computes last active person/vehicle/object from memory scans. Prior MB-STRUCT-8 inference about other last_person mutations was corrected by MB-STRUCT-32.
- Objective handlers dereference thing offsets and linked chains; they can compare object status bits, ownership, coordinates and 0x300 proximity thresholds. The correctness, range, count, signedness and legal referent classes must be validated for future editor inputs.
- Static AI objective dispatch is visible; safe arbitrary spawning, pathfinding, collision and map editing constraints are NOT established by this audit. Do not promise editor support for new entity types, terrain or procedural generation.

## 8. Save identity, compatibility and Continue
- Existing C load_save_text, load_player, save_player in engine/src/game_data.c construct save/%02d.gam and use LbFileOpen; assembly load-screen and main menu separately enumerate ten slots via ac_load_save_text, with ac_load_player/ac_save_player operations.
- Native LbFile transform can resolve relative saves under data directory; DOS low-level path routing differs and special-cases save/ under user root. The installed Windows file location must be determined by disposable runtime test, not assumption.
- load_player has void return and does not validate each read count; save_player lacks per-write result checks and has a credit adjustment side-effect. A separate Continue route is NOT source-confirmed.
- Required separate Original, American Revolt, World Domination and each Custom Campaign save namespace, identical-slot isolation, wrong-mode rejection, legacy preservation and between-mission-only saving: **not implemented**. Per-custom-campaign isolation is in the later structural gate register; the older draft main contract's unresolved granularity text is stale relative to the approved specific requirement. No legacy data may be moved, deleted or reclassified by guesswork.

## 9. Packages, paths, deployment and resource ownership
- engine/util/install installs original owned data into a newly accepted target; contains unzip -o and rm -rf TARGET failure cleanup. It is not safe for installation of a custom pack into an existing game data directory.
- game_transform_path_full, dos_path_to_native and lookup_path provide compatibility resolution, not a verified sandbox or custom-package path confinement policy. Prior MB-STRUCT-21–24 audits define traversal, case-collision, symlink, manifest, staged publication and rollback requirements; none implemented.
- Custom campaigns must not overwrite classic gameNN.dat, shared maps, existing saves or another custom campaign's files; resource namespace, bundle manifest, stable campaign identifier, version compatibility and safe install/uninstall remain designs subject to approval.

## 10. Test and evidence inventory
**Actually inspected**: assembly source blob, C headers/loader, native save functions, DOS wrappers, startup, installer and existing detailed GitHub handover/contract reports.
**Not performed**: compilation; Windows or other runtime testing; original/American Revolt gameplay; original resource fixture parsing; decompression fixtures; fuzz/corrupt-input tests; save location/compatibility; branch merge; entity-limit experiments; editor UI; or campaign-mode demonstration.
**Not obtained**: legally owned authentic retail mission-file fixtures and expansion-specific files. Their absence blocks claims of disk-format compatibility.

## 11. All release/readiness gates (consolidated)
R01 Authentic mission data / decoded layout: BLOCKED, retail fixtures absent.
R02 Independent capacity-bounded resource decoder: NOT IMPLEMENTED.
R03 Read-only mission/structure inspector: NOT IMPLEMENTED.
R04 Positive, negative and corruption fixture tests: NOT IMPLEMENTED.
R05 Human-readable objective meanings, sentinel state and valid target relationships: PARTIAL STATIC MAPPING; NOT VALIDATED IN GAME.
R06 Verified original versus American Revolt campaign/resource selection: UNVERIFIED.
R07 Multi-campaign mission/resource identity and safe routing: NOT IMPLEMENTED.
R08 Campaign-isolated save/list/load with legacy compatibility: NOT IMPLEMENTED.
R09 Custom pack manifest and ownership rules: PROPOSED; NOT IMPLEMENTED.
R10 Non-destructive staging, publish, rollback, uninstall: PROPOSED; NOT IMPLEMENTED.
R11 Windows builds, original campaign regressions and cross-branch review: NOT PERFORMED.
R12 Versioned source handover and static mapping: COMPLETED for audited parts; this is not an implementation gate.
End-to-end Mission Builder readiness: **NO**.

## 12. Safest future engineering path — proposal, not authorised implementation
A. Obtain licensed original mission samples (multiple gameNN.dat, mapNN.dat, missNN.dat and if available expansion variants) in an isolated copy. Record file hashes and version provenance.
B. Independently validate the header, packed and expanded sizes, decode using separately bounded test tooling, and verify source offsets, objective types and target references. Add read-only inspector and synthetic failure fixtures before any writer.
C. Trace actual Windows Original/American mode and asset availability, compare with existing source branch and save paths, confirm contract with mainline.
D. Only then submit a separately approved minimal custom mission proof of concept: one existing map, one validated objective, briefing, completion and return, no classic asset overwrite, no active-entity format expansion.
E. Following tests, design separate campaign ID/namespace, campaign-specific save service and transactional pack installer. Keep original 16 FPS/30 FPS menu timing, menu controls, fixed campaign outcome and between-mission save semantics.
F. Request cross-workstream review and use PR before any protected baseline integration.

## 13. Corrections, conflicts and audit scope
- Existing MB-STRUCT-2–32 already cover large structural portions, and substantially overlap MB-AUDIT-4.8–6.1. This consolidated report does NOT pretend those were all newly discovered in this work session.
- Earlier shorthand suggesting an owned happiness value 0xFF establishes a low-happiness revolt was corrected: it does NOT.
- A previous suggestion that general legacy save audit can be entirely separated from Mission Builder is too strong: save-mode contract and ID propagation are specifically integral to Mission Builder, with actual native implementation coordinated with mainline.
- No claim of exhaustive absence is valid where dynamic/indirect dispatch, external assets or runtime conditions are unresolved. The audit is a comprehensive current **static evidence/readiness** assessment, not a proof of finished executable behaviour.
- Future repeated static investigation should be tied to an identified gate, not open-ended commit numbering.

## 14. Handover and protection
Game source modified: NONE.
Resource/asset/save files modified: NONE.
New document: this consolidated static audit on feature/mission-builder-audit.
Build/test results: NONE EXECUTED; source checks only.
Original and American Revolt, v0.7, keyboard/mouse and existing speed/visuals preserved by non-interference. No unapproved mechanics, level edits, asset import or build changes.
