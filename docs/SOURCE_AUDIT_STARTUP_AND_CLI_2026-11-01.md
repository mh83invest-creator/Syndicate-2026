# Syndicate 2026 — Startup, Command-Line and Legacy Input Safety Audit

**Date:** 2026-11-01 (Brisbane)
**Scope:** Asset-independent static review of current `engine/src/*.c` source and exact startup call sites. **No source change**, build, exploit test or runtime verification.
**Baseline:** v0.7 `main` commit `a22b0b9580a9710b4e693e88854351835ef1ee40`; development branch `feature/v0.9-visual-showcase`. A prior full tree diff confirmed all original engine code is identical except the native window-title string in `engine/src/game.c:85`; therefore these startup issues **pre-exist v0.9**, and should not be attributed to the forensic audit.

## Finding 1 — intro is still launched at source level; startup command construction is unbounded

**Source:** `engine/src/main.c:87–103` and `main.c:263–269`.

The helper `run_intro(int argc, char **argv)` declares `char intro_cmdline[100]`, initializes it with `strcpy(intro_cmdline, "intro >nul:")`, appends a space and **each complete argument** using repeated `strcat`, then passes the result to `system(intro_cmdline)`. `main` calls `run_intro(argc, argv)` **unconditionally before processing arguments**.

**Source-backed defect:** No length check precedes repeated concatenation to the fixed 100-byte array. Sufficiently long argument text can write outside the destination buffer. The command is also assembled from command-line input and passed to a shell without argument quoting/escaping. Those are unsafe API patterns. Actual exploitability, binary build configuration and runtime impact were **not tested**.

**Important continuity correction:** The user previously confirmed that the opening intro no longer appears in their known-good playtest. That observed behaviour is **not equivalent to the source containing no startup intro invocation**. It may be failing or doing nothing on the packaged environment; the audit cannot establish which without seeing the Windows installation and process execution. **Do not re-enable the intro or alter startup behaviour incidentally.**

**Proposed later controlled verification (not authorized source change):** observe the process launch in a controlled Windows test environment, test invocation with ordinary and oversized *non-malicious* arguments, and verify normal startup, help behaviour, and whether an intro executable exists in the package. Preserve gameplay, versions, and release handover.

## Finding 2 — six long options declare no argument despite using optarg

**Source:** `engine/src/main.c:155–181`, `main.c:183–215`.

The `getopt_long` options table declares `has_arg = 0` for these names:

- `--iirq` / short `-R`;
- `--idma` / `-D`;
- `--iio` / `-I`;
- `--nport` / `-p`;
- `--nname` / `-n`;
- `--colang` / `-c`.

But the matching switch branches use `atoi(optarg)` for numeric values and `strncpy(..., optarg, ...)` for the network name. The **short options string** correctly includes colons, `R:D:I:p:n:c:LHdswSh`, marking the six short forms as taking arguments. The **long-option descriptors do not**. On a conventional GNU `getopt_long` implementation, using the long forms without an argument yields `optarg == NULL`, which can cause invalid pointer use. This was **not compiled or exercised** here.

**Scope:** This is a CLI parsing consistency issue, not a reason to add new controls, redefine gameplay flags, or change language defaults without approval. Other flags `-L`, `-H`, `-d`, `-s`, `-w`, `-S`, `-h` are intentionally no-argument and are not flagged by this check.

## Finding 3 — other unbounded string operations deserve targeted contract review

Scanned all **17 game-engine `engine/src/*.c` files** for common unbounded C string/format/shell calls (`strcpy`, `strcat`, `sprintf`, `vsprintf`, `system`, etc.). Direct occurrences were located in:

- `main.c`: `run_intro` command buffer, plus language-string copy at line 245;
- `game_data.c:127,137,151,177`: save-slot file-name formatting and empty-slot description string copy;
- `sound.c:353–354`: construction of `GTL_filename` from `"data/sample."` and `desc->data_suffix` (legacy audio driver metadata).

**Not every occurrence is a defect.** For example, `game_data.c` uses fixed save-slot paths and may be sufficiently sized for the intended numeric slot range. Whether `sound.c` and GUI empty-slot text can overflow depends on actual destination buffer sizes and source contracts; those paths were **flagged for review, not declared exploitable**. A source-level concern is that `load_save_text` copies a translated string before writing a terminator at `GAME_DESC_TEXT_MAX_LEN-1` (`game_data.c:134–138`); a post-copy terminator cannot retroactively prevent an overrun if the source is longer than the destination. Need original asset/text bounds.

**Audit boundary:** This is a targeted game-engine C search, not a whole-codebase security audit across the Bullfrog libraries and assembly. No assertion about all potential vulnerabilities or safe behaviour is made.

## Suggested priority and unchanged scope

| Finding | Source certainty | Runtime certainty | Action gate |
| --- | --- | --- | --- |
| Unbounded fixed-buffer intro command | High — unconditional concatenation confirmed | Unverified | Maintain as known risk; plan non-gameplay security repair only with explicit approval and regression testing |
| Shell command built from CLI | High — `system` and concatenation confirmed | Unverified | Determine whether external intro is actually needed; no change without scope approval |
| Invalid long-option argument descriptors | High — descriptors and `optarg` uses conflict | Unverified | Reproduce on real executable, then propose surgical parsing fix |
| Additional C string calls | Presence confirmed | Some may be safe | Review each buffer and input provenance; avoid blanket remediation |

**Tests performed:** read source; enumerated and statically matched CLI declarations to `optarg` consumers; checked startup invocation. **No compilation, CLI invocation, dynamic security tests or Windows gameplay.**

**Handover:** Keep `main` and v0.7 gameplay as-is. Document any actual repair as a separately approved, versioned source/CI/test change. FreeSynd stays external reference. No engine upgrades, menu rewrites or controller functionality are authorized here.
