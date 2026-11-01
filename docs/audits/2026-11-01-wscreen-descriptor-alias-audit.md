# Syndicate 2026 — WScreen descriptor and temporary alias audit
Date: 2026-11-01 (Australia/Brisbane)
Status: source inspection only, not runtime verified.
Repository: mh83invest-creator/Syndicate-2026; main.
Previous handover: docs/audits/2026-11-01-world-map-destination-pointer-provenance.md.
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf; engine/src/display.c; engine/src/main.c.

## Findings
1. syndre.sx lines 83901-83906 declare global WScreen and VScreen pointer storage and BScreen as a separate pointer. Declaration is NOT backing allocation.
2. syndre.sx load_files_vres16 descriptor at 83626-83632 refers to WScreen and contains explicit decimal size 256048, with source comment '256016 is too little, need 320x400 + 24 + 320x400 + some safety bytes (24)'. This is a loader-request/descriptor figure, not a proven successful live allocation or invariant for every graphics mode.
3. syndre.sx load_files_mcga descriptor at 83651-83656 similarly targets WScreen and has size encoded as bytes 0x10,0xE8,0x03 (little-endian 256016 decimal). The actual descriptor format, loader allocation semantics and conditional paths need a separate check before declaring these as confirmed allocated capacities.
4. Source references to these descriptors in assembly include 31141, 31242, 31249, 31328; the sequencing and active graphics mode remain to be confirmed.
5. syndre.sx 56471-56473 temporarily replaces exported WScreen with BScreen; 56506 restores it from EBP. Thus WScreen is sometimes an alias to another region during sprite drawing, not necessarily the original base pointer at every instant. This specific path does not by itself prove a map draw occurs while the pointer is swapped.
6. display.c creates a distinct VGABuffer via LbMemoryAlloc(640*480*2 + 16) and AppScreenSetup uses a pointer derived from VGABuffer for lbDisplay.WScreen. Neither identifies the independently referenced WScreen allocation.
7. Previous audit confirmed _draw_map_area is called twice with BScreen and twice with WScreen, with a caller-supplied destination pointer and 320-byte stride. Nominal horizontal spill for territories 14, 44 and 45 is not established to be out-of-allocation.

## Risk / open investigation
- Establish native loader's descriptor memory-size convention, exact allocation, error paths and disposal before labeling either decimal size a safe capacity.
- Trace which resource loader descriptor has completed before map_selection and which BScreen assignment is live for each caller.
- Trace 56506 restoration and any unexpected exit paths.
- Confirm the precise world-map destination address, last byte written (accounting for nonzero source pixels), and total allocation under 320x200 and other modes.
- Use known-good v0.7 and legally held retail assets for later Windows tests. No visual anomaly, crash or memory error was reproduced.

## Handover verification
Performed read-only GitHub source inspection of assembly declarations, descriptor bytes, pointer assignments and existing C buffer initialization. No executable changes, save changes, mission changes, Windows compile, instrumented runtime checks or visual tests. Mission Builder branch and campaign-save isolation design remain protected.
