# Syndicate 2026 — animation frame and element source audit
Date: 2026-11-01 (Australia/Brisbane)
Branch: audit/graphics-assets-2026-11-01
Status: focused original assembly source audit COMPLETE; private animation data and Windows visual equivalence UNTESTED.

## Source evidence
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.
- Lines 82650–82662 distinguish hfra-0.ani (frames/frames_end), hele-0.ani (elements_ani/elements_ani_end), and hsta-0.ani (starts_ani). These are different resource buffers.
- Lines 19045–19049 multiply the selected frame index by eight to address frames, then read byte +4. Lines 19052–19056 pass that byte to ac_SetBFSampleStatus when nonzero. This is a frame-associated sound-control field in this particular call path, not evidence of the entire frame layout.
- Lines 19058–19065 read a word at frame +0, multiply it by ten, and add the elements_ani base. This establishes a source-level eight-byte frame stride and ten-byte element stride in the inspected path.
- Lines 19069–19087: at element +0, a word is used as a byte offset relative to h_sprites; +2 and +4 are word position offsets; +6 is passed to the draw routine; +8 supplies the next element index.
- Lines 19090–19099 transform the element +8 word into another ten-byte element address and continue based on a pointer comparison. A corresponding high-resolution element loop appears at 17722–17764, drawing to WScreen rather than UScreen.
- Lines 27723–27730 confirm that h_sprites is initialised by ac_LbSpriteSetup using h_sprites_data and h_sprites_end. This does not validate referenced element offsets against the table end.

## Resource size arithmetic from prior verified audit
- hfra-0.ani: 71,584 decoded bytes / eight = 8,948 candidate frame records.
- hele-0.ani: 104,860 decoded bytes / ten = 10,486 candidate element records.
- hsta-0.ani: 3,940 decoded bytes; stride and logical format not established by this source trace.
- hspr-0.tab: 10,812 decoded bytes / six = 1,802 candidate sprite descriptor records.
These are divisible candidate capacities, NOT confirmed numbers of active frames/elements/sprites.

## Important correctness and safety limits
- Observed pointer comparisons in drawing code are not a proof of complete bounds checking. Read-only validation of frame indices, next-element links, sprite byte offsets and loops is required before an editor, renderer or importer uses the format.
- The source shows frame byte +4 involvement with a sample status API; meaning of other frame fields is unknown.
- The +8 element link cannot be assumed to be a finite, valid list without examining decoded data and cycle termination.
- No proprietary original animation bytes or images committed.

## Verification
PASS: full original assembly GitHub blob fetched; cited code regions inspected; length/stride arithmetic.
NOT RUN: original private animation archive decoding, frame/link checks, original native build, Windows visual tests, effect performance benchmark.
CHANGES: this report plus graphics branch continuity index, no engine or other workstream changes.
NEXT: decode privately and validate record ranges/link chains/sprite offset alignment; trace starts_ani consumer and frame field semantics separately.
