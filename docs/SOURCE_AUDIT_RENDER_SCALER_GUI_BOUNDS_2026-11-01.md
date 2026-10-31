# Syndicate 2026 — Renderer Scaler and GUI Text Boundary Audit

**Date:** 2026-11-01 (Brisbane)
**Scope:** Static source review and *synthetic arithmetic checks* only. No engine code, assets, workflow or tests modified. No native Windows execution.
**Repository:** `mh83invest-creator/Syndicate-2026`, `feature/v0.9-visual-showcase`. Protected `main` v0.7 baseline `a22b0b9580a9710b4e693e88854351835ef1ee40`.
**Sources read:** `engine/bflibrary/src/x86-win-sdl2/sscreen.c`; `engine/src/display.c`, `guitext.c`, `guitext.h`, `main.c`. Full-tree Git blob comparison in prior audit established these files are unchanged from v0.7, so findings are inherited risks, not v0.9 regressions.

## Finding R1 — Fallback SDL 8-bit-to-true-colour scaler writes four bytes regardless of destination pixel size

**Source:** `engine/bflibrary/src/x86-win-sdl2/sscreen.c:924–971` (`LbI_SDL_BlitScaled_totcbpp`).

This function accepts `dst_bpp` (bytes per pixel) and advances `dst_offset += dst_bpp` each pixel. **However**, the actual pixel store at **line 963** is:

```c
*((long *)(dst_buf+dst_offset)) = (c.r << rshift) + (c.g << gshift) + (c.b << bshift);
```

On the mandated **i686 Windows** build, C `long` is **four bytes**. Accordingly, the function performs a 4-byte write even when destination BPP is 2 (16-bit) or 3 (24-bit). For the final destination pixel, that exceeds a tightly packed pixel array by 2 or 1 bytes respectively; it also overlaps the next pixel during earlier iterations. Casting a byte pointer to `long*` may additionally create alignment issues.

The caller at **lines 976–1006** uses this custom routine for any scaled *1-byte source* with destination BPP != 1; it does not explicitly restrict that destination to 4-byte pixels. The mode mapping in `sscreen.c:344–369` includes 16-, 24- and 32-bit formats. Thus this is a **source-confirmed incorrect write width on a supported branch**, but we have **not** demonstrated that the affected 16-/24-bit destination is actually selected in the user's current Windows play mode.

**Synthetic arithmetic test:** with a three-pixel-wide tightly packed output row:
- BPP=2 gives allocated row bytes 0–5, last write at offsets 4–7 (**two bytes past row allocation**).
- BPP=3 gives allocated row bytes 0–8, last write at offsets 6–9 (**one byte past row allocation**).
- BPP=4 gives allocated row bytes 0–11, last write at offsets 8–11 (**fits**).

This is an *address calculation test* of the C code, **not a reproduced native heap overflow or observed crash**. Real SDL surfaces may have row padding, so the first cross-row write need not go outside the whole allocation until the last row; actual corruption depends on format and surface dimensions.

### Also: scaler ignores actual SDL surface pitch

`LbI_SDL_BlitScaled_to8bpp` at **878–921** and `LbI_SDL_BlitScaled_totcbpp` at **924–970** take width, height and raw pixel pointers but **not** `SDL_Surface.pitch`. Pixel indexing assumes packed rows: destination uses `dst_offset += dst_bpp`, source `src_offset += src_w`. This is not generally correct for padded 8-/24-bit surface rows. Current 320-/640-wide, 8- or 32-bit mode combinations may meet the assumption; actual window formats and pitches have not been observed.

**Source-based conclusion:** The SDL backend is a poor place to add lighting overlays without first auditing actual surface format, BPP, pitch, lock success, and mode transitions. This finding also reinforces the earlier documented risk that SDL lock/surface-recreation failures are logged but not properly propagated.

## Finding R2 — GUI text pointer-index construction can write outside its allocated array

