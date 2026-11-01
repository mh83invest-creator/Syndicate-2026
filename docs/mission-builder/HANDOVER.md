# Mission Builder / World Domination — Development Handover
Status: AUDIT ONLY — no game code changed
Date: 2026-11-01 (Australia/Brisbane)
Branch: feature/mission-builder-audit
Repository: mh83invest-creator/Syndicate-2026
Related shared coordination document: docs/SHARED_DEVELOPMENT_HANDOVER.md (main branch, commit 3fb2df07faa54cf7b10cbb5de1570f88cd5c25fb)

## Project intent and agreed decisions
- Mission Builder must integrate into the Syndicate 2026 native engine, not a separate incompatible game.
- Selection screen concept: Original Campaign, American Revolt, World Domination, Custom Campaign.
- Keep original campaigns intact; Mission Builder accessed separately.
- Support complete Bullfrog-style flow: selection > briefing > agent/equipment selection > tactical mission > results > campaign effects.
- Allow pre-authored random missions (especially revolts) and, when technically validated, variable parameters and eventual procedural generation.
- World Domination is a selectable campaign with overarching goals, territorial consequences, rebellion and dynamic missions.
- Do not implement unapproved mechanics or edit existing original campaign missions. Classic-campaign revolt randomisation is a separate optional change requiring approval.

## Current evidence
- GitHub README.md on main says engine/ contains native source derived from SyndicatFX 0.0.6.1022 and reflects Windows playtests through v0.7.
- README says mission loop at original 16 FPS, company configuration 30 FPS.
- Original game assets must be supplied separately; not in repository.
- A repository-level mission code search returned no results; this does NOT prove functionality absent.
- Individual engine source files have not yet been inspected. All feasibility and integration details below are INVESTIGATION TARGETS, not established capabilities.

## Technical audit checklist
1. Inventory engine code, documentation and build configuration. Record actual paths/functions before making conclusions.
2. Trace title/menu and Original Campaign / American Revolt selection, plus data loading.
3. Trace mission identity / briefing text / map selection / placement / inventory and agent allocation.
4. Trace objectives, triggers, win/loss, post-mission results, money and territorial state.
5. Trace taxation thresholds, uprising events, revolt mission dispatch.
6. Trace saves and persistence, version compatibility and original campaign data.
7. Trace spawning/AI/navigation, map collision, supported asset types and maximum entities.
8. Compare any relevant FreeSynd structures for architectural insight, without assuming file-format compatibility.
9. Define extension points: CampaignRegistry, MissionDefinition/Loader, MissionResult, Event/ScenarioSelector, SaveSchema as conceptual names only. Do not claim these interfaces exist.
10. Record risks: hardcoded mission IDs, binary format assumptions, licensing/data dependencies, compatibility, pathfinding and save migration.
11. Draft an integration contract with exact source references and provide mainline audit a cross-check opportunity.
12. Plan minimal end-to-end proof of concept (custom mission on existing map with briefing and completion) and integration tests before editor or strategic simulation.

## Isolation and collaboration rules
- Main is the protected production/development baseline. No changes to mainline gameplay from this branch without review and approval.
- Read latest docs/SHARED_DEVELOPMENT_HANDOVER.md from main at the start of each work session, plus latest mainline commit and relevant engine audit handovers.
- The shared coordination document lives on main. This branch was created before it and MUST be updated from main by a controlled merge/rebase before integration; do not assume it is present on this branch.
- Maintain this handover with dated evidence, file paths, exact modifications, tests, limitations and next steps.
- Use pull requests for eventual implementation; check for concurrent mainline changes to the same files.
- Preserve keyboard/mouse, timing, camera, combat visuals, original campaign and American Revolt behaviours.

## Implementation / test report
- Source code modified: none.
- Mission Builder implementation: none.
- Tests: no compilation or gameplay testing.
- Documentation: this handover created.
- Outstanding: source inventory and feasibility assessment, integration design, proof-of-concept test.
- Handover version: MB-AUDIT-0.1.

