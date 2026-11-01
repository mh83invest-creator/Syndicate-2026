# Syndicate 2026 — Consolidated graphics audit and decision-ready handover

Date: 2026-11-01 (Australia/Brisbane)
Repository: `mh83invest-creator/Syndicate-2026`
Audit branch: `audit/graphics-assets-2026-11-01`
Status: **consolidated source and CI evidence audit complete for the inspected rendering paths; Windows gameplay/runtime validation outstanding**. No engine changes authorised or made.

## Purpose and protected constraints
Understand graphics/rendering architecture before modifying it. Protect v0.7, original look/speed and 16 FPS game-timing expectations, main systems audit and Mission Builder. This is not an implementation request. Historical work exists in individual `docs/audits/2026-11-01_*.md` reports and `docs/GRAPHICS_AUDIT_CONTINUITY.md`. This report consolidates the verified evidence, adjusts priorities and specifies test gates.

## Verified rendering architecture
1. `engine/src/display.c:72-95`: `AppScreenSetup` maps legacy mode 19 to 320x200x8 and 18 to 640x480x8, fixes `WScreen` relative to `VGABuffer`, passes `GraphicsPalette` to `LbScreenSetup`. `display.c:244-260` copies logical VGA imagery into WScreen, calls `LbScreenSwap` and handles prior screen-lock status.
2. `engine/src/main.c:29-30,151-178,227-233,266-303`: fullscreen and **low-resolution stretch enabled by default**; `--windowed/-w` disables fullscreen, `--no-stretch/-S` disables stretch. `main` calls `display_set_lowres_stretch(cmdln_lores_stretch)` after initial setup. Default drawing flags choose high-resolution `DrwF_ScreenVres16` at line 273, with an explicit low-resolution option at line 210-212. Do not equate default stretch enabled to proof the 320x200 mode is active in every gameplay frame.
3. `engine/src/display.c:119-125`: stretch enables minimum SDL surface dimension 400; disabled sets 1. `engine/bflibrary/src/x86-win-sdl2/sscreen.c:328-342`: if requested mode's smaller dimension is below minimum, both dimensions are scaled proportionally using integer arithmetic. 320x200 logically requests **640x400 SDL window** under default stretch, while logical draw remains 320x200; 640x480 is not enlarged by 400 minimum. Note help text at `main.c:144-146` claims stretch to '640x480'; **actual SDL2 sizing code yields 640x400** for 320x200. Document this display/help inconsistency rather than silently normalising it.
4. `sscreen.c:344-378,497-535`: requested mode maps 8/15/16/24/32 bits to SDL format hints; actual SDL window surface obtained with `SDL_GetWindowSurface`, and secondary draw surface created when mismatch or control requires. `sscreen.c:290-325`: secondary surface can wrap app-owned 8-bit WScreen buffer via `SDL_CreateRGBSurfaceFrom` under `!LB_WSCREEN_CONTROL`. Flags/preprocessor mode matter. `LbIScreenDrawSurfaceCreate` has a conditional palette-copy failure path where the surface is freed but `lbHasSecondSurface` flag is left true; normal initial setup supplies non-null palette and calls with `set_palette=false`, so that particular palette-copy failure is **not normal initial creation**.
5. `sscreen.c:976-1006`: equal dimensions uses `SDL_BlitSurface`; dimensions differ with nonindexed src uses `SDL_BlitScaled`; differing dimensions with 8-bit indexed source uses custom indexed-to-indexed if destination BytesPerPixel==1, else custom indexed-to-true-colour. Custom conversion feeds source SDL palette colours and destination RGB shifts.
6. `sscreen.c:924-971`: custom true-colour scaler writes `*((long *)(dst_buf+dst_offset))` then advances `dst_offset += dst_bpp`. It does not receive SDL `pitch`; indexed scaler `sscreen.c:878-922` also assumes packed row layout. On the 32-bit MinGW target `long` is 4 bytes: conditional overlong stores are a concrete concern if destination has 2- or 3-byte pixels; separate ignored pitch risks padded rows even at 4-byte pixels. Earlier graphics audit synthetic AddressSanitizer harnesses reproduced overflow for tight 2/3-byte models and changed padding bytes in a padded stride model; these are **synthetic, not native Windows gameplay repros**.
7. `sscreen.c:988-1004`: failed SDL surface locks log errors but do not abort custom scaler call; unlock attempts remain unconditional when `SDL_MUSTLOCK`. `sscreen.c:1050-1124`: swap and swap-clear ignore restore/check results; they check blit/window-update errors, while SwapClear ignores `SDL_FillRect` status. `sscreen.c:1127-1187`: SwapBox path also ignores restore/check and resets `ret` to success after lock/copy logic; SwapBoxClear is explicitly unimplemented and asserts. Do **not** claim actual app invokes this function without a caller trace.
8. `sscreen.c:727-746`: `LbIScreenSurfaceRestoreLost` reacquires window surface on every swap. SDL2 `swindows.c:104-109` handles focus gained/lost; SDL1's different focus palette restoration was audited separately.
9. `engine/bflibrary/src/x86-win-sdl2/spalette.c:50-100`: palette 6-bit input masked by 0x3F, expanded via <<2 and applied separately to eligible SDL screen/draw palettes. Internal arrays and `lbDisplay.Palette` mutate before or notwithstanding an SDL failure, no rollback; normal late setup errors trigger reset, but generic `LbPaletteFade` ignores LbPaletteSet result.
10. `engine/bflibrary/src/x86-win-sdl2/sscrsurf.c:44-67,140-175`: offscreen surfaces are created with draw format; indexed blits temporarily replace offscreen SDL surface palette pointer with the draw surface palette pointer then restore it. This relies on SDL internal palette pointer behaviour across the blit, not independently native-tested; unlike the scaler memory-write risk this is an unvalidated ownership/aliasing concern, not a demonstrated breakage.
11. `engine/bflibrary/src/general/gpalette.c:38-106`: generic fades ignore get/set and timed swap errors and interpolate with divisor `fade_to` without local zero guard. **No direct LbPaletteFade caller found** in all 17 `engine/src` C sources or all 59 `bflibrary/src/general` C/C++ sources (prior completed bounded inventory). Remaining platform-specific, assembly and indirect paths not exhaustively excluded; zero-input gameplay reachability not established.

