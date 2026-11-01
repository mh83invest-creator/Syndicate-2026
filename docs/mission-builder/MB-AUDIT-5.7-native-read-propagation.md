# MB-AUDIT-5.7 — Native read result propagation
Date: 2026-11-01. Scope: documentation only.

## Source evidence
- engine/conf/wrappers_libc.conf maps read to a C-convention native function taking descriptor, pointer and count (read C i ipi).
- engine/src/syndre.sx lines 14884–14896: LbFileRead_ forwards the three parameters to ac_read and returns its value.
- engine/src/syndre.sx lines 14916–14960: LbFileReadRNC discards that read return value and calls UnpackM1 on the destination buffer.
- engine/src/dos.c defines a separate dos_low_level_read implementation which uses read and sets bytes_read to max(0,count). This is not the identified LbFileRead_ call route.
- Standard read semantics permit short positive results and zero on EOF, with negative return for error. The inspected assembly caller does not distinguish these cases.

## Limitations and future gates
No actual retail mission data or Windows execution examined. Do not conclude a particular game build fails or that any input triggers a memory error. Investigate exact platform read wrapper and runtime file errors with disposable fixtures before writing or accepting custom mission resources.

## Verification
Source-only audit. No changes to game code, assets, saves or baseline; no build or gameplay test. Protected v0.7, original campaigns and 16 FPS remain unchanged.