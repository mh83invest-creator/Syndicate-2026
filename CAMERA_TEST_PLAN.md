# Syndicate 2026 — Camera v0.4 test plan

## Established facts
- Original DOS version in DOSBox scrolls normally.
- SyndicatFX 0.0.6.1022 enters missions and renders sound/graphics.
- Both SDL1 and SDL2 versions exhibit the same mission scrolling bug.
- Windowed and fullscreen SDL2 behave the same.
- User reports camera moves mainly upper-right or lower-left; right/down edge scrolling does not work; top occasionally jumps left, left edge scrolls down.
- GitHub Actions run 38098838630 successfully compiled the **unmodified** native 32-bit Windows engine and uploaded an engine-only artifact.
- **No in-game test of our newly compiled engine has yet been completed.**

## Investigation gate before code edits
1. Check source of `scroll_map` and coordinate conversion in exact release tag 0.0.6.1022.
2. Inspect the shared assembly implementation and confirm which coordinate system camera expects.
3. Capture observed logical x/y coordinate values at all four edges in a purpose-built diagnostic executable.
4. Check whether clipping into the game viewport excludes 639/399; verify edge threshold assumptions.
5. Only then apply a small isolated camera change, avoiding mouse click/agent selection paths.
6. Build on this branch and retain unmodified main branch baseline.

## Acceptance criteria
- All four edges scroll in the intended directions at a comparable rate.
- All four corners provide predictable combined motion without jumps.
- Clicking and agent movement commands remain aligned.
- Windowed and fullscreen test; agent selection and sound unaffected.
- No speed/timing or explosion changes in camera v0.4.

## Publishing
Never commit licensed original Syndicate Plus data or compiled commercial assets. Windows build artifacts must remain game-engine-only.