## Actual Windows v0.7 build evidence
- Root `.github/workflows/windows-source-v0.7.yml`: 32-bit MSYS2 MINGW32 target `i686-w64-mingw32`, installing SDL2 and building with configure/make.
- GitHub Actions successful main run **38483184953**, source SHA `a22b0b9580a9710b4e693e88854351835ef1ee40`, run date 2026-10-13; successful job **115590427796**. Real job log `objdump -p release/src/syndicatfx.exe` printed **`DLL Name: SDL2.dll`**, confirming the compiled Windows v0.7 executable uses SDL2. Historic job log shows successful executable artifact upload ID 11858508104; currently retrievable executable bytes were not confirmed. No SDL runtime w/h/format/pitch captured.

## Prioritised findings and disposition
**P1 — Custom scaler memory-layout risk (verified source, runtime unverified).**
The 4-byte `long` write is unconditional regardless of dst BytesPerPixel, while offset increment uses the latter; both custom scaling paths ignore pitch. Test 320x200 -> 640x400 default-stretch scenario in isolated SDL2 Windows diagnostics and obtain actual SDL surface BytesPerPixel, pitch, dimensions. Include 16-,24-,32-bit destinations and deliberately padded rows as stand-alone fixtures. A proposed fix would need correct width-qualified stores, per-row pitch and bounds, preservation of palette and pixel packing, subject to approval.

**P2 — Surface errors and state consistency (verified source, runtime unverified).**
Failures ignored on lock, restore/check, fill and partial box-swap paths. Distinguish a genuine SDL error from normal alt-tab and mode transitions. Do not patch all paths blindly.

**P2 — Palette and offscreen ownership (verified handling, runtime effect uncertain).**
SDL palette updates not atomic; generic fade silently continues on errors; offscreen blit shares SDL palette pointer temporarily. Inspect ownership/SDL reference semantics with a real SDL surface test.

**P3 — Generic fade divisor (conditional source defect, reachability unproven).**
No direct calls in 76 scanned application/general C/C++ files. Avoid ranking as a demonstrated gameplay crash.

**Functional UI/display inconsistency.**
Help says low-resolution stretch to 640x480, source requests 640x400. Do not change documented behaviour or visuals without approval.

## Comprehensive audit boundary and validation plan
This is a consolidated source/CI audit across the tracked graphics concerns; **not a claim that every pixel-producing legacy assembly routine or every asset and rendering subsystem has been manually inspected in this turn**. Graphics source history provides separate mapping of sprite banks, palette, viewport/dirty-grid etc. Native validation, screenshots and complete assembly/assets/gameplay exhaustiveness are outstanding.

Required native matrix before code modifications:
- 320x200 and 640x480 modes; stretch on/off; windowed/fullscreen; fresh start, transitions and alt-tab.
- Log SDL2 source/destination w,h, BytesPerPixel, BitsPerPixel, pitch, buffer allocation extent and selected blit route per transition (isolated diagnostics only).
- Compare 8,16,24,32-bit window formats where obtainable; use standalone synthetic fixtures for unattainable SDL formats and make that distinction.
- Exercise all swap paths and palette state under controlled SDL error injection.
- Recheck original gameplay framerate, camera scrolling, explosions, sprite colours, menus, screen scaling, save-game preservation and screenshots against protected baseline. No present native tests performed.

## Verification and change ledger
PASS: audit-branch reads of `main.c`, `display.c`, SDL2 `sscreen.c`, `spalette.c`, `sscrsurf.c`, `swindows.c`; previous source and synthetic harness audits; GitHub Actions successful Windows build/objdump evidence.
NOT RUN: live Windows build in this turn, game runtime/screenshot comparisons, source patch verification. Synthetic tests described above originate from prior audit turns.
CHANGED: this consolidated report and graphics continuity index only on audit branch.
NO CHANGES: engine, original gameplay, protected v0.7, main, Mission Builder or other concurrent branches.

## Decision
Source/CI audit is sufficient to identify and rank the major SDL2 risk candidates, **not sufficient to assert the graphics subsystem is fully certified or to ship a fix**. Next phase should be a deliberate, separately approved isolated diagnostic Windows build with native surface logging rather than more speculative micro-audits.
