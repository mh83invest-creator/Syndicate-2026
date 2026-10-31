# Syndicate 2026 — Continuity Index

Updated: 2026-11-01 (Australia/Brisbane)
Repository: https://github.com/mh83invest-creator/Syndicate-2026
Audit/development branch: `feature/v0.9-visual-showcase`
Protected gameplay baseline: `main` at `a22b0b9580a9710b4e693e88854351835ef1ee40`

## Mandatory reading order on resumption
1. `docs/HANDOVER_POLICY.md` — mandatory per-stage/per-build documentation and verification.
2. This index — latest completed/in-progress work and navigation.
3. `docs/SOURCE_AUDIT_DECISION_REGISTER_2026-11-01.md` — latest forensic feasibility/risks.
4. `docs/SOURCE_FORENSIC_AUDIT_2026-10-31.md` and related specialist audit files — evidence and unfinished static/runtime gates.
5. `docs/releases/v0.9.md` and `docs/releases/v0.9A.md` — approved roadmap and actual code/build state.
6. `docs/releases/v0.7.md` — protected known-good gameplay reference. Older failed experiments: `docs/releases/v0.8.md`, `docs/releases/v0.8.1.md` on their respective branches.

## Historical handovers (retained without rewriting conclusions)

| Stage | Document | Recorded status |
|---|---|---|
| v0.7 | `docs/releases/v0.7.md` | Historical user-playtested build; restored source CI succeeded |
| v0.8 | `docs/releases/v0.8.md` (feature branch) | Failed Windows gameplay test; do not merge |
| v0.8.1 | `docs/releases/v0.8.1.md` (isolation branch) | Windows compile pass, gameplay unverified |
| v0.9 roadmap | `docs/releases/v0.9.md` | Approved plan; no completed v0.9 visual features |
| v0.9A | `docs/releases/v0.9A.md` | One-line native window-title edit, uncompiled and untested |
| Forensic overview | `docs/SOURCE_FORENSIC_AUDIT_2026-10-31.md` | Substantial source reconnaissance; complete end-to-end audit NOT claimed |
| Decision register | `docs/SOURCE_AUDIT_DECISION_REGISTER_2026-11-01.md` | Static findings, feasibility constraints and remaining gates |
| Assembly navigation | `docs/ASM_ADDRESS_LABEL_INDEX_2026-11-01.md` | Original assembly address/label lookup index |
| Asset-free continuation | `docs/SOURCE_AUDIT_ASSET_FREE_CONTINUATION_2026-11-01.md` | Mission frame lifecycle, palette, input/focus and buffer investigation |
| Rendering/tests | `docs/SOURCE_AUDIT_RENDERING_AND_TEST_GATES_2026-11-01.md` | Render path and testing gaps |
| Geometry | `docs/SOURCE_AUDIT_FRAMEBUFFER_GEOMETRY_2026-11-01.md` | Planar display and offset geometry |
| Map format | `docs/SOURCE_AUDIT_MAP_FORMAT_AND_GEOMETRY_2026-11-01.md` | Column-pointer layout, synthetic comparisons |
| Scaler/GUI | `docs/SOURCE_AUDIT_RENDER_SCALER_GUI_BOUNDS_2026-11-01.md` | Conditional memory risks, not runtime crashes |
| Effects/profiling | `docs/SOURCE_AUDIT_EFFECT_POOL_AND_PROFILING_2026-11-01.md` | Candidate hotspots, no runtime benchmarks |
| Other investigations | `docs/SOURCE_AUDIT_GAMEPLAY_AI_AUDIO_2026-11-01.md`, `docs/SOURCE_AUDIT_MENU_HITBOX_STRATEGY_2026-11-01.md`, `docs/SOURCE_AUDIT_STARTUP_AND_CLI_2026-11-01.md`, `docs/SOURCE_AUDIT_PALETTE_GROUPS_AND_BUILD_2026-11-01.md`, `docs/SOURCE_AUDIT_RENDER_SCALER_GUI_BOUNDS_2026-11-01.md` | Specialist source-only findings |

## Current work-state handover — 2026-11-01
**Stage:** Continuity policy expansion / recovery reconciliation.
**User request:** Firm GitHub-uploaded notes after *every* completed build, process, audit and investigation, not only each release.
**Files changed:** `docs/HANDOVER_POLICY.md` extended; this index created. No engine, runtime, gameplay, data, workflow or protected main files changed.
**Source baseline:** v0.7 `main`; v0.9 branch retains the existing one-line native window-title change. The failed v0.8 camera prototype is excluded.
**Verification:** GitHub writes and read-back to be confirmed in this session; no compilation, automated tests, Windows execution or gameplay testing performed.
**Open audit work:** Complete remaining static source dissection, source/asset feasibility and rendering buffer questions, design test-only checks, then seek original retail asset/Windows run evidence. Do not silently implement or approve lighting or performance fixes.
**Next action:** Continue source-only audit from decision register and outstanding source references; publish a new `docs/audits/YYYY-MM-DD_<topic>.md` for each subsequently completed discrete stage and update this index.
**Rollback:** Documentation changes reside on v0.9 branch; `main` remains the verified gameplay reference.

