# MAIN handover — Windows symbol mapping and verified CI build
Date: 2026-11-01. Documentation only on main.

Audit: `docs/audits/2026-11-01-main-windows-function-map-build-invocation.md`; source audit commit `06a29220b640ad4b751b3a759e2112c40481f784`.

Verified actual `engine/conf/fnmap_windows.conf` C-runtime symbol mapping, configuration's MinGW-specific wrapper rename flags, and actual `.github/workflows/windows-source-v0.7.yml` 32-bit MSYS2/MINGW32 pipeline. CI pins OpenAL Soft 1.24.3 and WildMIDI 0.4.5 source dependencies, checks `i686-w64-mingw32`, runs configure and `make -C release V=1` and exports build artifacts. The separate `restore-v0.7-source.yml` is a source restoration pipeline and must not be used for profiling experiments.

Next: inspect isolated branch and build trigger safety before designing a separate optional diagnostic workflow; preserve existing source, CI and protected v0.7. No code edits, Windows compile, build run, measurement, Graphics or Mission Builder modifications.