## Audit 0.2 — Source architecture discovery (2026-11-01)
Read from main repository source:
- `engine/src/game.c`: `game_initialise()` asserts fixed offsets for `level__CPObjectives` relative to `level__Seed` (lines ~69–70). `game_update()` targets 16 FPS, and `game_update_menu()` targets 30 FPS. This confirms existing memory layout constraints, not an editable mission loader.
- `engine/src/wrcountry.h`: packed `CountryState` contains `Happiness`, `OwningPlayer`, `Revenue`, `TaxPercent`, `Population`; `country_states[50]`. `wrcountry.c` contains no corresponding active gameplay logic in current source snapshot. Territory mutation code must be located elsewhere.
- `engine/src/game_data.c`: `load_player(int slot)`, `save_player(char *,int)` operate on `save/%02d.gam`, using fixed binary arrays `players`, `country_states`, `weapons`, `cybmods`, `research`, `selected_team`. Hard `sizeof` assertions include players=8376 bytes, countries=500 bytes. `save_player` also changes credits under a threshold condition. Do not change original save format without explicit compatibility plan.
- `engine/src/Makefile.am`: compiles `syndre.sx`, wrapper objects generated from `conf/wrappers_*.conf`, with native C files. Treat mission/campaign extension as native+assembly integration until further proved.
- `engine/src/game_bstype.h`: `ThingIdx` and `MapCoord` types available for later review.
- `engine/src/player.c` and `wrcountry.c` contain scaffolding, not yet substantive gameplay implementations in this snapshot.

Engineering consequences:
1. First prove how original assembly-linked mission selection/objectives work. Do not assume simple C hooks exist.
2. Prefer separate versioned World Domination state or demonstrably compatible save extension; no changes to existing `.gam` layout until proven safe.
3. Mission builder should initially emit a separate data model; actual in-engine execution is unverified.
4. Preserve original territory state and mission update timing.
5. Existing selectable mode design remains approved concept, not proven implementable yet.

Next inspection: `engine/conf/wrappers_game.conf`, `engine/src/game_data.h`, `engine/src/player.h`, generated symbol relationships, source data inventory and `syndre.sx` treatment.
Tests: static inspection only, no build/runtime test.
Code modified: none.
Handover version: MB-AUDIT-0.2.

## Audit 0.3 — Objective schema and execution boundary (2026-11-01)
Additional inspected repository files:
- `engine/src/game_data.h`: packed `CPObjective` (fields `Child`, `Parent`, `field_4`, `field_6`, `ActionType`, `Action`, `TargetOffs`, `field_B`, `field_D`); declared `level__CPObjectives[128]` and `level__Seed`. Semantics of objective action values are NOT yet known.
- `engine/src/main.c`: declares `current_levno`, `level__MapNumber`; initialises both to 1; calls `syndicate()` after resource initialization. `syndicate()` is declared as assembly import. This is a critical runtime boundary requiring assembly/data examination.
- `engine/src/Makefile.am`: includes `syndre.sx` as source and generated wrapper objects. `engine/conf/wrappers_game.conf` includes `game_update`, `game_update_menu`, `load_player`, `save_player`, etc. Wrapper presence does not imply editable mission functions.
- `engine/src/guitext.c`: loads `language/<language>/guitext.dat` into GUI strings. This is menu text, NOT established mission briefing text support.
- `engine/src/player.h`: `PlayerInfo players[8]`, selected agent team `selected_team[4]`; direct use in binary save routines reinforces ABI constraints.

Implications:
- An existing objective array provides a concrete target for identifying mission semantics, but its encoding and loading path must be verified.
- `current_levno` and `level__MapNumber` must be traced into mission selection and any expansion switches.
- Assembly imported `syndicate()` is the principal unknown. Do not promise adding selectable campaigns until the integration seam is demonstrated.
- Need inspect stored form and provenance of `syndre.sx`: connector returned empty text, so cannot conclude it is a genuinely empty file or accessible textual assembly.
- Need inspect actual original game mission resources separately if legally provided; none committed.

