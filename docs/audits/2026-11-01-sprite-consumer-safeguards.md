# Syndicate 2026 — Sprite consumer safeguards

Date: 2026-11-01. Static audit only.

Source: engine/bflibrary/src/general/gsprite.c lines 27–61; engine/src/syndre.sx lines 27719–27737, 31252–31263 and 32235 onward.

LbSpriteSetup iterates from start to end and relocates each sprite Data pointer. It has no explicit null validation of start, end or data, and does not independently validate the sprite table's recorded extent. LbSpriteSetupAll checks its descriptor pointer fields but does not validate their dereferenced sprite buffers before calling LbSpriteSetup.

The mission setup assembly invokes ac_LbSpriteSetup after the unchecked mission resource load count; graphics initialisation invokes a different ApSpriteSetup_ForceHeight routine whose implementation has not yet been located. World-map display_cities calls drawing routines, but downstream guards remain unverified.

Consequence: a failed or malformed resource is a plausible risk for native sprite setup; no runtime fault reproduced. Next verify actual wrapper mapping, table bounds and remaining rendering consumer safeguards. Keep custom-mission resource checks and campaign-save isolation independent. No code changes, compilation, runtime tests or save operations. v0.7 remains protected.