**Source:** `engine/src/guitext.h:28` declares `STRINGS_MAX 800`; `guitext.h:40` declares `char *gui_strings[STRINGS_MAX]`. `engine/src/guitext.c:63–103` defines `create_strings_list`.

For each input string, the code stores one pointer, then if `text_arr < &strings[270*3]`, it stores **two additional pointers**, presumably for multilingual indexing. The comparison target is **`&strings[810]`**, beyond the valid 800-entry array. The loop's `text_idx` counter counts **input strings**, not pointer writes; it cannot detect a three-pointer write crossing the 800-pointer destination.

**Synthetic index-count test, following the exact increment and condition rules:**
- After **266 input strings**: 798 written pointers, indices 0–797 (within bounds).
- For the **267th input string**: code writes entries **798, 799, 800**; index **800 is outside the array**. Later input strings can continue writing past the end.
- `while (text_idx >= 0)` provides no bound on `text_arr`, even after duplication stops.

This is a **source-confirmed conditional out-of-bounds write**, assuming an input `guitext.dat` contains **267 or more distinct strings**. The audit does **not** have the user's commercial/localised GUI data, so **the actual number of strings and runtime triggering is unknown**. The count test is not a compiled C memory-sanitizer run.

**Additional GUI-loader error-path concern:** `guitext.c:39–42` dereferences `(*strings_data)[text_len - 1]` immediately after `LbMemoryAlloc(text_len)` without first testing whether allocation returned NULL. `guitext.c:53–60` has a failure log branch, but it cannot protect the earlier dereference. Actual allocation-failure behaviour was not reproduced.

**Why this matters to v0.9A:** Replacing or expanding GUI text data, translations or labels could cross the 266-string safe-write boundary and corrupt memory. Never expand the text files as a “simple visual enhancement” without identifying and protecting this array contract. The original menu code also uses language-dependent text metrics and hitboxes.

## Finding R3 — source inspection alone does not prove the existing build exhibits these defects

The known-good v0.7 branch uses the same SDL2 backend and GUI loader. Existing user playback has not demonstrated a failure in either identified path. A 32-bit destination may bypass the four-byte mismatch, and existing original GUI string data may stay below the triggering entry count.

**Classification:**
- Incorrect scalar-store width for 16-/24-bit destination: **definite source bug in conditional format path**, **runtime reachability unknown**.
- Ignoring `SDL_Surface.pitch` in custom scaling: **definite non-general implementation assumption**, actual supported format consequences unknown.
- GUI string pointer overflow if 267+ input entries: **definite conditional source bug**, actual source data length unknown.
- GUI null-allocation dereference: **definite unsafe null handling**, runtime likelihood untested.

None of these findings authorises a source fix or establishes a direct connection to previous scrolling/focus regressions.

## Validation gates before authorised repairs

1. Keep v0.7 unchanged and record native desktop SDL window/draw surface `BitsPerPixel`, `BytesPerPixel`, width, height and `pitch` in a *separately authorised diagnostic build*.
2. Use independent mock 8-bit source surfaces and 16-/24-/32-bit destination surfaces; run an instrumented/native sanitizer build for format conversion, checking row padding and sentinel pixels. Confirm source-to-destination pixel equivalence.
3. Inventory the actual counts and byte lengths of `language/*/guitext.dat` files privately; do not commit retail text assets to public GitHub.
4. Add a bounded-array synthetic GUI string fixture with 266, 267 and 270 entries in a test-only branch; prove failure before any surgical repair and record the post-fix result.
5. Verify all supported UI languages, original text/hover coordinates, save-game format, 16 FPS mission pacing and 30 FPS strategic pacing after any approved modifications.

**Tests actually executed in this audit:** Source read and synthetic *index arithmetic* for pointer counts and write offsets. **Not executed:** Windows native build, SDL C unit test, memory sanitizer, original-game asset read, screenshots, focus/scroll or gameplay regressions.

**Handover:** The above findings are audit entries only. No changes to graphics, audio, menus, scroll logic, test pipelines or game code; main/v0.7 protected; FreeSynd remains an independent research reference.