Next audit: inspect workflow, assembly asset provenance, data directories and available symbols; produce risk-graded implementation strategy; no coding before validated integration seam.
Tests: static code/document reading only. No compilation or runtime tests. Existing behaviour unchanged.
Handover version: MB-AUDIT-0.3.

## Approved requirement — siloed saves by campaign (2026-11-01)
User explicitly requires Original Campaign, American Revolt and each added mode (including World Domination and Custom Campaigns) to have separate save collections. A campaign's save/load interface must ONLY display/access its own saves; saving in one campaign must not overwrite another campaign's identically numbered slot.

Cross-development contract to establish before implementation:
- Campaign identity selection and propagation into save/load/continue (and deletion if offered).
- Storage namespace and validation against incorrect campaign loads.
- Preserve compatibility with existing legacy `save/%02d.gam` fixed-format saves; identify what old slots represent before proposing migration.
- Determine treatment of multiple user-made Custom Campaigns (individual campaign IDs versus one shared Custom Campaign namespace) during design; no silent assumption.
- Avoid changing original binary payload layout without a tested migration or versioned extension.
- Test independent slot numbering, save-list filtering, wrong-campaign rejection, legacy compatibility and uninterrupted original campaign behaviour.
- Co-ordinate this requirement with mainline handover at `docs/SHARED_DEVELOPMENT_HANDOVER.md` (main). Both workstreams must review save-related changes.
Status: documented and approved requirement, NOT implemented or tested.
Handover version: MB-AUDIT-0.4 (documentation-only).
 
## Audit 0.5 — Gameplay assembly and save isolation integration (2026-11-01)
Verified:
- GitHub contents API returned substantive `engine/src/syndre.sx` assembly text (begins with symbol export macros and game routines). Prior connector empty response was incomplete; the assembly is NOT an empty stub. Its menu/mission internals remain to be traced.
- `engine/src/main.c` calls imported `syndicate()` (actual gameplay entry) and sets `current_levno=1` and `level__MapNumber=1` before start.
- `engine/conf/wrappers_game.conf` generates native-to-original wrappers for `load_save_text`, `load_player`, `save_player` and timing/update routines. This establishes a C-to-assembly calling seam for saves, not yet a campaign-selection seam.
- `engine/src/game.c:game_transform_path_full()` detects `save/` and redirects to user directory; `game_data.c` hardcodes `save/%02d.gam` for slot operations. Save isolation must cover both direct references and path resolution.
- `engine/src/game_data.h` defines packed `CPObjective` with 128 elements; semantics of each action and target are not yet established.
- `docs/HANDOVER_POLICY.md` mandates versioned release handovers for v0.8+ builds. Any future build must obey it, even experimental or failed builds.
- Source archive tree has 411 Git objects in main and includes no proprietary retail mission data.

Save isolation design investigation:
1. Locate campaign selector in original assembly, establish stable campaign identity before any save/load UI or wrapper call.
2. Decide explicit stable ID for each built-in and custom campaign; do not infer from slot position.
3. Save list must enumerate only active namespace, and all save/load entry points must enforce it, not just UI filtering.
4. Check how old shared `save/%02d.gam` slots identify campaign, if at all. Unidentifiable legacy files require a non-destructive compatibility policy for user approval.
5. Consider versioned per-campaign storage and additional mode state as *proposals*, not approved format.
6. Test cross-campaign identical-slot isolation, wrong-campaign load denial, restore/continue and old files.
Status: static audit, no code changes, no build or gameplay tests. Handover version MB-AUDIT-0.5.

