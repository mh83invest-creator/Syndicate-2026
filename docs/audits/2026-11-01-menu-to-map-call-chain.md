# Syndicate 2026 — Menu-to-map initialization and destination trace
Date: 2026-11-01 (Australia/Brisbane)
Status: Static source audit only; no source modifications or runtime tests.
Repository: mh83invest-creator/Syndicate-2026, main.
Predecessor: docs/audits/2026-11-01-world-map-menu-buffer-sequencing.md
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Verified control flow
- select starts at line 54149, first loads UI resources at 54156–54163 and then processes sprites and menu preparation.
- At lines 54239–54255, menu preparation conditionally switches DrawFlags=1 / AppScreenSetup(0x13) for data_55590 values 2 or 3, then unconditionally (within the shown path) sets BScreen = WScreen + 0xFA00 (64000).
- The code proceeds through selection menu dispatch (jump table at 54135–54145 and branch around 54265–54271).
- The menu handler func_37b29 at 54593–54597 directly calls map_selection. Literal search in the inspected assembly blob yielded a single 'call map_selection' at line 54597.
- map_selection loads mmapblk resources at 31719–31725 and calls display_cities at 31727, then the main loop calls LbCopyScreenFromBackMCGA at 31731. Display overlays call _draw_map_area with BScreen (31769–31779), while two later drawing paths use WScreen (32738–32748 and 32783–32792).
- This establishes a normal main-menu route from BScreen assignment through menu dispatch to map_selection. It does not establish all possible alternate entry paths, external launchers, or absence of BScreen reassignment along every helper call.

## Memory implications
- Menu setup points BScreen 64000 bytes beyond WScreen. The successful synthetic allocations previously established (256016 MCGA and 256048 vres16) have sufficient total nominal span for two contiguous 64000-byte frame regions. This is conditional on the correct resource descriptor being loaded and pointer lifetime being preserved.
- The actual _draw_map_area writes use a linear 320-byte row pitch and no local right-edge clipping. Some built-in coordinates extend horizontally beyond nominal column 319 (prior audit indices 14, 44, 45). Whether this is intentional visual row-wrap cannot be decided from allocation arithmetic alone.
- The distinct WScreen+128000 and WScreen+128024 BScreen assignments elsewhere in assembly require future control-flow/lifetime scrutiny, especially if mode switching occurs.

## Outstanding checks
1. Trace any BScreen writes or resets between menu setup and each world-map drawing call, including helpers entered via jump tables.
2. Derive actual max destination offset from each built-in territory coordinate using accurate screen pitch and full extent for each of the four call sites; classify linear allocation safety separately from visible row-wrap.
3. Confirm resource loader success, WScreen lifetime and frame-buffer mode during menu transitions.
4. Gameplay visual checks for territory indices 14/44/45; 32-bit Windows build with legally held assets and runtime instrumentation.

## Testing and cross-branch policy
Performed: GitHub assembly read and static caller / branch tracing. Not performed: executable launch, compile, memory instrument, pixel test or retail-data test. No game files or saves altered, no Mission Builder branch edits, original v0.7 baseline protected. Campaign-save namespace design remains separate.
