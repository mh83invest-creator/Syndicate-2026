# Syndicate 2026 — Graphics Audit Continuity Index

Updated: 2026-11-01 (Australia/Brisbane)
Active isolated branch: `audit/graphics-assets-2026-11-01`
Forked from: `feature/v0.9-visual-showcase` at `d42db5258f2786ea26a8e6e3ee235e09fa404cf6`
Protected original playable-source baseline: `main` at `a22b0b9580a9710b4e693e88854351835ef1ee40`

## Read in this order
1. `docs/HANDOVER_POLICY.md`
2. `docs/GRAPHICS_AUDIT_CONTINUITY.md` (this file)
3. `docs/audits/2026-11-01_graphics_asset_parallel_audit.md`
4. `docs/CONTINUITY_INDEX.md` for the concurrent original audit's snapshot at fork.
5. `docs/releases/v0.9.md`, `docs/SOURCE_AUDIT_PALETTE_GROUPS_AND_BUILD_2026-11-01.md`, and existing original graphics/menu asset analyses.

## Stage 1: graphics asset evidence reconciliation
Status: COMPLETE as a documentation-only audit stage. No new visual inspection, frame decoding, or engine test is implied.
Handover: `docs/audits/2026-11-01_graphics_asset_parallel_audit.md`
Exact scope: reconcile already independently documented original asset sizes, palette sharing, menu animation findings and source rendering limitations.
Engine modifications: none.
Tests: GitHub access and report read-back; no native compile, Windows game playtest or graphics equivalence tests.

## Parallel development policy
- This branch: read-only graphics asset analysis and dated handover documentation only.
- `feature/v0.9-visual-showcase`: separate source/engine/visual development audit workstream.
- `feature/mission-builder-audit`: mission builder workstream.
- `main`: protected v0.7 reference. No direct changes.
- On publication, compare branches, resolve docs conflicts intentionally, and review before merging.
- Do not put commercial game data or reconstructed copyrighted frames in public GitHub.

## Next stage
Trace hspr-0.tab against original assembly drawing code and record verified metadata boundaries. Continue with tile/block resource mapping, sprite animation classes, explosion frames, then per-mission palette and lighting feasibility. Add a dated report to `docs/audits/` for each completed stage and append the result here; distinguish asset parsing, automated checks and real Windows rendering.

## Parallel graphics audit coordination — 2026-11-01

A third documentation-only workstream has been established:
- Branch: `audit/graphics-assets-2026-11-01`, forked from `feature/v0.9-visual-showcase` at commit `d42db5258f2786ea26a8e6e3ee235e09fa404cf6`.
- Continuity: `docs/GRAPHICS_AUDIT_CONTINUITY.md` on the graphics branch (commit `82c0b4c4e54c05ddf5a99f666b1ba1a1523a766d`).
- Stage-1 evidence handover: `docs/audits/2026-11-01_graphics_asset_parallel_audit.md` on the graphics branch (commit `a012ee8e32b8c68c62a9c0fa6d5ae6fd63026880`).
- Scope: inventory and source-to-asset correlation of original tiles, building faces, sprites, animations, effects, palettes, menu art and constraints relevant to proposed night lighting. Stage 1 reconciled existing evidence only; it is **not** a new pixel-level audit, renderer patch, CI build or Windows test.
- Ownership: core source and v0.9 graphics implementation remain with `feature/v0.9-visual-showcase`; mission definitions and save-mode interfaces remain with `feature/mission-builder-audit`; graphics workstream records read-only asset findings and proposals, with no gameplay edits.
- Integration contract: any proposed modifications to tile indices, palette selection, sprites, mission resource filenames, asset loaders, render buffers, map/object representation or screen overlays must be reviewed across all three workstreams before implementation. Keep the original and American Revolt modes, campaign-siloed save requirements, and protected v0.7 reference intact.
- Confidentiality: original retail assets and reconstructed copyrighted frames must not be committed publicly.
- Synchronisation: each branch is independent; the presence of this link does not mean another branch has received or merged its commits. Updates are not automatic; new findings need an explicit dated cross-branch handover.
- Verification: GitHub documentation changes only. No source build, Windows gameplay test, image equivalence test or new graphics feature.

