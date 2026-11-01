# Mission Builder — Read-only objective inspector verification (2026-11-01)

Branch: feature/mission-builder-audit
Tool: tools/mission_builder/mission_inspector.py
Tests: tools/mission_builder/test_mission_inspector.py
Scope: genuine user-supplied original-style game resources, private local ZIP only; no commercial assets committed.

## Verified local findings
- All 60 gameNN.dat resources expanded with exact 116010-byte size and expanded CRC verification using previously committed RNC decoder.
- Inspector decoded 8 tactical records x 60 = 480 record slots, each 14 bytes at relative offset 0x1BD32.
- Inspector decoded 128 CP objective records x 60 = 7680 record slots, each 15 bytes at relative offset 0x1BDAA.
- Distinct map numbers across missions: 53.
- game01.dat: map number 1; tactical objective slot zero has type 2, status 0, target offset 738. One nonzero tactical objective and two nonzero CP records.
- Type-frequency across 480 tactical slots: 0:370; 1:25; 2:22; 3:5; 5:4; 10:1; 11:12; 14:8; 15:3; 16:30. Nonzero type does NOT establish verified objective story semantics.
- Assertions passed for 60 unique files, fixed expanded lengths, counts, ordinal slots, type range and game01 fixture. Prior six negative RNC tests also passed again locally.
- Tests operated read-only with ZIP API; no assets modified and no network upload of game resources.

## Implementation
mission_inspector.py consumes rnc_readonly.py from same directory, supports archive overview, --mission game01.dat selection, --json and --all-cp. By default it suppresses all-zero CP entries in display only; the parser still reads all 128 per mission.
test_mission_inspector.py contains fixture-dependent regression assertions; the local source-matched scripts were executed. The GitHub copies were committed, not separately downloaded and executed after commit.

## Interpretation limits
Coordinate field names x/y are positional interpretations from the assembly layout; the last word remains z_or_parameter until semantics are confirmed. CP fields field_4/field_b/field_d deliberately retain unnamed labels. No reference target-class validation or runtime mission completion test has been performed. The tool is for private development data, not a hardened service for arbitrary hostile files.

## Preserve
Protected v0.7, native engine, 16 FPS game speed, original and American Revolt, graphics, saves and commercial assets unchanged.

## Further engineering
Cross-correlate objective type and target offsets with entity pools, map coordinates, and gameplay evidence. Add more negative structural fixtures before any write-capable editor. Campaign-save integration and Windows runtime testing remain separate open gates.
