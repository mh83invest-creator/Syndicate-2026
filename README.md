# Syndicate 2026 — Working v0.7 source baseline

This repository tracks the **actual native source** of Syndicate 2026,
derived from public GPL SyndicatFX tag `0.0.6.1022`, plus changes validated
by Windows playtests through v0.7.

- `engine/`: complete imported source with camera edge scrolling fix,
  native Windows engine and 30 FPS pacing on company configuration,
  world-map selection and briefing tactical map.
- Mission loop remains at original 16 FPS. Other menu loops unchanged.
- `.github/workflows/windows-source-v0.7.yml` builds the committed source
  on Windows i686 and rejects MSYS runtime audio dependencies.
- `.github/workflows/restore-v0.7-source.yml` preserves the reproducible
  baseline import recipe. Do not re-run casually after new development!
- **No proprietary Syndicate retail game data is committed**. Users must
  supply their legally obtained game data separately to run builds.
- Development continues here in `main`; historical branches remain
  available for recovery. Prior working v0.6/v0.7 test packages should
  stay separate until subsequent builds pass gameplay tests.

## Provenance

Upstream: https://github.com/swfans/syndicatfx tag 0.0.6.1022.
Validated assembly, camera and timing changes recreated from
`diagnostics/camera-v0.4` workflow `menu-fps-v0.7.yml`.
License and original author files are retained under `engine/`.