## Graphics sprite rendering follow-up — 2026-11-01
- Report: `docs/audits/2026-11-01_sprite_rendering_followup.md`.
- Verified: C display and sprite remap paths; 16-colour and MCGA rendering paths must not be conflated.
- Unresolved: the original assembly sprite-table entry layout and rendering consumer; oversized assembly source line queries returned empty data.
- Status: source documentation only; no new decoded art, engine build, Windows visual test or game-code modification.
- Next: source extract and private sprite-table record cross-check. Other audit branches unchanged.

## Graphics block-face cross-check — 2026-11-01
- Report: `docs/audits/2026-11-01_graphics_block_face_crosscheck.md` (commit `36ea7097aefa9be777a13d6cc5ea1d4862df5db5`).
- Scope: independently reconcile existing map-column rebase, block face index selection, descriptor component and zero-offset sampling findings.
- Confirmed as existing evidence: 12,288 map pointers; selected block IDs index 24-byte records, with draw-path component fields +0/+4/+12/+16/+20. Flag-test semantics do NOT establish illumination or geometry classifications.
- Not performed: new binary decoding, compilation, Windows visual/performance testing or any engine change.
- Next: original sprite-table assembly consumers; then private resource-to-pixel validation and scoped night-lighting feasibility.

## Sprite pointer setup source audit — 2026-11-01
- Report: `docs/audits/2026-11-01_sprite_pointer_setup_source.md`; report commit `7ba11371c089ea6641516f0e9539510b813e084f`.
- Confirmed from Bullfrog library C source: LbSpriteSetup performs offset-to-pointer adjustment for TbSprite.Data; reset can reverse that conversion. Sprite drawing preparation checks dimensions, encoding uses width/height and the data pointer.
- Crucial limit: no proof yet that private `hspr-0.tab` uses TbSprite or any particular entry stride. No direct assembly consumer analysis or newly decoded sprite data was performed.
- Verification: source reads and report commit. No executable build, game test, or modification to gameplay. Next inspect original assembly consumer and table layout.

## Original sprite descriptor assembly trace — 2026-11-01
- Report: `docs/audits/2026-11-01_original_sprite_table_assembly_trace.md`; commit `3b2a01f1a24ab365d8073d82f5be2f9b4e0ca445`.
- Full original `syndre.sx` blob successfully fetched, resolving the previous large-file access blocker. `init_level_data` calls `ac_LbSpriteSetup` with `h_sprites`, end pointer and `h_sprites_data`.
- Direct drawing routines read sprite descriptor 32-bit pointer at +0, dimensions at +4/+5; 10,812-byte decoded sprite table therefore has a 1,802-entry *candidate* six-byte interpretation, pending private data checking.
- Selected animation loops use 10-byte element indexing and sprite-table byte offsets. Exact valid element and sprite counts, rendering equivalence and lighting compatibility are unverified.
- Tests: assembly source read and arithmetic only; no new private decoding, compilation or Windows gameplay. Next: CRC-checked private 6-byte/10-byte record validation.

## Packed sprite descriptor ABI and asset validation gate — 2026-11-01
- Report: `docs/audits/2026-11-01_sprite_descriptor_abi_validation_gate.md` (commit `2b347433b42b194edceeb09bc255e93586693608`).
- Source cross-check: packed `TbSprite` in `engine/bflibrary/include/bfsprite.h` with pointer at +0 and, when `SPRITE_FORMAT_V2` is not selected, byte dimensions at +4/+5; direct assembly draw fields and `ac_LbSpriteSetup` align with this layout.
- 10,812 bytes / 6 = 1,802 potential entries. Asset-level record count, pointer ranges, empty entries and animation reference integrity remain **unverified** because private retail archives are not available in this session.
- Verification: GitHub header and assembly source read; no new data decode, build or Windows game test. Next: private archive availability and actual six-byte descriptor bounds validation.