## Audit 0.6 — Assembly access assessment and integration contract (2026-11-01)
- GitHub recursive tree lists `engine/src/syndre.sx` as a real tracked source blob, SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf`, size 1,848,334 bytes.
- The repository API provides the initial assembly excerpt, but full raw retrieval rejected the oversized file and line-ranged connector read returned empty; therefore campaign selection and mission-loader functions are not yet traced. No claims of located assembly hooks.
- Cross-workstream contract recorded on main: `docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md` (commit `07770b24d4d5047ebcae442c33bec4386b31a0c2`). Read this alongside shared handover before implementing any save or mode changes.
- Native code confirms `load_save_text()`, `load_player()`, `save_player()` use the common slots; `game_transform_path_full()` separately handles save prefix.
- Need a locally available checkout or an endpoint capable of indexed assembly searching to finish tracing all callers. Do not bypass this by implementing a speculative hook.
- Integration gates G0–G5 and design risks are in the shared contract.
- Tests: static repository inspection and GitHub tree checks only. Source code unchanged. Handover version MB-AUDIT-0.6.

## Audit 0.7 — File-access boundary and save-list constraints (2026-11-01)
Inspected `engine/src/dos.c`, `engine/src/game.h`, `engine/src/guitext.h`, `engine/src/util.c`, `engine/conf/fnmap_windows.conf`, `engine/conf/fnmap_posix.conf`, `docs/HANDOVER_POLICY.md`.
- `dos_low_level_open()` calls `game_transform_path()` and then `dos_open()`, providing another file-path interception seam for assembly-driven DOS-style accesses; `dos_open()` itself converts path with `dos_path_to_native()`. Which assembly save calls use each route is still unproven.
- `dos_creat()` / `dos_sopen()` delegate to `dos_open()`; writer and reader access paths must be traced independently to avoid cross-mode save bleed.
- `game.h` declares `game_transform_path()` and timing/event functions but no explicit campaign-selector API; no inferred ability to select campaigns from that header.
- `guitext.h` has `GSTR_SLOT_EMPTY` and `STRINGS_MAX 800`; the existing GUI has an empty-slot text mapping, but this does not establish save enumeration semantics.
- Name mapping configurations affect symbol/platform linkage, not mode selection.
- Large `syndre.sx` (1.85MB) is accessible only in part through current GitHub connector; full source retrieval via raw endpoint was rejected for response size and line-ranged fetch_file returned empty. No assembly campaign, revolt or level loader function claims are established.
- Repository checkout attempt in local analysis environment failed due unavailable github.com DNS. This was not a build or gameplay test.

Assessment: There are at least two documented file-IO layers (LbFile family from native C and DOS low-level wrappers); save isolation cannot be implemented safely solely by changing a filename in `game_data.c` without examining all callers and screens. Campaign mode identity needs to be established prior to both listing and data IO. Recommend evidence-gated interface specification, not speculative edits.

Next steps: obtain indexed assembly access via supported GitHub capabilities or a usable local checkout, inventory all save path references and campaign-selection logic, then document exact hook points and compatibility policy. No changes to engine. No tests. Handover version: MB-AUDIT-0.7.

## Audit 0.8 — FULL ASSEMBLY SEARCH ACCESS RESTORED (2026-11-01)
**Major correction:** `mcp__GitHub__fetch_blob` by SHA successfully returned all 1,848,334 bytes of `engine/src/syndre.sx`, 85,803 lines. Prior blocker about unavailable indexed assembly is RESOLVED. Inspected exact assembly lines. No compilation or runtime tests.

Verified gameplay hooks (line numbers on the current main blob SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf`):
- `mission_briefing` line 1207; caller line 54661 uses `brief_text`. The menu handler calls `load_text` with `current_levno` at lines 54622-54625.
- `map_selection` line 31709; world map selection puts mission ID from `data_5b071` into `current_levno` around lines 32017-32022 and 32062-32066.
- `load_map_level` line 27845 updates `current_levno`, reads an RNC level block and loads `mission_load_files` (lines 27873-27904). Mission resource descriptor `mission_load_files` at 82620 includes `data/map??.dat` dynamically patched via map number.
- `load_text` line 55249 and path patterns around 82526 include `data/miss%02.2d.dat` and `data/miss%01d%02.2d.dat`. Need inspect language/variant branching and validate actual game assets.
- `LoadSaveGame` line 30354 uses `ac_load_save_text` at 30397 and `ac_load_player` / `ac_save_player` at 30627 / 30643 for 10 slots.
- Main menu also requests `ac_load_save_text` repeatedly at 54575-54582. These provide **two verified save-list callers**.
- Legacy assembly implementations of `ASM_load_player`, `ASM_save_player`, `ASM_load_save_text` remain in assembly around 55293–55448 and use `aCSyndSave` string `synd/save` at 85010, while native C save functions use `save/%02d.gam`. The active `ac_` wrappers call C versions; do not assume old assembly path string is live.
- `next_mission_countries` symbol at 85318, but revolt logic and campaign selection not fully traced.
- `current_levno` appears in 13 assembly lines; `level__MapNumber` in 8. Further inspect mission ID resolution and mode variation before implementing new selectable modes.
- IMPORTANT: normal GitHub `fetch_file` returned empty for huge assembly, but `fetch_blob` succeeded. Future sessions should use the SHA recorded here or refresh it from Git tree.

