# Syndicate 2026 — Asset-Informed Feasibility Addendum

Date: 2026-11-01 (Australia/Brisbane)
Status: completed synthesis of the current static-source and private-asset investigations; NOT a finished whole-engine audit or a Windows gameplay certificate.
Source branch: feature/v0.9-visual-showcase. Protected baseline: main v0.7 a22b0b9580a9710b4e693e88854351835ef1ee40.
Source of truth: docs/CONTINUITY_INDEX.md and docs/SOURCE_AUDIT_DECISION_REGISTER_2026-11-01.md together with newer docs/audits/ stage reports.

## What is now materially better supported
- The two historical uploaded Windows packages (v0.6 and failed v0.8 prototype) carry identical private data/ content for all 437 members by SHA-256. The engine binaries are NOT equivalent; v0.8 user playtest failed.
- Private originals are now present for offline study. 143 of 437 data members carry RNC1 headers and all 143 packed payloads pass header-size and packed CRC16 checks. The repo contains original RNC decoder source. An independent Python decoding trial passes decompressed length and CRC for 103/143; 40 fail the experimental decoder, which requires investigation and is not proof of corrupt game assets.
- The first three original high-resolution palettes decode to 256 RGB triplets with valid CRC and 6-bit channels; the other two remain unverified at decompressed level. The source still confirms five original palette IDs shared across the original level table. A palette-only adjustment is insufficient for localized lamps, vehicle headlights, windows or neon, and may affect unrelated missions and UI.
- Fifty-nine original map resources are available; 48 independently decoded and passed 128-by-96-by-12 header plus 12,288 offset-table alignment and in-file bounds checks, cross-confirming the original map assembly's fixed lookup count. Eleven other maps failed the experimental decoder, not the packed integrity test. Block identity/occlusion and windows/sign positions remain unclassified.
- Original menu/title art was investigated as actual private files, not just C platform-window title. Six original menu animation containers use 320-by-200 frame headers, palette/BRUN/SS2 chunks. All six declared indexed frame sequences were reconstructed by a private read-only decoder with bounded writes. Three first frames were visually inspected privately. That is tangible baseline artwork evidence but NOT an actual game screenshot, menu hitbox test or finished UI improvement.
- Original assembly and C display audits now trace phase-dependent WScreen, VScreen, PalMem and GraphicsPalette pointer behaviour, the SDL2 pixel/surface/lock lifecycle and the default WScreen-control setting. Render-compositing risk is materially characterized, but no safe object-local lighting or window-overlay implementation has been demonstrated.
- Explosion effects/pool and slow-down hypotheses remain static; no real CPU frame timings or benchmarks have been measured.

## Current decisions and nondecisions
- Keep SyndicatFX. FreeSynd remains read-only reference. No controller work.
- Keep v0.7 main as gameplay baseline; failed v0.8 speed experiment excluded. v0.9 has only its previously recorded native title string source change, uncompiled/unplaytested.
- The source and original assets support a plausible targeted v0.9A *menu presentation study* but not an approved renderer rewrite or claim of working v0.9A UI.
- Nightlighting requires actual object/scene classification, draw ordering and Windows captures; five existing palettes alone are not the effect described in the approved specification.
- Explosion slowdown must be measured before optimization; retain all original effect visuals and 16 FPS mission cadence.

## Outstanding gates before claiming implementation feasibility or audit closure
1. Establish independent RNC full decode parity using the repo's original C implementation or another validated decoder; require checks over all 143 compressed private entries, including hpal04/hpal05, hblk01 and hspr-0.
2. Correlate decoded block/column/sprite semantics with original assembler map rendering, projection, occlusion and object draw order; do not invent luminaire identities.
3. Establish safe scene-versus-HUD/cursor overlays and buffer ownership at runtime, including focus/surface restoration and pixel formats.
4. Validate menu composition and hitboxes against actual 320x200-to-window output and source animation frame behaviour; do not confuse the platform window title with in-game title.
5. Run native i686 compilation, automated library graphics checks, controlled idle/traffic/explosion profiling, and Windows gameplay comparisons with the known-good v0.7 package.
6. Record actual user gameplay, sound, scrolling on all edges/corners, top-edge drift, focus/Alt-Tab, menu navigation, mission start/end and image legibility. Static-source review is not substitute evidence.

## Verification classification
PASS: GitHub source reads, previously logged blob comparisons, archive member hashing, packed RNC CRCs, subset decoded CRCs, subset map structure checks, six animation frame reconstructions and private first-frame visual observations.
NOT RUN: original C decoder compilation, full decompression verification, native Windows executable build, automated CI gameplay suite, actual SDL display/lighting capture, profile/focus/audio tests.
NOT IMPLEMENTED: full v0.9A menu redesign, nighttime light emitters, performance optimization, camera slider.

## Handover
Read this addendum after the decision register and stage-specific reports. Continue source-only decoding and map object classification where feasible; keep private assets outside GitHub. The first unavoidable user-assisted gate for *actual gameplay verification* will be the historical working v0.7 Windows installation/screenshot and testing, to be requested only when ready. All feature branch documentation remains distinct from stable main. No gameplay code has been touched during these audit stages.
