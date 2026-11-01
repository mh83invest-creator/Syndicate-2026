# MB-AUDIT-5.6 — Legacy read-count and destination capacity
Date: 2026-11-01. Branch: feature/mission-builder-audit.
Source: engine/src/syndre.sx blob c35e22621a93d1e07c44d40de8867446ff7c64bf.

## Source observations
- LbFileRead_ at lines 14884–14896 forwards three arguments to ac_read and returns its result.
- LbFileReadRNC at 14916–14960 calls LbFileLengthRnc, sends that value as LbFileRead_ requested count, then invokes UnpackM1 without comparing the read return count to requested count.
- LbFileReadRNC receives a filename and destination pointer but no explicit destination capacity. Its eventual return value is the retained file-handle-related value, not a validated decoded byte count.
- load_map_level at lines 27873–27885 passes level__Seed as destination, then uses level__MapNumber without immediately checking the LbFileReadRNC result. The other draw mode invokes the same loader at 27917–27924.
- clear_savegame at 27627–27636 zeros the existing level region [level__Seed, level__End), previously calculated as 116010 bytes. Clearing does not constitute a check that a future decoded resource fits in that region.
- This is static evidence of absent checks in the inspected path; not a reproduced runtime crash or proof of behaviour of all loader variants.

## Required future verification
Compare actual legal retail mission RNC compressed/uncompressed sizes against the allocated level region. Inspect low-level ac_read behaviour for short reads. Validate the link between file data, objective arrays and entity offsets before specifying a writer. Use an isolated fixture and controlled Windows test before any integration.

## Protection and test status
Documentation only; no engine code, assets, saves, build or gameplay changes. Preserve classic campaigns, v0.7, fixed 16 FPS and save isolation.