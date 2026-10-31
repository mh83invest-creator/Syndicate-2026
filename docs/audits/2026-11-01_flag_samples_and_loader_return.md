# Syndicate 2026 — Block flag sampling and mission loading

Date: 2026-11-01. Feature-branch source audit; main v0.7 protected. Original assembly SHA c35e22621a93d1e07c44d40de8867446ff7c64bf.

Source at syndre.sx lines 79740–79781 constructs six flag bits per block descriptor. Each bit indicates whether sixteen 32-bit samples, spaced 20 bytes apart, were all zero. This is not an exhaustive zero-byte test of an entire 320-byte graphics component. Earlier wording calling these 'empty component flags' should be read narrowly as *sampled-zero* flags. Their purpose is rendering, not lighting.

At assembly lines 27901–27913, the mission resource loader is called and its return value is not immediately checked before the optional sound-bank setup and init_hires_blocks. In gdata.c, LbDataLoadAll returns the count of failed files. The local code therefore has a conditional risk when game data fails to load; no such failure has been demonstrated during gameplay.

PASS: read original assembly and gdata.c control flow. NOT RUN: native build, loading-failure injection, actual flag-array comparison, gameplay, lighting, profiling. No game source, workflow or retail resources changed. Next investigate what values the flags take for actual private block data and confirm branch behavior.