Integration significance: confirmed concrete seam from map selection -> mission ID -> briefing text -> mission loading, and from menu save list -> native save wrappers. This is an evidence-backed starting point, **not proof of custom-mission compatibility**. Continue tracing exact file layout, objective semantics and mode switching before any code change.
Status: full static assembly indexing and targeted reading; no engine code modified; no build/runtime testing. Version MB-AUDIT-0.8.

## Audit 0.9 — Mission resource filenames and territory AI (2026-11-01)
Static source inspection of complete assembly blob SHA `c35e226...`, line references:
- `load_text` at lines 55249-55285 chooses English `data/miss%02.2d.dat` or language-prefixed `data/miss%01d%02.2d.dat`, then calls `LbFileReadRNC`; these are concrete mission briefing resource paths. Do not assume string format/editor grammar without examining original assets.
- `mission_load_files` at lines 82620-82663 holds a dynamically formed map filename `data/map??.dat` plus fixed `data/col01.dat`, `data/hblk01.dat`, sprites, animation resources. `load_map_level` changes the map-number characters at 27881-27904. Map reference and mission ID are distinct inputs.
- `map_selection` resolves a selected territory into `current_levno` via `data_5b071` at 32017-32022 / 32062-32066.
- `ComputerPlaysMap` at 11896-11997 chooses territory targets with `random` and writes `next_mission_countries`. This is evidence of existing dynamic strategic opponent logic, NOT evidence of random mission objective generation or revolt mission logic.
- Assembly `LoadSaveGame` calls native save wrappers for ten UI slots. Historical `ASM_*save*` functions and `synd/save` string remain in assembly but active wrappers are the confirmed call sites; verify runtime before assuming older paths are used.
- Feasible first experiment is a fixture that changes only a single test mission briefing/data asset in an isolated runtime data directory, with checksum protection and original legal assets supplied locally. This is a PROPOSAL, NOT implemented or approved as a gameplay change.
- Risk assessment: fixed mission ID resource patterns and 2-digit filename formation suggest possible indexing constraints; confirm actual valid ranges and handling of campaign expansion before assigning new IDs.
No code changed, compiled, or executed. Version MB-AUDIT-0.9.

