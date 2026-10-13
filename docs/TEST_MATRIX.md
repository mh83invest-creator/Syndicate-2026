# Syndicate 2026 — acceptance test matrix

Status codes: **PASS** = user tested; **FAIL** = user tested and failed; **PENDING** = untested for a new build.

| Test | v0.6 history | Next source-based build |
|---|---|---|
| Launch into menu | PASS | PENDING |
| Mission gameplay | PASS | PENDING |
| Music and effects | PASS | PENDING |
| Camera edge scroll: left/right/up/down | PASS | PENDING |
| Menu responsiveness | Improved per user | PENDING |
| Intro playback | FAIL / missing | PENDING |
| Explosion-heavy performance | Known slowdown | PENDING |
| Gameplay simulation speed unchanged | No reported regression | PENDING |
| Window/fullscreen behavior | Not established | PENDING |
| Binary built from committed modified source | NOT ESTABLISHED | PENDING |

Do not mark a new build PASS based only on a successful CI compile. Record executable artifact, source commit, configuration and user testing separately.
