# Syndicate 2026 — Enhanced Cyberpunk Graphics Roadmap
Date: 2026-11-07 (Australia/Brisbane)
Status: user-approved direction/order; roadmap documentation, not implementation.
Milestones EG-v0.1 through EG-v0.4 are GRAPHICS WORKSTREAM LABELS, not app versions. Protected main/v0.7 stays unchanged.

## Approved visual direction
Enhanced Cyberpunk. Modern, detailed, oppressive corporate dystopia: the world establishes the materials, lighting language and level of detail. Preserve original Syndicate silhouettes, urban organisation, world geometry, simulation, controls, frame pacing and tactical visibility. Avoid cartoonishly oversaturated neon.
**No scanlines or CRT filter** (neither required nor planned).
User-approved order, transposed from first proposal: **3 → 2 → 4 → 1**.

## EG-v0.1: Environmental graphics — FIRST
Visual goal: improve buildings, facades, roof surfaces, roads, intersections, pavements, industrial materials, water, terrain seams and scenery props. Set materials and texture-detail hierarchy that works at native gameplay scale.
Technical work: map environment resource category to renderer consumers; verify map-column and block-face structures against local assets, not guessed labels. Use user-verified first mission as representative comparison where its exact asset set can be identified. Keep exact geometry, draw order, palette safety, collision, occlusion and scrolling. Determine feasibility of replacing source art versus isolated enhanced visual pass before choosing implementation.
Deliverable: evidence-backed environment asset/render-path map; environment art direction specification; one narrowly scoped environmental prototype on an isolated graphics development branch; paired Windows screenshots and regression/performance checks. No mission data/AI changes.
Gate: player can read/build/test one visually improved environment without regression or changes to game mechanics, and the user approves its aesthetic before expanding.

## EG-v0.2: Environmental lighting and atmosphere — SECOND
Depends on accepted EG-v0.1 environment art direction. Selective neon signage, windows and industrial lights, shadow depth, optional contextual haze and wetness only where supported by the environment. Investigate indexed palette, scene composition and HUD masking before selecting renderer approach. Avoid bloom/neon wash and tactical obscurity.
Gate: coherent visuals across representative environment scenes, tested windowed/fullscreen, palette transitions and performance. Preserve source gameplay and input.

## EG-v0.3: Characters and vehicles — THIRD
Depends on EG-v0.1/v0.2 style and contrast. Improve readability and definition of agents, civilians, security and vehicles; integrate their material/shadow/light treatment. Preserve animation timing, per-frame placement, hitboxes, collision and selection; independently verify sprite descriptors/animation references with local original assets before editing.
Gate: matched scene comparisons in motion, all relevant zoom/modes, no sprite clipping, palette or performance regressions.

## EG-v0.4: Weapons and combat effects — LAST
Depends on environments, atmosphere, characters and vehicles. Upgrade muzzle flashes, projectile/tracer visuals, sparks, impact marks, explosions, smoke and flame to fit established art direction. This is a VISUAL pass, not authorisation to alter weapon stats, AI, combat rules or gameplay timing. Explosion slowdown is a pre-existing priority.
Gate: high-load explosion performance at least as good as the preserved working build; visuals match environment; no gameplay/save/audio regressions.

## Architecture and constraints (source-backed)
- `engine/src/display.c` maps original logical 320x200 and 640x480 indexed modes and exchanges WScreen/VGABuffer through SDL screen swap.
- `engine/bflibrary/src/x86-win-sdl2/sscreen.c` handles SDL surface scaling and display conversion: presentation scaling cannot invent new texture detail.
- `engine/src/syndre.sx` contains original environment render consumers; `docs/audits/2026-11-01_graphics_block_face_crosscheck.md` identifies 12,288 rebased map-column pointers, +10/+11 column selectors and 24-byte block descriptors accessed at offsets +0,+4,+12,+16,+20 in inspected paths. Those fields are not yet proven road/wall/window categories.
- `docs/audits/2026-11-01_graphics_asset_parallel_audit.md` reports audited compressed environment assets including `hblk01.dat`, palettes shared across missions and separate sprite banks. The decoded asset lengths and candidate table layouts do not prove their artistic meaning; private original data cross-check remains required.
- `docs/GRAPHICS_WINDOWS_RUNTIME_VALIDATION.md` gives comparable Windows rendering test expectations; prior graphics-scaler isolated CI was green. User successfully played first mission on graphics repair candidate and reported smoother subjective feel, not measured FPS.

## Working and release contract
One approved phase at a time; no related/unrelated feature expansion. Preserve protected v0.7 and prior working v0.8 prototype; work in independent branch with tests and rollback. No original copyrighted commercial game assets or reconstructed game frames committed publicly. Record each change's paths, version, source SHA, compilation result, Windows runtime observations, performance measurements and limitations. Do not claim source audits are runnable enhancements.
**No EG implementation is complete as of this roadmap.** Start EG-v0.1 source/asset mapping, then one representative prototype, not weapons or lighting.
