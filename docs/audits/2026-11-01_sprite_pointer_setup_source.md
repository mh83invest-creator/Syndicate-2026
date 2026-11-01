# Graphics audit — 8-bit sprite pointer setup and encoder

Date: 2026-11-01. Branch: audit/graphics-assets-2026-11-01. Source-only review.

Direct reads of engine/bflibrary/src/general/gsprite.c, gspr_std.c and gspr_enc.c confirm that the Bullfrog 8-bit sprite abstraction distinguishes a sprite table from its separate encoded pixel data. LbSpriteSetup iterates a start/end range and converts sprite Data values into pointers relative to a data buffer when the stored value is less than that base address; LbSpriteReset applies the inverse comparison and subtraction. LbSpriteDrawPrepare checks for nonpositive sprite dimensions, and LbSpriteEncode iterates SHeight lines using SWidth and the sprite's data pointer.

This establishes an existing library-level offset-to-pointer setup pattern; it DOES NOT establish that the original hspr-0.tab uses the same TbSprite records or that its 10,812-byte decoded length has any specific record stride. Original tactical assembly consumers and table layout remain unverified. Do not infer sprite count or use this pointer conversion on private data without validating file structure and assembly callers.

Checks: source files fetched and corresponding functions inspected. Not run: original assembly extraction, private asset parsing, compilation, Windows gameplay, screenshot comparison, benchmarks. Changes: this report and index only; no engine changes. Next: original assembly sprite entry consumer and record stride, followed by private data cross-check.
