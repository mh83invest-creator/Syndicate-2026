# Graphics audit — animation chain bounds and cycle exposure

Date: 2026-11-01 (Australia/Brisbane)
Branch: audit/graphics-assets-2026-11-01
Status: narrow original assembly trace complete. Not a reproduced crash or asset defect.

## Source inspection
Original engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.
- Lines 19058-19067: a 16-bit frame field at +0 selects an animation element by ten-byte indexing. The only nearby guard compares the computed address with the elements_ani *base* and skips when <= base.
- Lines 19090-19100: element +8 supplies a subsequent 16-bit index, multiplied by ten and rebased. Execution repeats when the computed address is greater than elements_ani base.
- Lines 19117-19131 and 19154-19164: draw_element uses the same frame-first-element indexing and next-element chaining. Again the observed branch tests against the *base* only; no element-end comparison or cycle counter appears in the shown loop.
- Lines 17683-17695: show_selected_frame initialises from starts_ani at fixed offset 0x4b0 when current state is zero, then reads frame+6 as subsequent state.
- Lines 18800-18815: a separate path either uses frame+6 or resets from starts_ani at 0xf3e plus an indexed word offset; branch choice depends on other gameplay state.

## Deductions and limitations
An element next-index value zero computes exactly the elements_ani base and therefore terminates these inspected element loops. This is a demonstrated branch result. An index that points to any positive offset can continue, including self-references or cycles; the inspected loops have no upper-bound check or cycle limit. This is a conditional risk, not proof that the original data contain cycles or out-of-range references.
Frame +6 supplies a successor *value* in examined routines but does not establish a universal zero/end-of-animation convention. Different paths reset the frame based on conditions.
Existing reported decoded element length of 104,860 bytes implies a 10,486-record candidate capacity at ten-byte stride, not a proven active count. Whether source-generated addresses exceed this capacity can only be determined from private decompressed data.

## Required private validation
Decode the actual hfra-0.ani and hele-0.ani resources with expected unpacked lengths and CRC checks. For each reachable frame, validate the initial element index. Traverse every linked element with visited-node tracking, explicit upper bounds and hop caps; report zero termination, cycles, out-of-range links and unreachable records separately. Do not modify engine loops based on hypothetical malformed data. Verify behaviour against Windows rendering and time progression before any repair proposal.

## Verification
PASS: direct read and manual branch inspection of original full assembly source.
NOT RUN: private data decode, automated exhaustive animation-graph tests, compile, Windows run or graphics rendering.
Scope: documentation-only report. No gameplay, engine, build, private art, protected main, Mission Builder or core audit modifications.
