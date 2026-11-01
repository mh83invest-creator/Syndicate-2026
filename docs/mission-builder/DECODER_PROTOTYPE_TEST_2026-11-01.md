# Mission Builder — RNC1 Decoder Prototype Test (2026-11-01)
Branch: feature/mission-builder-audit
Status: FAILED FULL-SUITE GATE; DECODER NOT ACCEPTED FOR INTEGRATION.

## Test environment
Local Python implementation of RNC1 Method 1 based on engine/bflibrary/src/general/rnc_1fm.c. Input: user-uploaded Syndicate_2026_v0.8_Camera_Speed_Prototype_Test(3).zip. No original game data committed, extracted or written to source tree.
Prototype enforces 18-byte signature/header, maximum 116010-byte output, compressed payload length, packed CRC, per-literal and back-reference bounds, and final expanded CRC.

## Results
- 60 mission files present.
- 32 files decoded to exactly 116010 bytes and passed expanded CRC validation.
- 28 failed the prototype's input/literal boundary checks. These are decoder compatibility failures, NOT proof the retail data is corrupt.
- The prototype is not approved for GitHub source integration and was deliberately not committed as a functioning decoder.
- Prior header-only audit (60/60 lengths and packed CRCs) stands independently and is not evidence all 60 can be decoded by this prototype.
- No Windows executable, retail resources, save files or gameplay were changed/tested.

## Immediate next actions
Reconcile Python bitstream refill and literal transition against rnc_1fm.c, especially end-of-stream lookahead, then rerun all 60 files. Require 60/60 exact-length and expanded CRC results and targeted malformed-input negatives before committing a parser or interpreting objective fields. Preserve the protected v0.7 baseline and all existing campaigns.

## Status
R01 authentic-file availability and compressed metadata: confirmed.
R02 bounded decoder: PARTIAL LOCAL PROTOTYPE, GATE FAILED (32/60).
R03 decoded entity/objective inspector: NOT IMPLEMENTED.
R04 full negative test suite: NOT IMPLEMENTED.
