# Animation advancement and termination — source trace
Date: 2026-11-01 (Australia/Brisbane)
Branch: audit/graphics-assets-2026-11-01
Scope: original assembly source investigation only.

## Confirmed
- `syndre.sx:17677-17695` (`show_selected_frame`) initialises a state word from `starts_ani+0x4b0` when zero, indexes `frames` at eight bytes per frame, then writes frame word +6 back to that state. This is explicit frame-to-frame progression in the inspected path.
- `syndre.sx:18789-18815` uses `starts_ani+0xf3e+2*i` for certain initial/reset states, and advances from `frames[current]+6` in a conditional path. Advance/reset selection is state-dependent; do not assume every animation uses the same loop.
- `syndre.sx:19090-19100` and `19154-19164` read element word +8 as a next-element index, multiply by ten, and loop only while the resulting address compares above `elements_ani` base. This source-level base-pointer termination condition does NOT establish safe upper bound or cycle protection.
- `syndre.sx:59859-59870` shows an object animation reset on a changed, nonzero animation identifier, obtaining a word from `starts_ani[id*2]` and storing it at object +0x10 alongside identifier +0x12.
- These observations support different responsibilities for starts, frames, and elements; no byte-level retail-asset validation has been run.

## Open validation gates
1. Validate all decoded `starts_ani` values against frame capacity and known sentinel conventions.
2. Inspect `frames+6` transition values, distinguishing terminal markers, loops and out-of-range references.
3. Follow element +8 chains, check base and end bounds, cycles and valid sprite byte offsets; do not interpret base-only pointer comparison as comprehensive validation.
4. Confirm visual timing, effects, and sequence termination in a real Windows run.

## Verification
Pass: full original assembly blob read and exact source regions inspected.
Not run: private graphics-byte parse, C/assembly compilation, Windows gameplay, visual frames, performance profiling.
Changed: documentation only on graphics audit branch. No game code, assets or other branch touched.
