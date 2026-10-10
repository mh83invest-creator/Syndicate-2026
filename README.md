# Syndicate 2026

This repository is for rebuilding and improving the open-source **SyndicatFX** engine.

## Build plan
1. Reproduce the original SyndicatFX 0.0.6.1022 Windows build using GitHub Actions.
2. Verify the downloadable Windows executable against an existing local Syndicate Plus installation.
3. Address the mission camera edge-scrolling defect (SDL1 and SDL2 versions both reproduce it).
4. Only then investigate rendering smoothness, frame timing, and explosion-heavy slowdowns.

## Copyright and game data
**Do not upload the Syndicate Plus CD image, retail data, compiled commercial assets, saves, or other licensed game content to this public repository.** Original game data stays on the user's Windows machine.

SyndicatFX upstream: https://github.com/swfans/syndicatfx

The first Actions workflow retrieves the upstream source at the release tag, and compiles the *unmodified* engine. No game data is part of the build artifact.

## Status
Windows compilation: pending GitHub Actions verification.
Native game runtime: original distributed executable launches, but in-mission scrolling is defective.
