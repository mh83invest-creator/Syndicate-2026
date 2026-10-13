# Syndicate 2026 source patches

Numbered `*.patch` files are applied in lexical order against **SyndicatFX 0.0.6.1022**.

Each source patch requires a clear explanation of the problem, exact code changes, and a regression test in `docs/TEST_MATRIX.md`. Do not add game binaries or retail assets.

The Windows i686 GitHub Actions workflow now checks out this project's branch alongside pinned upstream, runs `scripts/apply-patches.sh`, then compiles and uploads the engine artifact.

**Current state:** No custom engine patches have yet been proven. If no patch exists, CI explicitly builds **unmodified upstream**. Never label this the fixed v0.6 implementation.
