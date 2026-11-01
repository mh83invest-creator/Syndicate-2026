# Main Systems — map loader audit handover

2026-11-01. MAIN audit; source-only. No playtesting during parallel Graphics and Mission Builder audits.

Previous map caller report: docs/audits/2026-11-01-main-map-loading-callers-data-sources.md (commit b8707d33d7070f73301cb35674230e7613eecaa5). The earlier companion handover was not committed. This document closes that gap and incorporates the new briefing-versus-tactical loader investigation.

New report: docs/audits/2026-11-01-main-briefing-versus-tactical-loader-contract.md (commit b0598fdce3150a1efe775d89369f0c899be634b4).

Verified: mission_briefing separately reads game and map data with explicit -1 error handling, then relocates the 0x3000 map pointer table; tactical load_map_level uses its own mode-dependent resource paths and calls init_level_data, which also relocates map data. These are different call sites; double relocation of the same buffer was NOT proven. process_action can trigger tactical level load and actor initialization. Preserve file, memory, input, palette, audio and network-slot contracts in future reconciliation.

Outstanding: map lifetime/reallocation, full binary format, malformed-file checks, end-to-end mission UI flow, runtime verification. v0.7, graphics branch and Mission Builder unchanged. No build or gameplay tests.