## Audit 1.0 — Tactical mission dispatch, strategic clock and objective array (2026-11-01)
Read complete assembly blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`, with line numbers specific to that blob.
- Single-player `process_action` path around lines 28531-28542 pushes `current_levno` and `level_fname`, calls `load_map_level`, then `init_players_people`. This completes the source-level trace from world-map selection to tactical level initialization.
- `load_map_level` at 27845 chooses RNC mission data and map resources as documented in Audit 0.8/0.9. No data-editing or alternate mission loader exists in inspected path.
- Periodic strategy update at 11603–11640 iterates 50 states, calls `AdjustStateHappiness` and `GetRevenueFromCity`, adds credits, then calls `ComputerPlaysMap`. These are concrete future World Domination investigation points. The exact revolt condition/dispatch is not yet proven.
- `adjust_tax` at 33082–33106 clamps a per-country tax value to 0–100 in `data_553a0`. The exact connection to territory rebellion is still being audited.
- `init_computer_players` at 7052–7080 walks `level__CPObjectives` with 0xF (15-byte) stride for up to 0x80 (128) entries; code at 9279–9289 traverses linked objective children with equivalent 15-byte stride. This independently corroborates the packed `CPObjective` layout. Treat 128 as an existing array capacity, not an approved custom-editor limit or safe extension capacity.
- No user-provided game retail resource files were analysed. Binary data grammar and new mission ID ranges remain unverified.

Risk: strategy simulator and tactical game share fixed-memory country/mission fields; any new mode needs stable identity and non-intrusive dispatch without changing clock/tactical timing in classic campaigns.
Next: trace happiness/revolt trigger to mission selection, campaign Original/American mode decision, and mission result transitions. Define testable proof-of-concept only after source-confirmed dispatch routes.
Tests: static reading only. No source modifications, compilation or gameplay testing. Handover version MB-AUDIT-1.0.

## Audit 1.1 — Happiness calculation and tactical outcome presentation (2026-11-01)
Source: complete `engine/src/syndre.sx` blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`. Line numbers from that blob.

Verified:
- `AdjustStateHappiness` lines 11759–11850 operates on per-country state. It tests ownership/neighbour adjacency and increments a country-state short field by 2 in an observed branch (11775–11798). It then compares `data_5539f` to 30, adjusts the state value, and constrains the result to the range 0–255 (11803–11844). The mapping of `data_5539f` to an exact user-facing tax field is not independently proven, but prior `adjust_tax` updates `data_553a0` and the revenue routine reads `data_5539f`.
- Periodic strategy update (11603–11640) calls `AdjustStateHappiness` for 50 country records before `GetRevenueFromCity` and `ComputerPlaysMap`. This establishes the update path, but **does not establish the revolt trigger**.
- `set_mission_complete` (37914–37932) draws completion text; `set_mission_failed` (37936–37945) draws failure text. These alone are presentation routines, not proof of campaign-state mutation or mission unlock effects.
- `reset_mission_info` (25849–25903) clears mission state and sets game_speed; important not to change timing when implementing new mode.
- Current source still contains no implemented campaign save silo or new mission builder.

Outstanding: locate branch where low happiness triggers rebellion and determines which mission to replay; trace caller(s) of mission-complete/failed routines to territorial progression. Trace American Revolt mode selection and campaign identity, then draft small, testable integration seam.

Verification: full assembly blob inspection; **no executable built, no gameplay tests, no game-code changes**. Handover version MB-AUDIT-1.1.

