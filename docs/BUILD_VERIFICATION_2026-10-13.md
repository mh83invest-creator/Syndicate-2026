# Syndicate 2026 — CI verification (13 October 2026)

## Completed GitHub Actions runs

| Run | GitHub commit | Outcome | What it validates |
|---|---|---|---|
| [#5](https://github.com/mh83invest-creator/Syndicate-2026/actions/runs/38389111890) | `9aaa1fb5dc89ca63794ce5d2b171c41ee242de11` | SUCCESS | Two repository checkouts, patch handler, toolchain, 32-bit dependencies, source compilation, packaged executable |
| [#6](https://github.com/mh83invest-creator/Syndicate-2026/actions/runs/38389317589) | `c4c58f9192564bdac8e12cc5a4f56817062206ad` | SUCCESS | All preceding stages plus source-provenance capture and upload |

Run #6 published:
- `syndicatfx-windows-i686-patch-workflow` (compiled engine artifact, no retail data).
- `syndicate-2026-i686-dependencies` (build-time library bundle).

**IMPORTANT:** There were no custom `patches/*.patch` changes in these runs. They compiled clean **upstream SyndicatFX 0.0.6.1022** through the new, patch-ready pipeline. They are *not* verified replacements for the user's v0.6 playable build. This record verifies CI functionality, **not gameplay correctness**.

## What remains unverified
- Four-direction camera edge-scrolling functionality.
- Faster menus and original gameplay-speed stability.
- Audio and intro playback.
- Performance with many explosions.
- Reproduction of user-tested `Syndicate_2026_v0.6_Two_Menus_30FPS_Test.zip` modifications from committed engine source.

## Next implementation work
1. Trace camera/mouse event interaction in upstream code and decompiled/reassembled game logic.
2. Commit the scrolling correction as an explicit source patch.
3. Rebuild and regression-test in a local game-data installation without uploading proprietary assets.
4. Only then promote a new playable build version.
