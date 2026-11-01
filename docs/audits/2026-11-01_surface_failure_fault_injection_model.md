# Syndicate 2026 — Four-case surface failure-state model

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: standalone model based on SDL2 source state transitions; graphics documentation only.

## Basis
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c:290-325`: create surface, set flag, optional palette failure frees/nulls surface.
- `sscreen.c:400-411,520-539,617-639`: mode setup resets initialized flag before create; setup's attempt to call LbScreenReset on creation failure short-circuits while uninitialized.
- `sscreen.c:1014-1047`: app WScreen rebind snapshots old draw pointer and restores it on creation failure.

## Executed isolated fault-state model
Created a short C program at `/tmp/synd_surface_fault.c` in temporary local container storage. Implemented the exact relevant boolean, pointer and return-value transitions in a *stubbed state model* of `create`, `setup`, `reset` and `rebind`; no actual SDL calls or game source were compiled. Synthetic static old/new surface objects. Compiled using `cc -Wall -Wextra -O0 -g -fsanitize=address,undefined` and executed locally. Exit 0; no sanitizer diagnostics.

| Injected path | result | draw | has_secondary | initialized | freed old | freed new |
|---|---:|---|---:|---:|---:|---:|
| setup_alloc | 0 | NULL | 0 | 0 | 0 | 0 |
| setup_palette | 0 | NULL | 1 | 0 | 0 | 1 |
| rebind_alloc | 0 | old | 1 | 1 | 0 | 0 |
| rebind_palette | 0 | old | 1 | 1 | 0 | 1 |

## Finding
The isolated state model confirms the specific source-derived flag/pointer inconsistency after *initial setup palette failure*. Rebinding failures from an initially valid secondary surface restore the old pointer and preserve a true flag, so the previous audit's broad concern about rebind flag inconsistency **does not materialise for those modelled starting conditions**. The model does not validate SDL palette error reachability, actual SDL resource lifetime, native backend or game execution.

## Next verification
Use real SDL calls with controlled injected allocation/palette failures in an isolated test build, inspect library reset and cleanup after failure, and verify active Windows backend and compilation configuration. No gameplay or baseline changes before that testing and approval.

## Verification ledger
PASS: standalone stubbed C model compiled and ran; all four state outputs captured; ASan/UBSan no alerts.
NOT TESTED: original SDL code compilation, actual SDL failures, Windows build, runtime gameplay, graphics screens or proprietary assets.
GitHub changes: report and graphics continuity only.