## Animation frame/element source trace — 2026-11-01
- Report: `docs/audits/2026-11-01_animation_frame_element_source_trace.md`; commit `2024f0a681c1179a6c5c49d4307411f12226917c`.
- Source confirmed: frame lookup uses 8-byte stride; frame +0 references ten-byte animation elements; frame +4 participates in sound control; element +0 addresses a sprite by table-relative byte offset, +2/+4 position, +6 drawing parameter and +8 next element reference.
- Decoded file sizes imply **candidate**, not verified active, capacities: hfra-0.ani 8,948 frame slots; hele-0.ani 10,486 element slots; hspr-0.tab 1,802 sprite entries. hsta-0.ani semantics remain open.
- Tests: original assembly blob inspection and divisibility calculations only; no private data record validation, Windows rendering or game code changes. Next trace starts_ani and validate private animation link chains.

## Animation starts table audit — 2026-11-01
- Report: `docs/audits/2026-11-01_animation_starts_source_trace.md`; commit `9bc60d769ee93f2a7d437ed1e6855d8d06616d4e`.
- Source confirms `starts_ani` is addressed with 16-bit word entries. Ordinary scaled indices and fixed offsets, including 0x4b0 and 0xf3e, are used in distinct animation and drawing paths.
- Existing 3,940-byte decoded hsta-0.ani length is compatible with 1,970 candidate word entries, not 1,970 validated active sequences. Some offsets serve separate logical groups; their semantics and bounds require private asset validation.
- Tests: original assembly source read and size arithmetic only. No game-code modifications, private asset decode, build or Windows runtime tests. Next: inspect starts/frame/element cross-reference and termination rules.

## Animation progression and termination — 2026-11-01
- Report: `docs/audits/2026-11-01_animation_advancement_termination.md`; report commit `e7b60c330e21b0f9613d46617455fa2e8ab0ff75`.
- Original assembly source shows `frames[current]+6` as a next-frame state in examined routines, with `starts_ani` reset/default values. Element +8 provides subsequent ten-byte element index. Both types of transitions need private data validation for end bounds, loops, invalid links and sentinel behaviour.
- Verification: original assembly read. No new data decoding, compilation or Windows tests. No game files or other workstreams changed.

## Animation chain bounds / cycle audit — 2026-11-01
- Report: `docs/audits/2026-11-01_animation_link_bounds_cycle_audit.md`, commit `35e623072f3b80b8b28e286457f9914e5b05c6f7`.
- Inspected original assembly confirms zero-index element chaining terminates at the animation-elements base pointer, while positive element indices continue without a visible end-bound check or visited-node limit in the inspected loops. This identifies a conditional risk only, NOT corrupted source assets or a reproduced runtime failure.
- Frame progression may take frame+6 or reset from starts_ani depending on state; no universal end-frame convention has been proven.
- Next: private CRC-checked hfra/hele graph traversal with bounds, cycle checks and comparison to Windows playback. No code or gameplay changes; no runtime validation.

## Agent-specific animation sprite selection — 2026-11-01
- Report: `docs/audits/2026-11-01_agent_animation_sprite_selection.md`, commit `f35f1a0ac59a69897b869738c267912b7fcccdb2`.
- The original `draw_agent_element` path may replace an element's sprite offset using agent state and an alternate descriptor selection before calling `DrawASpriteHires16_A`. An offline viewer cannot assume the raw element +0 sprite is always the displayed agent sprite.
- Inspected links and descriptor selections only test against their respective *base* pointers, not observed end bounds. Conditional risk only; no malformed asset or crash established.
- Verification: source-only assembly inspection, no new asset decoding, build, Windows testing or gameplay modifications. Next trace alternate selection state and validate private sprite/animation record references.

