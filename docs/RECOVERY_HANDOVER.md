# Syndicate 2026 — recovery handover (2026-10-13)

## Confirmed baselines
- Last user-confirmed playable build: `Syndicate_2026_v0.6_Two_Menus_30FPS_Test.zip`.
- v0.6 archive SHA-256: `e1cc3fe89b68261c8e88f3c63d651ce8806235af25ec4dc76e94ae761fc5015f`.
- Candidate (NOT confirmed playable): `Syndicate_2026_v0.7_GitHub_Build.zip`.
- v0.7 archive SHA-256: `89445c8e55e4ad58def694c12ee63ac4dd36f36375d30c6b47c08b9546e4ca71`.
- Reference source: [swfans/syndicatfx](https://github.com/swfans/syndicatfx), tag `0.0.6.1022`.

## Repository facts
The existing GitHub Actions Windows-i686 workflow checks out **unmodified upstream** and builds it. This repository has not yet been shown to contain the modified C/C++ source corresponding to v0.6. Do NOT claim that current GitHub Actions reproduces v0.6.

## Previously user-tested observations
- Music and sound worked.
- Edge-scrolling was reported working in all four directions.
- Menu responsiveness had improved; user said the game remained fine.
- Intro playback remained problematic.
- Explosion-heavy slowdown is a priority, while retaining original gameplay timing.
- Mouse and keyboard only; controller support is excluded.

## Required next work
1. Recover any local patched source; otherwise check out the pinned upstream tag and reimplement fixes with source commits.
2. Preserve v0.6 ZIP and local installed retail data, but do not upload proprietary game files to this public repository.
3. Implement and review camera edge-scroll fix first, menu rendering timing second, and intro/explosion performance separately.
4. Build from committed source and test it against the known-good installation.
5. Carry this handover, changelog, test outcomes, exact source commit IDs and open issues into each future iteration.

## Release status
This is source-control recovery documentation only. It is **not** a v0.8 playable release, and no modified source or gameplay fix is claimed here.
