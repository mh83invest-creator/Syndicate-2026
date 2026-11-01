# Syndicate 2026 — Resource Packaging and Mission Number Ranges
Date: 2026-11-01 (Australia/Brisbane)
Status: static repository review only; no builds, gameplay tests, engine edits or save changes
Related: docs/audits/2026-11-01-american-revolt-launch-resource-audit.md; docs/CAMPAIGN_SAVE_INTEGRATION_CONTRACT.md; docs/SHARED_DEVELOPMENT_HANDOVER.md

## Files inspected
- engine/util/install: copy_data() and install_file() functions
- engine/README.md: startup and installation
- engine/conf/Makefile.am; engine/src/Makefile.am
- Previously inspected engine/src/main.c and engine/src/syndre.sx mission resource path logic

## Confirmed observations
RES-01: Installer explicitly requests miss01.dat through miss50.dat and four more groups of miss101–150, miss201–250, miss301–350 and miss401–450: 250 explicit mission-text resource filenames total. These are resource names, not proof of 250 playable missions or five campaigns.
RES-02: The same installer copies selected gameXX.dat resource files (01–50 and 90–99) and a noncontiguous list of mapXX.dat files. Do not equate briefing file index with tactical map number or campaign ID.
RES-03: Installer places these game data files under one target data directory. No separate American Revolt data destination or SAVE2 handling is apparent in copy_data(). This does not prove that installed files lack expansion content or that the engine cannot switch campaigns.
RES-04: The documented ordinary Windows launch uses syndicatfx.exe with language selection; no American Revolt-specific switch or separate executable was found in the inspected launch documentation. Selection through other mechanisms remains unresolved.
RES-05: engine/src/Makefile.am builds one syndicatfx target from assembly + C, and engine/conf/Makefile.am declares platform-specific wrapper mappings. No additional American Revolt-specific executable target was identified in those build files.
RES-06: Earlier full assembly inspection found load_text choosing data/miss%02.2d.dat or data/miss%01d%02.2d.dat depending on language. The installer resource groups may be language variants; DO NOT attribute groups 101–450 to American Revolt without verifying the branch and real assets.

## Required next checks
1. Reopen assembly load_text exact conditional and confirm 101–450 group selection by language, preventing false inference of campaign content.
2. Trace gameXX.dat and mapXX.dat loading, mode and mission IDs, and any expansion-specific resources.
3. Check the install data manifest against retail Syndicate Plus files (only legally available data) and American Revolt entry behaviour on Windows.
4. Define and test save isolation for Original, American Revolt, World Domination and playable Custom Campaigns, preserving old saves and the known-good v0.7 baseline.
5. Update the Mission Builder team on confirmed resource numbering and which assumptions remain unsupported.

## Verification
Repository source files read using personal GitHub connector; no compile, execution, asset validation, file migration or gameplay testing. Documentation-only handover; source/Windows workflows untouched.