## Agent sprite variant selection arithmetic — 2026-11-01
- Report: `docs/audits/2026-11-01_agent_sprite_substitution_arithmetic.md`; report commit `baa612aeb0809d853c78f9b44ff5831130f53efe`.
- Assembly demonstrates six-byte sprite-offset division, an eight-value special range after subtracting 0x29, and state-derived variant selection using a byte from `data_5e4ab` with base index 0x461. The fallback preserves the raw element sprite reference.
- State byte's meaning remains unknown; do not infer outfit, weapon, direction or other labels without corroboration. Additional source path corroborates variant substitution.
- Source-only, no private asset-byte test, Windows rendering, compile or gameplay changes. Next: trace state-byte provenance and numeric bounds.

## Agent sprite player-state provenance — 2026-11-01
- Report: `docs/audits/2026-11-01_agent_variant_player_state_provenance.md`; report commit `8b42c0145e01fb6219b85ff43348b2455b562996`.
- `data_5e4ab` is written in player/network slot initialisation and menu state paths; independently, its byte indexes `player_colour_for_map` and contributes to agent sprite variant selection. This is stronger evidence of player-side graphical identity, but **does not yet prove exact semantic labels or colours**.
- Source-only inspection; no private art inspection, native build or Windows runtime. Next: trace `player_colour_for_map` and bounds/mode behaviour. Other branches untouched.

## Player map-colour static table — 2026-11-01
- Report: `docs/audits/2026-11-01_player_map_colour_lookup.md`; commit `08eaf818215aa0bda5edc1bdfbe60c499559ac66`.
- Original assembly defines exactly ten table bytes at `player_colour_for_map`: hex `06 07 0E 03 0B 0C 0D 0F 06 00`. Multiple draw paths index these using player-related data, while another accesses the lookup in WScreen rendering.
- These are indexed lookup values, not proven RGB colours, valid player count or validated bounds. No engine changes or Windows/private-resource tests. Next inspect valid source indices and current palette mappings.

## Player colour-index limits — 2026-11-01
- Report: `docs/audits/2026-11-01_player_colour_index_limits.md`, commit `9f3a651b4b9aa2847356de10d1cdbf48bbbf2cf9`.
- Assembly confirms an eight-slot search in one configuration path and successive slot identifiers during player initialisation; neither proves a universal global limit. The static ten-entry map-colour lookup has eight distinct first values, a ninth repeating the first, and a tenth zero.
- Palette RGB values and all index bounds remain unverified; no engine edits, asset parsing or Windows tests. Next verify active palettes and source indices against private resources.

## Mission palette loading source trace — 2026-11-01
- Report: `docs/audits/2026-11-01_mission_palette_loading_path.md`, commit `cef9c1faf16c638808baf285a6fd8c25c2766db7`.
- Original assembly: level palette IDs 1–5 repeat for 50 entries, followed by two zero bytes; current_levno indexes this table in the inspected file-selection branch. The palette resource loads into GraphicsPalette using LbFileReadRNC.
- The subsequent ac_LbPaletteSet call receives WScreen. Exact implementation semantics and runtime palette effects require further tracing. RGB mapping still requires original palette resource bytes.
- No engine changes, private data decoding or Windows tests. Next trace ac_LbPaletteSet and fades/effects.

## Palette set callers and fades — 2026-11-01
- Report: `docs/audits/2026-11-01_palette_set_fade_call_trace.md`; commit `045ce2b7e90dc08cfe5db65cb237651a9015abd0`.
- Direct assembly calls set palette with GraphicsPalette (main-loop branch), m_select_pal (menu), WScreen (after mission palette load), and one conditional VGA path. WScreen argument is source-verified but semantics remain unresolved; do not claim an actual bug.
- Bullfrog C library LbPaletteFade interpolates indexed palette bytes, with blocking and stepped modes. Actual runtime fades and RGB effects remain untested; no game changes.

