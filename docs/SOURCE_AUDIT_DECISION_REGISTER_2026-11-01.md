# Syndicate 2026 — Consolidated Source-Audit Decision Register (before original game assets)

**Date:** 2026-11-01 (Brisbane)
**Source branch:** `feature/v0.9-visual-showcase`
**Protected gameplay baseline:** `main` v0.7 at `a22b0b9580a9710b4e693e88854351835ef1ee40`
**Architecture decision:** **Keep SyndicatFX**. FreeSynd is a **read-only reference**, not an engine migration or source-code donor.
**Status:** A **static-source decision register**, not a final end-to-end certification. Code implementation remains frozen until scope/feasibility and runtime gates are satisfied. There are no new v0.9 gameplay changes.
**Inputs reviewed:** GitHub recursive tree, original 85,803-line assembly via blob SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf`, C libraries/ABI/CI, and user-supplied FreeSynd reference ZIP. The original retail data are not available in this execution environment.

## 1. Feasibility decision for the actual v0.9 requirements

| User-approved requirement | Static-source finding | Status before asset/runtime tests | Exact technical starting points |
| --- | --- | --- | --- |
| Keep original gameplay/camera/audio and mission timing | Branch matches v0.7 at **every pre-existing tracked source/CI blob except one-line window-title edit**; 16 FPS mission/30 FPS three menus source verified | **Preservable as baseline**, not a proof that a future modification will preserve it | `engine/src/game.c:142–226`, `syndre.sx:198–303,33330–33337,19352–19871` |
| Native Windows title | Earlier single-line `LbSetTitle("Syndicate 2026")` edit exists | Code change **present but not compiled/tested** | `engine/src/game.c:85` |
| Visible in-game title/menu/briefing improvement | Original animation and menu assets and draw routines identified; there is **no modern UI layout layer**; original hover bounds and font metrics hardcoded in assembly | **Potentially feasible**, but asset layout, mouse mapping and appearance unverified | `syndre.sx:54149–54300,55935–56119,10069+,10925+,1207+,31709+` |
| Different atmosphere for selected missions | Original level palette selection exists, with **five shared IDs repeated ten times**; sprite/map resource tables separate | **Technically plausible as a separate original-mode palette strategy**, but global palette substitution alone will recolour multiple missions and UI | `syndre.sx:27768–27793,82677–82684`; `spalette.c:50–99` |
| Scene-only colour grading | 4-plane 640×400 mission expands into 8-bit indices positioned at output rows 40–439, then SDL software blit; cursor/UI separation not proven | **Potential stage identified, visual correctness unverified** | `display.c:72–95,127–131,149–260`; `syndre.sx:27866–27880` |
| Actual streetlight pools, moving headlights, bright windows and neon | No existing native per-object light emitters or scene mask demonstrated; original tiles and actors draw through assembly with clipping/dirty change tracking | **NOT YET DEMONSTRATED**; may require distinct compositing/object-geometry work. Do not promise this as minor palette tweak | `syndre.sx:18029–19350,71385+,76222+`; `engine/bflibrary/src/x86-win-sdl2/sscreen.c` |
| Better graphics without replacing engine | SDL2 support and off-screen software-surface APIs exist; colour remap applies to 8-bit Bullfrog path, not automatically to 4-plane mission | **Constrained opportunities exist**, but no pre-approved new renderer/shaders or higher-resolution assets | `gspr_map.c:198–435`, `sscrsurf.c:44–175`, `sscreen.c:976–1085` |
| Reduce explosion slowdown without speeding up simulation or removing visuals | 256 fixed effect slots and fireball sub-pool; 53-case effect dispatch; collision/allocator loops; separate planar conversion and SDL scaling | **Instrumentable**, cause **unmeasured**; no optimization warranted before profile | `syndre.sx:19879–21317,27937–27975,33330–33337`; `display.c:149–231` |
| Cross-platform/controller work | Not requested; controller explicitly excluded; Windows is the accepted build platform | **Out of scope** | Do not introduce |

### Hard conclusions

- **A global darkened palette is not a satisfactory implementation of the requested night effects**. Original per-level palette IDs are shared and sprite/HUD colours can be affected. Scene-local lamps must account for actual world objects and occlusion.
- **Sprite remapping availability in the 8-bit Bullfrog library does not prove mission lighting support**. The mission's four planar source pixels expand to eight-bit indices and the generic remapper is not directly referenced by original assembly.
- **Menu rendering cannot be redesigned independently of its original mouse rectangles and campaign-state calls**. The eight `DrawMenuGadget` call sites and `map_selection → process_day` link are concrete guardrails.
- **Do not change source timing** to mask explosion slowdown: six-stage `move_it` includes a final tail jump to vehicle updates; overlapping effects involve collisions, allocation and drawing.
- **Do not use FreeSynd as the running game**. Its architecture is helpful to understand map/tiles but is not a verified drop-in replacement or code equality proof.

## 2. Confirmed source-level risks (not yet real-world crashes)

All of these code paths are **unchanged from v0.7** in the audit branch. No repair was made or approved.

| Priority for a future separate task | Area | What actual source confirms | Runtime evidence |
| --- | --- | --- | --- |
| High, conditional | GUI strings | `guitext.c` duplicates source entries into `gui_strings[800]`, but the threshold is **810**; a 267th distinct input entry writes array index 800 | Only synthetic index arithmetic; actual retail language file count unknown |
| High, conditional | SDL scaled output | `sscreen.c:963` writes a **4-byte `long`** even for **2-/3-byte destination pixels**, which the caller permits; custom scaling also ignores actual SDL row pitch | Byte-address arithmetic test; real 16-/24-bit output format not observed |
| High, conditional | Startup CLI | `main.c:87–103` appends arbitrary argv into a **100-byte fixed buffer** using unbounded `strcat` and passes to `system`, invoked before CLI parsing | Source call confirmed, runtime launched process not observed |
| Medium | Long CLI options | `main.c:155–215`: six long descriptors declare no parameter although corresponding switch branches dereference `optarg` | No executable CLI tests |
| Medium, conditional | SDL focus/surface failure | `LbScreenSwap` ignores failures from surface restore/recreate; scaling path logs lock failure but continues | No demonstrated crash or connection to v0.8 regression |
| Conditional | Game asset loading | Loader uses original RNC, palette, sprite, tile, sound and animation data; real asset bytes and failure modes untested | No original-game resource samples |
| Low/moderate, conditional | GUI memory allocation | GUI loader dereferences allocation before NULL check | No allocation-failure injection |
| Unknown until profile | Performance | Effect pool, repeated collision work, planar conversion and software blit are plausible measured stages | No benchmark or flamegraph |

**Source-bug vs user-visible bug:** Confirmation of an unsafe branch does **not** establish that the normal v0.7 gameplay triggers it. Avoid declaring regressions without Windows reproduction. Any repair requires separate explicit scope, versioned handover and tests.

## 3. Exact mission display/palette contract

For `DrwF_ScreenVres16`, the source-driven presentation pipeline is:

```text
Original mission world / actors / sprites (assembly, planar WScreen etc.)
        ↓ original dirty/scroll/entity draw and mouse operations
