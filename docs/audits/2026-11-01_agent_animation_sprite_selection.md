# Syndicate 2026 — agent animation sprite selection and bounds

Date: 2026-11-01 (Australia/Brisbane)
Branch: audit/graphics-assets-2026-11-01
Status: focused original assembly source audit; no engine changes.

## Exact source evidence
Original assembly blob c35e22621a93d1e07c44d40de8867446ff7c64bf (engine/src/syndre.sx).

- 19173–19194: draw_agent_element takes a frame number, obtains frame +0's first-element index and addresses elements_ani using 10-byte element stride. A lower/base-pointer comparison avoids traversing an element index that addresses the base.
- 19195–19212: the agent-specific path derives an offset using a pointer relative to level__People and reads separate agent-related data.
- 19213–19246: draw_agent_element reads an element's sprite field (+0), but for certain character/sprite state combinations computes a replacement descriptor offset rather than using the element's original sprite offset. It adds the h_sprites base and tests only whether the resulting pointer exceeds that base.
- 19247–19274: the selected descriptor is drawn using DrawASpriteHires16_A; the element link at +8 selects the next 10-byte element. In the inspected loop, next-element continuation compares only against elements_ani base.
- 18829–18890: a separate drawing path uses element +0 sprite offset, draws the sprite, then calls set_block_changes before following element +8, showing sprite render/update logic is not uniform across all call sites.

## Consequences
A decoded animation element's +0 sprite reference is not always the sprite actually drawn for an agent. At least one agent drawing path performs a state-dependent alternate sprite descriptor selection. A future offline image viewer that ignores that substitution could misrepresent the original appearance or frame sequence.

No inspected agent loop checks the computed descriptor against h_sprites_end or the element against elements_ani_end. This is a conditional bounds/cycle concern, not proof of malformed retail resources or a reproducible crash. Distinguish the existence of a comparison against the base pointer from an actual range validation.

## Required follow-on
Trace the alternate sprite selection arithmetic and the agent state source; verify candidate offsets are aligned with six-byte descriptors. Validate private animation links and sprite offsets against the decoded DAT/TAB resources; check actual rendering under Windows before any modifications.

## Verification
PASS: direct GitHub original assembly blob inspection, exact call-site trace.
NOT RUN: binary sprite/animation resource validation, build, Windows gameplay, graphics comparison, profiling.
Changes: documentation only on graphics branch; protected main, core audit and Mission Builder unchanged.
