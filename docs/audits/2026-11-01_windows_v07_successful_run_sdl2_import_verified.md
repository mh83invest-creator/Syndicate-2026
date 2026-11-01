# Syndicate 2026 — Successful Windows v0.7 run confirms SDL2 DLL import

Date: 2026-11-01 (Australia/Brisbane)
Branch: `audit/graphics-assets-2026-11-01`
Scope: read-only GitHub Actions/build audit, graphics continuity. No game code changed.

## Specific run and verified job
- Direct GitHub Actions repository runs endpoint `https://api.github.com/repos/mh83invest-creator/Syndicate-2026/actions/runs` was accepted by the connected GitHub fetch action (unlike previously attempted narrower workflow and artifact listing URLs).
- Run **38483184953**, named `Syndicate 2026 v0.7 source build`, branch `main`, SHA `a22b0b9580a9710b4e693e88854351835ef1ee40`, created `2026-10-13T23:16:07Z`, completed **success**.
- Job **115590427796** (`Compile committed v0.7 source`) completed successfully, including dependency verification, configure, compile, import rejection check, executable packaging check.
- Read actual job logs using `fetch_workflow_job_logs`. `objdump -p release/src/syndicatfx.exe | grep 'DLL Name:'` output explicitly included **`DLL Name: SDL2.dll`** (log line approximately 2485), along with `OpenAL32.dll`, `libWildMidi.dll` etc. This is direct build-log evidence of a **real compiled Windows v0.7 executable importing SDL2.dll**, not only configure prediction.
- Log also shows the successful upload of artifact `syndicate-2026-v0.7-verified-source` at artifact ID **11858508104**, size **15941747** bytes (log lines approximately 2645-2646).
- `fetch_workflow_run_artifacts` currently returned two artifacts: `syndicate-2026-v0.7-handover` and `syndicate-2026-i686-dependencies`; executable package was not in that current response. Do not claim its bytes are currently obtainable or have been inspected. The historical job log proves successful upload at run time.

## Supersedes prior limitation
The prior `2026-11-01_windows_v07_artifact_verification_boundary.md` correctly recorded a connector access failure for narrower Actions listing endpoints at that time. This turn resolved the blocker by using the **repository actions/runs endpoint**, retrieving an actual run ID and job log. The earlier uncertainty about SDL backend is now resolved for run 38483184953 as SDL2.

## Graphics implication
The compiled v0.7 Windows source build imports SDL2. Thus the inspected SDL2 graphics code is relevant to that build. However `SDL_GetWindowSurface` format, resolution, BytesPerPixel and `pitch` and actual `LbI_SDL_BlitScaled` dispatch are **not recorded in these logs**; no scaler buffer overflow or visible corruption is claimed for real gameplay.

## Verification ledger
PASS: real run metadata, job status, logged executable `objdump` SDL2.dll import, successful historic artifact upload line.
NOT DONE: executable bytes, actual SDL surface runtime diagnostics, graphics screenshot, Windows gameplay, crash/repro tests.
CHANGED: this report plus `docs/GRAPHICS_AUDIT_CONTINUITY.md` only. Protected v0.7, main, Mission Builder and other branches untouched.
NEXT: obtain actual Windows runtime surface BPP/pitch/dimensions from a separate diagnostic build or accessible compiled artifact, without touching baseline.
