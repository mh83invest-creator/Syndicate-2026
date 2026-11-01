# Syndicate 2026 — Main Systems Audit: Animation Consumer Index and Element-Chain Contracts

**Date:** 2026-11-01 (Australia/Brisbane)  
**Scope:** main/core engine source audit on `main`; documents animation reads and validation contracts, without changing gameplay code.  
**Predecessor:** `docs/audits/2026-11-01-main-animation-bounds-and-profiling-feasibility.md` (commit `5d279b5e8cc8cfbd1adc9bd5ce8b0dc91bc2cf82`).  
**Evidence:** complete `engine/src/syndre.sx` assembly, 85,803 lines, GitHub blob SHA `c35e22621a93d1e07c44d40de8867446ff7c64bf`, read directly through the GitHub blob reader. Loader and build rules cross-referenced with the predecessor. These are *static source observations*, not tested corrupt-asset outcomes.

## MAIN-ANIC-01 — Animation start selectors span multiple entity/drawing systems

The `starts_ani` global pointer appears in more than effect simulation:
- `syndre.sx:17662–17671`: general entity-animation update derives selector from prior animation, entity state, a direction-derived increment and a lookup in `data_5a596`; then reads `starts_ani + 2 × selector`, without a local buffer length test.
- `17677–17695`: selected-frame draw code reads a 16-bit value from **fixed byte offset `0x4b0` (1,200)** into `starts_ani`, then uses the resulting frame index to read `frames + 8 × index + 6`. For *that one read*, the start table needs at least **1,202 bytes**.
- `18717–18906` (`number_people`): a four-member local loop (`18773–18775`, `18895–18900`) reads from `starts_ani + 0xf3e + 2 × member_index` (`18792–18794`, `18812–18815`). With member index 0..3 on the inspected loop, the largest lookup ends at byte **0xf3e + (3 × 2) + 2 = 3,910**. This is a code-derived addressability minimum *if all four iterations reach those particular branches*, not a measured file size or proof that all original team states use them.
- `21295–21306`: `move_effects` selects `starts_ani[animation_id]` (16-bit records), including previously established fireball selector `0x187` (391, needs 784 addressable bytes).
- `37813–37824`: object animation state can choose `0x18b` (395) and then index `starts_ani[selector]`, so that chosen entry requires **792** bytes.
- `38743–38956`: panel/overlay dispatch reads a series of **fixed 16-bit start-table offsets** `0x242`, `0x24a`, ... `0x2c2`; the largest listed direct fixed read at `0x2c2` ends at **708** bytes. These are per-branch selectors; some branches call `draw_element`.
- `40698–40713`: a panel element computes `0xd4 + field_10 + branch_0_to_3` before reading a 16-bit entry from `starts_ani` on a changed-index path; source does not locally verify the computed selector against actual buffer length.
- `57801–57863` and `59859–59870`: vehicle and weapon animation transitions produce selectors from entity state or stack-supplied animation values, read `starts_ani[selector]`, and write the resulting frame index; no local bounds check is present at these reads.

**Interpretation:** the earlier 784-byte fireball minimum is insufficient as a **universal start-table validation rule**. Even the newly observed 3,910-byte conditional team lookup is **not a proven upper bound**, because multiple consumers produce dynamic 16-bit selectors. A future validation contract must map *all reachable selectors and file extents*. Do not assert the original `hsta-0.ani` is too short; its contents have not been inspected.

## MAIN-ANIC-02 — Frame-table consumers do not use the declared `frames_end` guard locally

`animate_model:33353–33372` takes a current 16-bit frame index, retrieves the next index from the 8-byte frame record at offset +6, then reads the next record's completion bit from byte +5. `animate_panel:37839–37858` performs an analogous two-step chain starting at a panel field +0xc. `draw_pelement:19035–19063`, `draw_element:19109–19127`, `draw_agent_element:19173–19191`, and `show_selected_frame:17685–17695` access `frames` by a derived index. The inspected routines do **not** compare against the `frames_end` symbol.

A safe read-only validator needs loaded allocation extent, 8-byte alignment/count, current index, **next index**, and the fields used by the specific consumer; validating only the first record is not enough. No replacement loader, validator or fix is implemented.

## MAIN-ANIC-03 — Element records are 10 bytes; drawing loops check the *base*, not the end