## Audit 1.2 — Mission outcome flags and campaign state mutation (2026-11-01)
Inspected full assembly source blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`, line numbers refer to this exact blob.
- `level_failed` (451–469) sets failure bit 0x4 of `data_60afc` unless terminal state bits already set, plus audio/status bookkeeping.
- `level_complete` (473–489) calls `set_mission_complete`, sets completion bit 0x2 of `data_60afc`, and sets status/audio bookkeeping. Existing mission resolution uses flag bits, not simply a UI label.
- A menu/post-mission routine checks the same completion bit 0x2 at line 55045. It conditionally changes `country_states` and ownership array `data_5539e`, resets two per-country values `data_5539f` and `data_553a0` to 0x1e (30) at 55077–55079. Therefore campaign-state mutation DOES occur after completion, outside the display-only text helpers. The precise branching between capture/revolt or special states remains to be mapped.
- `set_mission_complete` (37914–37932) remains display text routine; `level_complete` is the true trigger for flag changes.
- `set_default_player` (26165, 26191–26194) initialises per-country values; `data_5539f` and `data_553a0` start at 30, and country happiness starts at zero before subsequent updates.
- Mission result integration for a custom scenario should initially observe these terminal flags and the existing campaign handler, not overwrite the original campaign logic. Actual extension implementation deferred pending comprehensive branch tracing.
- No direct isolated rebel-mission selector proven yet; continue examining special `country_states` values, selection and mission ID mapping.
Verification: static assembly reading only; no engine code changes, Windows build, or runtime tests. Handover MB-AUDIT-1.2.

## Audit 1.3 — Campaign endgame and mission-result mutation (2026-11-01)
Source evidence: `engine/src/syndre.sx` SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf` (all lines below are for this blob).
- `level_finished` at lines 54998 onward checks completion flag bit 0x2 at 55045. It updates country happiness and ownership in `country_states` and `data_5539e` (~55047–55069), resets two related territory values to 30 (~55071–55079), then counts territories controlled by active player with happiness below 0xFF across 50 entries (~55086–55104).
- If the count is 50, `level_finished` plays the endgame win animation and sets terminal flag bit 0x10 (~55103–55130). This identifies a protected existing campaign win condition that optional World Domination must not silently replace in Original Campaign.
- Map-selection routine checks `can_i_play_this_city`, then maps selected city via `data_5b071` to `current_levno` (~32002–32023). An alternate event/mission selector for random rebellions must be upstream of (or explicitly compatible with) this pipeline.
- `AdjustStateHappiness` adjusts per-country happiness, clamps to 0..255 (~11803–11844). This function alone does not directly dispatch a rebellion mission. The precise rebellion predicate remains unverified.
- Further analysis needed: follow all `country_states` comparisons with 0xFF and callers of `can_i_play_this_city`; map original campaign versus American Revolt selection and endgame boundaries.
- Existing `level_finished` is a high-risk integration point shared with mainline code. Do not alter terminal flag 0x10, completion flag 0x2, or 50-country endgame condition without campaign-aware compatibility review.

Status: STATIC AUDIT and handover update only; no code changes, builds or runtime tests. Version MB-AUDIT-1.3.

## Audit 1.4 — Playable-city gating and loss flow (2026-11-01)
Static source: `engine/src/syndre.sx` main blob SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf`; line numbers belong to this snapshot.
- `can_i_play_this_city` at 32871–32965 gates territory selection, with world-map cheat bypass, owner comparisons using `data_5539e`, `country_states` value 0xFF, special mission-number check `data_5b071 == 1`, and adjacency path over `data_5b069`. Hence a new scenario dispatcher must preserve classic city eligibility rules and must not blindly replace `current_levno`.
- `draw_connecting_cities` around 32663–32755 also calls `can_i_play_this_city` when showing map connections; introducing mission availability needs UI and execution consistent or map highlights may lie.
- `level_finished` failure route 55140–55192 scans agent/cyborg data and can trigger loss animation, terminal flag 0x10 and `set_default_player` when no surviving agents. This is another protected original campaign lifecycle transition distinct from mission failure display.
- `country_states` comparisons with 0xFF appear in multiple world-map, selection and result paths, but a precise named rebellion predicate is still not conclusively identified. Do not equate every 0xFF test with revolt.
- Original/American campaign switch still unverified; explicitly avoid claiming it was found.

Integration guidance: campaign mode extension must coordinate map selectable-city presentation, actual tactical mission dispatch, and loss/terminal state routing. New-mode custom scenarios should use their own validated identity while preserving classic eligibility and endgame semantics.
Tests: static assembly analysis only. No game code modified, compiled or run. Handover MB-AUDIT-1.4.