Four 32,000-byte bit planes (WScreen)
        ↓ update_vscreen_whole_vres16() [C]
640 × 400 8-bit indexed image at VScreen = VGABuffer + 0x6400
        ↓ rows 40–439 within the 640×480 VGABuffer
swap_wscreen(): copy first 307,200 output bytes
        ↓
SDL2 8-bit draw surface → software scaling/palette conversion
        ↓
window surface / SDL_UpdateWindowSurface
```

**Address/lifetime warning:** The Vres16 unpack begins at byte **25,600** (40 lines), but clears **307,200** bytes, extending **25,600 bytes into the later staging area** starting at offset 307,200, still within the one 614,416-byte allocation. Nothing indicates a heap out-of-bounds write from this alone; it does demonstrate alias/copy dependence. `mouse.c` independently subtracts 40 from Y to compensate. Never reposition the scene without correcting input and guarding both buffers.

**Proposed future exploration, not implemented:** A post-unpack `8-bit indexed` scene-only mapping is a research candidate, but on its own cannot know which pixels represent headlights/buildings, cannot restore proper occlusion and cannot guarantee HUD/cursor readability. A modern RGB shader renderer would be a substantially different architectural project, not an invisible step in v0.9.

## 4. Source-only verification achieved

- Non-truncated full Git tree/blob comparison: **PASS** (v0.7 `main` versus audit branch at documented snapshots). All existing gameplay code, SDL graphics/audio libraries, CI and tests are identical except preexisting title string. Documentation-only additions.
- Original full assembly retrieval: **PASS**.
- Original effects size/stride/dispatch calculations: **PASS** (256 records, first 128 fireball, 53 table cases).
- Menu hover rectangle source decode: **PASS** (logical x 500–628 / y 346–372, strict interior).
- Five-palette original table decode: **PASS** (IDs 1–5 repeated ten times + two zeros).
- Framebuffer geometry/alias math: **PASS** (40-line mission offset, 640×400 image, source buffer overlap under clear).
- Synthetic scaler byte-address checks: **PASS** (2/3/4 BPP write-width arithmetic), **no native SDL test**.
- Synthetic GUI string pointer-count checks: **PASS** (267th string index 800), **no actual game language file**.
- Synthetic four-plane bit-position parity: **PASS 228/228 examples**. The actual C converter was not compiled or run.
- CI workflow inventory: **PASS**. The existing v0.7 workflow builds Windows i686 but **does not invoke** seven declared Bullfrog library `make check` test programs.

### Not tested

No Windows executable build, native DLL/runtime check, performance counters, keyboard/mouse/scroll gameplay regression, SDL 16-/24-bit scaling, memory sanitizer, original game/palette/text resources, night-time screenshot, music test or automated test suite execution was performed in the forensic audit. The user's working v0.7 playback is historical user evidence, not a test run in this session.

## 5. Work remaining and staged go/no-go gates

**Asset-independent source work still possible:**
- Precisely trace remaining assembly control flow and unnamed switch handlers for map rendering; fully inspect sound/library initialisation and teardown; verify save-format sizes.
- Verify all UI click-action rectangles beyond the one decoded hover widget and all language variants.
- Design/perform independent test-only harnesses for scaling and GUI arrays if a safe, isolated environment is available **without modifying the protected baseline**.
- Review third-party dependency versions and licensing with current upstream material before sharing modified binaries.

**Requires later user-provided private original game resources and Windows playback:**
- Confirm the actual palette RGB values, sprite transparency, building tile/window/sign placement and path-of-light geometry.
- Capture original v0.7 menu, day mission and explosion frames; compare 640×400 logical scene and SDL 640×480 output.
- Verify moon/night effect source and minimum readability of HUD/agents/cursor without collision/gameplay changes.
- Profile frame stages with idle/normal/heavy explosions and compare identical scenarios before any proposed speed optimization.
- Test focus/Alt-Tab, four edges/corners, strategic progression and mission start/end, music and saves.

**Scope/approval gate:** Only after the above evidence should v0.9A UI or v0.9B real lighting code be proposed and independently approved. Do not silently fix unrelated source defects, refactor huge assembly blocks, change game pacing or import FreeSynd.

## 6. Source report navigation

- `docs/SOURCE_FORENSIC_AUDIT_2026-10-31.md` — primary chronological audit.
- `docs/ASM_ADDRESS_LABEL_INDEX_2026-11-01.md` — lexical original assembly navigation aid (not semantic call graph).
- `docs/SOURCE_AUDIT_RENDERING_AND_TEST_GATES_2026-11-01.md` — renderer/ABI/CI scope.
- `docs/SOURCE_AUDIT_FRAMEBUFFER_GEOMETRY_2026-11-01.md` — exact mission placement and converter index tests.
- `docs/SOURCE_AUDIT_RENDER_SCALER_GUI_BOUNDS_2026-11-01.md` — confirmed conditional scaler/GUI bounds risks.
- `docs/SOURCE_AUDIT_EFFECT_POOL_AND_PROFILING_2026-11-01.md` — effects pool, handler branches and profiling hooks.
- `docs/SOURCE_AUDIT_MENU_HITBOX_STRATEGY_2026-11-01.md` — hardcoded original menu bounds and campaign-state coupling.
- `docs/SOURCE_AUDIT_STARTUP_AND_CLI_2026-11-01.md` — original intro shell invocation and getopt inconsistencies.
- `docs/SOURCE_AUDIT_GAMEPLAY_AI_AUDIO_2026-11-01.md` — simulation/audio dependencies.
- `docs/SOURCE_AUDIT_ASSET_FREE_CONTINUATION_2026-11-01.md` — screen lifecycle, pointer ownership, focus.
- `docs/SOURCE_AUDIT_PALETTE_GROUPS_AND_BUILD_2026-11-01.md` — original palette sharing, fonts, upstream build identities.
- `docs/releases/v0.9.md`, `docs/releases/v0.9A.md` — approved scope/actual unbuilt implementation status.

**Bottom line:** The source can now be traced to concrete animation, map, sprite, palette, framebuffer, menu, timing, audio and effects hooks; a path to **targeted testable experimentation** is identifiable. **The full forensic audit and proof of localized lighting remain incomplete without real assets, a build, and Windows gameplay evidence.** Source analysis is not a substitute for those checks.

## 7. Map offset structure verified with original and reference source

**Additional focused report:** `docs/SOURCE_AUDIT_MAP_FORMAT_AND_GEOMETRY_2026-11-01.md`.

Original assembly `init_map_data` (`syndre.sx:25694–25716`) uses `map_buf+12`, then rebases **12,288 four-byte relative column offsets in place**, creating a 49,152-byte table. `draw_horizontal` (`71385+`) indexes columns with a **128-column stride** and parity-based draw dispatch. FreeSynd's external `src/map.cpp:47–70` independently reads three little-endian 32-bit dimensions followed by relative offsets also based at byte 12. **12,288/12,288 synthetic lookup comparisons** matched the shared base-plus-offset arithmetic. This is a cross-implementation source-format confirmation, **not actual retail map compatibility proof**. The original assembly assumes a fixed lookup count, while FreeSynd uses dimensions from the file; actual game bytes still need checking.

**Lighting impact:** Building face/window/sign identification requires more than the map filename: actual block IDs, `h_blocks` structure, projection and occlusion require inspection. A screen-space glow overlay could wrongly light occluded surfaces. No geometry/classification/light source system is yet verified.