## Cross-workstream coordination — 2026-11-01
Three streams coexist: mainline core audit on `main`, independent graphics source/asset audit on this `audit/graphics-assets-2026-11-01` branch, and Mission Builder/World Domination audit on `feature/mission-builder-audit`. Shared mainline coordination lives in `docs/SHARED_DEVELOPMENT_HANDOVER.md` on `main`; Mission Builder coordination in `docs/mission-builder/HANDOVER.md` on its branch.

Graphics audit must remain independent of the `feature/v0.9-visual-showcase` graphics implementation branch. The last reviewed graphics handover includes indexed map/player colours, mission palette path and palette-set/fade tracing; these are static observations, not validated RGB values or runtime art changes. Mainline RNC/resource bounds findings and Mission Builder RNC/Huffman briefing investigations overlap in loader risks but are not permission for a graphics audit patch.

Integration gates: no changes to palette tables, sprite indices, mission resources, map graphics, asset loaders or render buffers without coordinating with mainline and Mission Builder. Retain v0.7 gameplay baseline, original and American Revolt campaigns, 16 FPS tactical loop, siloed saves between missions only, and no mid-mission saving. Do not commit retail copyrighted art. Each branch must publish source-verified documentation and explicitly differentiate static audit from compiled/gameplay tests.

This is a documentation-only synchronization note; no cross-branch merge or gameplay verification is implied.

## SDL palette ABI versus original WScreen call — 2026-11-01
- Report: `docs/audits/2026-11-01_palette_set_abi_wscreen.md`; report commit `7a4cf6becf073492e711a886fd73cbf57be29289`.
- Both SDL1 and SDL2 LbPaletteSet implementations treat their argument as RGB triplets (six-bit source channels). Original mission loader passes WScreen after loading GraphicsPalette, which remains an unresolved buffer/alias discrepancy, **not a confirmed bug**.
- No source change, builds, private palette decoding or Windows tests. Next trace WScreen buffer alias and palette state at mission loading.

## WScreen buffer alias and palette call — 2026-11-01
- Report: `docs/audits/2026-11-01_wscreen_palette_alias_source_trace.md`, commit `4e8c14452a444830ae2565cec97309369ab2dcaf`.
- Source confirms mission load reads palette to GraphicsPalette, clears WScreen and passes WScreen to palette setter; a mode-specific branch derives UScreen and BScreen from WScreen, and another routine temporarily aliases WScreen to BScreen for rendering. SDL LbPaletteSet expects RGB triples, so the source-level pointer roles differ. Runtime effect unverified and no bug fix authorised.
- Tests: assembly/library source inspection only, no builds or Windows rendering. No engine or other branches changed.

## WScreen allocation, palette separation and phase bounds — 2026-11-01
- Report: `docs/audits/2026-11-01_wscreen_allocation_palette_phase_bounds.md`; report commit `10d335e3ca14e5fe73fae323c100c770f41399a3`.
- **Important pointer correction:** exported original `WScreen` is loader-allocated separately (256,048 bytes Vres16; 256,016 bytes MCGA). `lbDisplay.WScreen` is a *different field*, set to `VGABuffer + 307,200` within a separately allocated 614,416-byte VGA buffer. Do not conflate them; `ApSpriteDrawMCGA` temporarily bridges the two pointers.
- `GraphicsPalette` has its own load entry and pointer. The mission-load routine loads it and then clears/passes original `WScreen` to a palette-set function that expects colour bytes. Runtime meaning is still unverified; **no fix authorised**.
- Source-derived title/mission/post-level scratch pointer starts are all within the nominal WScreen allocation in the report's comparison. This does **not** establish valid object extents, runtime modes or drawing safety.
- Cross-references prior core audit docs for pointer transitions, loader and SDL surface ownership. Verification: direct source read and arithmetic; no private asset decoding, Windows build or visual test. Other development branches remain untouched.

