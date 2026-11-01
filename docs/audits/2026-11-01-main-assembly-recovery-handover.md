# MAIN audit handover — restored assembly source access
Date: 2026-11-01. Workstream MAIN, main branch. No engine or parallel workstream changes.

## Verified milestone
GitHub `fetch_blob` returned the full readable `engine/src/syndre.sx` blob `c35e22621a93d1e07c44d40de8867446ff7c64bf`, despite empty contents-API ranged reads. Source verification: tactical loop 220–278, `move_it` 33330–33337 (tail `jmp move_vehicles`), `move_effects` entry and indirect dispatch 19979–20007, `sprite_colide` and `move_mapwho` sequence 20757–20783. Audit report `docs/audits/2026-11-01-main-assembly-blob-recovery-and-direct-control-flow.md`, commit `b69056627460b875e6497f1131f18691603d4734`.

## Next checkpoint
Read the remaining `move_effects` end/branch labels and collision function return boundaries using blob access to determine ABI-safe observational probes. Profile via disposable diagnostic Windows build only when approved; protect v0.7, original 16 FPS tactical pacing, explosions, audio and campaign save semantics.

## Scope and limitation
Documentation only. No generated disassembly, Windows compilation, gameplay test or runtime benchmark. The original expanding cross-workstream handover file update was blocked; this separate companion document preserves the status without altering existing handover or parallel branches.
