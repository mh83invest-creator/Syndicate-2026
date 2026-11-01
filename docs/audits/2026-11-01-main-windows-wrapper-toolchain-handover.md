# MAIN handover — MinGW wrapper toolchain / profiler constraints

2026-11-01. MAIN audit. Companion to the source-based wrapper and effects audits.

Audit `docs/audits/2026-11-01-main-windows-wrapper-toolchain-profiling-gates.md` committed at `d5533200fffbfc17fa4288e9546fdd89f17c4ef0`.

Verified from `engine/configure.ac`: MinGW selects Windows conditional with initial 4-byte stack alignment, Windows symbol mapping and no Darwin-only assembler filter; `-g` gets removed from assembler flags for large `syndre.sx`. `engine/src/Makefile.am` generates wrappers through `mkwrappers` then compiles assembly. Confirmed existing game-update and display C bridge declarations, but did not verify generated binary ABI nor claim direct instrumentation is safe.

Next: read Windows symbol mapping and actual build invocation; map whether the build can produce an isolated diagnostic artifact without changing release defaults. No code edits, compilation, profiling or gameplay testing. Protected v0.7 and Graphics/Mission Builder untouched.
