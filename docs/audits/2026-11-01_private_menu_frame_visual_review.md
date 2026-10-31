# Syndicate 2026 — Private Original Menu First-Frame Visual Review

Date: 2026-11-01 (Australia/Brisbane)
Status: focused offline visual inspection complete; actual Windows/SDL game output untested.
Branch: feature/v0.9-visual-showcase. Protected main v0.7 commit a22b0b9580a9710b4e693e88854351835ef1ee40.
Scope: inspect privately reconstructed indexed first-frame images for mtitle.dat, mselect.dat and mbrief.dat from the user's historic v0.6 test ZIP. No images or commercial asset bytes were committed to public GitHub.

## Evidence and findings
- The independently decoded 320x200 indexed first frames and their 256-colour 6-bit palette chunks were converted privately to RGB by applying the same 6-bit-to-8-bit scaling convention as the existing SDL palette setter. Internal image review confirms the reconstructed mtitle.dat first frame depicts the historical SYNDiCATE lettering/Bullfrog identification composition rather than an abstract blank screen.
- The reconstructed mselect.dat and mbrief.dat first frames depict predominantly green, dark strategic-console themed image compositions with thin luminous green borders and graphical highlights. These are private offline reconstructions, NOT captured Windows runtime frames and not a full composited menu with active UI text, cursor and dynamic widgets.
- The existing visual character can therefore be studied without inventing menu artwork. It would still be unsafe to conclude that changes to the underlying animation file alone automatically update in-game menu layout or hitboxes.
- Neither the user-approved new Syndicate 2026 in-game title treatment nor other requested UI styling has been implemented. The only v0.9 source delta remains the uncompiled native window title string.
- Actual menu navigation, animation timing, 320x200-to-output scaling and text readability must be tested in a real Windows build before accepting a visual rewrite.

## Verification
PASS: private 320x200 indexed first-frame reconstruction, palette application and human review of three resultant images.
NOT RUN: full animation palette evolution, pixel comparison with actual engine screenshots, options/briefing overlays, Windows gameplay, CI or visual redesign.
OPEN: frame-to-frame presentation and cursor/HUD overlay separation; detailed menu gadget draw coordinates and fonts.

## Next
Complete SS2 palette and frame-color handling, correlate the drawing order with interactive overlay functions, and capture original Windows v0.7 screen images when the user is available for testing. Keep all copyrighted images inside private test environments; only observations/metrics belong in public GitHub.

No gameplay, engine, CI, asset or protected main changes. Rollback: documentation-only feature-branch entry.
