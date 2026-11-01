# MB-STRUCT-20 — Installer, startup and resource-path boundaries

Date: 2026-11-01 (Australia/Brisbane)
Workstream: Mission Builder, audit only; no implementation.

## Verified repository source
- `engine/util/install` lines 183–189 attempts to copy `data/gameNN.dat` with explicitly enumerated IDs **01–50 and 90–99** (60 candidate files). These are **installer-listed names**, not independently verified existing retail assets, not a full resource manifest, and not a proof of occupancy of all other indices.
- The same installer separately copies map resources from a non-contiguous number list (191–193) and mission text `data/missNN.dat` for 01–50 and language-qualified text `data/missNNN.dat` for 101–150, 201–250, 301–350, 401–450 (195–213). Map, mission image and text namespaces must not be conflated.
- `engine/src/main.c` (284–348) processes command-line options, initializes game facilities and calls `syndicate()`. No source-verified American Revolt/custom-campaign selector emerges from this inspected startup path.
- `engine/src/game.c` (84–89, 114–138) registers resource mapping and transforms `save/` paths to `GetDirectoryUser()`; non-save relative files resolve under `GetDirectoryHdd()`. It handles absolute paths separately. This path router is broad, not campaign-aware.
- `engine/src/dos.c` (106–123) calls `game_transform_path` before DOS-level opens, providing another path through the common transformation. It does not prove all `LbFile*` reads or script-level operations are automatically isolated.
- `syndre.sx` loader resolves `data/gameNN.dat` with a two-digit mission number (MB-STRUCT-15–17), while the installer has a definite copy list. This **narrows observed protected native filenames** but does not establish safe free IDs or full American Revolt/other-media resource coverage.

## Integration boundaries and requirements
1. Original installer-listed mission filenames are protected; never place custom files over them. Treat every additional native gameNN index as reserved/unknown pending authoritative data inventory.
2. Original, American Revolt, World Domination and each Custom Campaign must keep distinct selectable identities, resource isolation and separate save namespaces; saves remain between missions only.
3. A dedicated custom resource lookup is a *possible future design*, not verified legacy behaviour or an approved engine patch. Global path transform changes could inadvertently affect map/text/graphics/sound or saves and require separate source review and testing.
4. Installation and launch currently have no verified custom pack setup. Asset packaging, campaign selector and compatible save routing remain unresolved.
5. Do not infer production asset availability from copy loops alone or claim American Revolt works based on filename enumeration.

## Next scoped audit
MB-STRUCT-21: inspect `install_file` and resource path mappings for overwrite and missing-asset behaviour, then document a non-destructive custom pack distribution contract. No distribution implementation until approved.

## Verification
Read `engine/util/install`, `engine/src/main.c`, `engine/src/game.c` and `engine/src/dos.c` via GitHub. No source edits, tests, build, actual installer execution, retail assets or Windows gameplay validation.