## Resolved high-resolution palette blackout and restore — 2026-11-01
- Report: `docs/audits/2026-11-01_palette_blackout_restore_source_trace.md`; commit `1c6a3769ee003852f2b14d60902531876f95f671`.
- **Important resolution to preceding palette/WScreen investigations:** `LbScreenSurfaceClear` is implemented in original assembly (`syndre.sx:75307-75340`), not an unresolved external graphics library routine. For `DrawFlags & 0x2`, it zero-fills the first **128,000 bytes** of exported `WScreen` via `LbMemorySet`; the immediately subsequent `ac_LbPaletteSet(WScreen)` therefore receives 768 zero colour bytes: a temporary black palette, subject to successful calls.
- The active mission loop initialises its `ebx` counter to zero; its third iteration (when `ebx==2`) sets `GraphicsPalette` after the screen-swap call, consistent with palette reveal. **Not a confirmed renderer bug; do not patch.** Other draw modes do not execute the same zero-fill path.
- Source inspection verified; no Windows screenshots, runtime timing, gameplay tests, or private palette decoding. Next: phase ordering and non-hires palette paths.

## Mission startup palette and screen-swap ordering — 2026-11-01
- Report: `docs/audits/2026-11-01_mission_start_palette_swap_order.md`; commit `5a8414a806042c205ac1083a3cd439bf2d01035c`.
- High-res mission startup zero-fills the initial 128,000 bytes of game `WScreen`, then uses its first 768 bytes to install a black palette. In `syndicate`, the `ebx=2` normal palette restoration is **after** the third active-loop swap call, provided loop progress is uninterrupted. This is source-supported blackout/reveal sequencing, **not proof of three visibly black frames or a timed fade**.
- There are two cursor/swap-dispatch routes determined by `data_60b4f`; both precede palette restoration. The SDL `swap_wscreen` copies from `VGABuffer` in Vres16, or from game `WScreen` in MCGA. The clearing routine does not zero the same buffer on other graphics-mode paths; don't extrapolate the blackout behaviour.
- Source-only testing; no Windows runtime, screenshots, original assets or performance measurement. Next: DrawFlags non-Vres16 / mode transition and `data_60b4f` handling. No engine, main or parallel-branch changes.

## Full-redraw flag, 25×16 dirty grid and alternate swap modes — 2026-11-01
- Report: `docs/audits/2026-11-01_full_redraw_flag_alternate_swap_modes.md`; commit `1c038e76c389062d969fb53c5000ed4bb33b630d`.
- Assembly establishes `data_60b4f` as a pending full-redraw signal written by `set_all_changes`, which marks all 400 cells in `data_5db2c`; `set_block_changes` can mark individual cells. `swap_screen_vres16` consumes/clears the flag to select the full-update wrapper; `copy_back` separately uses both the flag and dirty-cell values.
- Main-loop order matters: screen swap occurs **before** scroll calls and `copy_back`. Scrolling may set the flag after the current swap, allowing `copy_back` to use it and a subsequent swap to consume it. Preserve this two-stage update sequence.
- `DrawFlags`: default Vres16 `0x2`; CLI `-L` selects `0x4` (named `DrwF_Unkn04`, not `DrwF_ScreenMCGA=0x1`). The `0x4` swap helper calls `nullsub_6`, which only returns, before the common non-DOS presentation. Actual `swap_wscreen` chooses `VGABuffer` 640×480 vs game `WScreen` 320×200 based on high-res bit. Menu phases may save/temporarily change flags.
- Source review only; no build, Windows graphics testing or private data. No game code or other branches changed. Next: trace `copy_screen_cube` consumption/clearing of dirty markers and determine impact on map lighting overlays.
