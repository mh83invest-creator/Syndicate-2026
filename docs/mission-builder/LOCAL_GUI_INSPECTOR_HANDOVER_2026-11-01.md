# Mission Builder — Local GUI browser milestone
Date: 2026-11-01. Branch: feature/mission-builder-audit.

## Implemented
- New read-only Tkinter GUI at tools/mission_builder/mission_inspector_gui.py.
- User selects a ZIP containing gameNN.dat files; existing rnc_readonly.py, mission_inspector.py and test_mission_entity_chains.py provide decode, objective data and diagnostics.
- Mission list, map number and decoded size; tactical objective table, nonzero computer-player objective table and diagnostics table. No editing, export or write operations. CP zeros are hidden for legibility only.
- Known original anomalies are diagnostics, NOT automatic invalidations.

## Verification
- A local GUI implementation's underlying load_archive function ran against the user-uploaded ZIP: 60 missions loaded, 8 tactical and 128 CP records per mission; 3 diagnostic occurrences across missions, in line with existing regression facts.
- The GitHub GUI implementation was committed; local and GitHub interface implementations differ in UI arrangement, though both use the same inspector and diagnostic modules. GitHub version was not independently downloaded and executed.
- No actual Windows GUI launch or mouse interaction was tested in this environment. Browser-like screenshot/design approval not claimed. No build, gameplay, original data write, or saves modified.

## Running
With Python and Tkinter installed, place the GUI alongside the existing three imported files and run:
python tools/mission_builder/mission_inspector_gui.py /path/to/your/private-game-data.zip
Or run without an argument and choose the ZIP in the file picker. Keep proprietary assets outside the public repository.

## Limitations
Read-only inspection only; type labels remain numeric. Diagnostic checks are partial and original file quirks are preserved. A separate Windows UI smoke test is needed. Protected v0.7, 16 FPS, original campaigns, graphics and save systems remain untouched.