Several rendering procedures compute an element address from a frame's 16-bit element index as `elements_ani + index × 10`: `syndre.sx:17722–17735` (selected frame), `19058–19074` (`draw_pelement`), `19119–19138` (`draw_element`), and `19182–19195` (`draw_agent_element`). Within the same draw routines, the next-link index is read from record offset +8, recomputed as `elements_ani + 10 × next_index`, and used for subsequent records (`17754–17765`, `19089–19100`, `19153–19164`, `19264–19275`).

The **observed** pointer checks compare the calculated element pointer with the **start** pointer (`elements_ani`), then branch when it is below/equal or above. They do **not** compare against the separately declared `elements_ani_end` (`syndre.sx:82655–82658,83881–83884`) to establish that a full 10-byte record fits. A positive but too-large or corrupt index could therefore escape the local base-only check; runtime fault reachability and the legal asset format are unverified. Some sprite-pointer checks similarly compare with their base but not necessarily with the loaded sprite-table end in these exact snippets. Do not extrapolate that **all** draw functions lack every form of validation without a complete separate review.

Any future element-chain validator must check the full 10-byte record, each 16-bit next-link index and termination, sprite reference validity, reasonable cycle limits for malformed synthetic input, and absence of overflow in `10 × index` arithmetic. These are proposed diagnostic requirements, not verified bugs in retail resources or an instruction to change engine rendering.

## MAIN-ANIC-04 — Frame sound events share the earlier sound-bank failure chain

At `syndre.sx:18236–18254` the draw-list path reads frame byte +4 from the 8-byte `frames` record to obtain a nonzero sound identifier, then calls native `ac_SetBFSampleStatus`. Another animation drawing path, `draw_pelement:19045–19056`, does the same. These accesses combine three previously independent validation obligations: valid frame pointer and index, valid game sample table and index, and sound-bank readiness. Earlier core audio audits verified the native C `SetBFSampleStatus` is not guarded against a NULL `smptable` locally, unlike the historical assembly version.

**Consequence:** a partial mission-animation load and partial sound-bank load could interact at drawing time, but no specific execution with missing files, invalid index or crash has been reproduced. Document the cross-resource interface; avoid treating audio and graphics as disjoint failure states when designing a test harness.

## MAIN-ANIC-05 — Test design and workstream limits

**Synthetic, disposable test cases (NOT executed):**
1. start table: exactly 784, 1,202, 3,910 bytes; one record shorter than an observed selector; missing buffer; maximal 16-bit computed selectors; test each source path independently and confirm actual loaded-file requirements first;
2. frames: missing/empty; 7-byte partial first record; valid current but invalid next index; nonterminating next-frame chains; recorded completion flags;
3. elements: size not divisible by 10; first/next index past end, invalid next index zero/termination assumptions, cycles, large 16-bit indexes, invalid sprite references;
4. integrated cases: valid animation + missing TAB; missing frames + valid audio; invalid frame sound ID; ordinary single/overlapping explosions and post-explosion decay;
5. on a separately authorised *disposable 32-bit Windows build* confirm native compiled wrappers, map actual loader result and abort behaviour, record safe failures versus effects/sound/draw path with high-resolution, low-overhead timing and an uninstrumented control.

Tests must not touch retail installations, user saves or protected v0.7. Do not commit proprietary `.ani`, sprite or sound files. Preserve Original/American Revolt data, original effects, collision, RNG call order, tactical 16 FPS, mouse/keyboard and **between-mission-only campaign-isolated** saves. Graphics Audit owns animation/image meaning and Mission Builder owns custom mission resources; this main report defines **shared engine read boundaries**, not their implementation.

## Verification and remaining limits

**Actually performed:** comprehensive **lexical source sweep in the complete original assembly blob** of `starts_ani`, `frames`, `frames_end`, `elements_ani`, `elements_ani_end`, and direct inspection of the cited consumer branches; comparison of fixed offsets and record widths. Existing GitHub C/load/wrapper handovers were cross-referenced.

**Not performed:** complete cross-language/dynamic dataflow, exhaustive reachability of every variable selector, real `.ani` decoded sizes or sprite assets, compiler/Windows runtime, profiling, fault injection, gameplay, CI, engine edits, branch merges or modifications to other streams. The observed fixed-offset minimum is **not** a certification of universal input length or safety.

**Next MAIN checkpoint:** derive bounded ownership and extent-propagation rules from `TbLoadFiles` fields into `starts_ani`/`frames`/`elements_ani`; confirm synthetic fixture approach against resource loader without modifying it; complete source-level timing/ABI verification before proposing an instrumented diagnostic branch. Shared handover must note the Graphics and Mission Builder compatibility boundaries.