## Publication protocol
For each completed stage: write handover → commit on active development branch → fetch it back from GitHub → update this index → fetch index back → report exact committed scope and verified/nonverified checks. If any step fails, report it without claiming completed publication.


## Completed audit stages — 1 November 2026

| Stage | Handover | Verification and status |
|---|---|---|
| Display/palette ownership C-source audit | `docs/audits/2026-11-01_display_palette_ownership.md` | Source read and static checks completed; no compile or runtime tests. Report commit `8b718f3d77ee2a1a0299fa2873c56bf261996018`. Overall forensic audit remains incomplete. |

**Next action:** Trace original assembly pointer assignments and Bullfrog SDL surface lifetimes as identified in the new report, while keeping `main` and existing gameplay code untouched.

| SDL2 surface/focus lifecycle | `docs/audits/2026-11-01_sdl_surface_focus_lifecycle.md` | Focused source-only review complete; no compilation or Windows playback. Report commit `a86741c7bee62b1f13f763aa5c114168d525bd90`. |

| SDL setup/Windows build configuration | `docs/audits/2026-11-01_sdl_setup_build_config.md` | Static screen setup and CI review complete; `LB_WSCREEN_CONTROL` effective definition remains unresolved. Report commit `7ec07024b79f9f72fa78dd221bdc69b07d79771a`. |
| WScreen build flag/SDL draw surface ownership | `docs/audits/2026-11-01_wscreen_build_flag_ownership.md` | Source default resolved (`no`); generated Windows config not tested. Commit `212b074cd67d39599a524ff747de5dce4e43f05c`. |
| Assembly screen-pointer phase transitions | `docs/audits/2026-11-01_assembly_screen_pointer_transitions.md` | Focused static audit completed, no build or runtime tests. Commit `857f5cc0c1f0534dd9ee2763b2a641ce9d7bb2bb`. Next: allocation boundaries and resource-loader aliasing. |
| Resource loader offsets | `docs/audits/2026-11-01_resource_loader_offsets.md` | Static source audit completed, runtime bounds unverified. Commit `d8b487346ea35c1f5e1abd212e08fc9baec3cfcf`. |
| Resource loader ownership/failure handling | `docs/audits/2026-11-01_data_loader_failure_semantics.md` | Static audit completed; failed-load leak possibility documented, no runtime tests or fixes. Commit `2004106b6c5dcf2cfaa5c71974e2fc82ac2ae5c8`. |
| MCGA descriptor and loader-return flow | `docs/audits/2026-11-01_assembly_loader_return_and_mcga.md` | Low-res buffer length decoded as 256016; local load return unchecked, alternate teardown unresolved. Commit `caa2b302531fbff060cbe4b4b71743cae8fe2a36`. |
| Original mode selection and teardown | `docs/audits/2026-11-01_mode_lifecycle_reachability.md` | Focused source trace complete; conditional descriptor mismatch noted, runtime untested. Commit `a9e8444f88acbb9f64fa7d6cb69326a6093fc03b`. |
| Private asset inventory / RNC headers | `docs/audits/2026-11-01_private_asset_inventory_rnc.md` | Existing uploaded game ZIP contains private map/sprite/palette resources; header inspection complete, decompression/runtime not tested. Commit `d7e5b583ae636a1b04087e780044c4e6c216741e`. |
| Historical private-resource hash parity | `docs/audits/2026-11-01_retail_resource_hash_parity.md` | 437/437 data members match v0.6 vs v0.8; does not validate engine changes. Commit `a823b6b462f6b5eecbeb9b479ee1f75a7c37d32d`. |
| RNC decompressor source trace | `docs/audits/2026-11-01_rnc_decoder_source_trace.md` | Original unpacker identified; header correlation pass; decoding/CRC runtime not performed. Commit `b337fd0bc234e2ff541ba09242e13b07f9751b11`. |
| RNC1 archive packed-integrity audit | `docs/audits/2026-11-01_rnc_packed_crc_integrity.md` | 143/143 compressed header lengths and CRC16 values pass; decompression unverified. Commit `8d328cbe945fd8cb337a2563b8aebadec9af8614`. |
