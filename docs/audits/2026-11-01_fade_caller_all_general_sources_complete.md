# Syndicate 2026 — Completed direct fade caller scan in application and general library sources

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: graphics audit only; no engine changes.

## Verified source inventory and matches
- GitHub main-branch recursive tree identified **59** C/C++ source files under `engine/bflibrary/src/general/`. File contents were read individually from the dedicated graphics audit branch and searched with `/LbPaletteFade\s*\(/`.
- First pass inspected 12 selected `general` sources; second pass inspected all **47 remaining** sources in three groups of 15, 15 and 17. All requests in these groups completed successfully.
- The only match in those **59 general source files** is the `LbPaletteFade` **function definition** at `engine/bflibrary/src/general/gpalette.c:38`; **zero direct call sites** were found.
- Prior report `2026-11-01_fade_caller_engine_src_scan.md` similarly inspected all **17** `engine/src/*.c` files with no direct calls.
- Combined audited scope: 76 C/C++ source files from these two directories. Their code shows no direct invocation of `LbPaletteFade`; this does not establish that the function is never used elsewhere.

## Risk classification
`gpalette.c:63,100` divide by `fade_to` without a local zero check, and palette/fade API failures are ignored, as noted previously. However **no actual application caller or zero-valued argument has been demonstrated** in the audited source directories. Do NOT call this a confirmed Syndicate gameplay division-by-zero or request a gameplay fix on that basis.

## Scope boundary
Not scanned: platform-specific `bflibrary/src/x86-*` files, other application modules or repositories, generated/assembly files, function-pointer dispatch, macros and binary references. Main-branch tree supplied *paths* only; all stated source contents were read on graphics branch.

## Verification ledger
PASS: 47 remaining general source fetches, plus prior 12 general and 17 application source fetches; matching function definition only.
NOT TESTED: runtime fade usage, Windows executable/backend, 0 divisor injection, graphics screenshots.
GitHub changed: this report and `docs/GRAPHICS_AUDIT_CONTINUITY.md` only. Protected v0.7, Mission Builder and other branches untouched.
NEXT: assess platform-specific calling sites if useful and prioritise higher-evidence graphics defects (scaler width/pitch and real SDL surface formats) rather than hypothetical fade crash.
