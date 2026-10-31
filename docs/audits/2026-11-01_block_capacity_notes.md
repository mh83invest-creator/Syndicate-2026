# Block capacity notes

Source review of engine/src/syndre.sx shows two unrolled collector paths, each with 36 distinct append sites matching a 36-entry pointer array. This is static evidence only, not a runtime test. Protected main unchanged.

Source details: array data_48ad0 is at syndre.sx lines 71587–71623. First collector func_48b60 at 71628–72422 has 36 store-and-increment sites. Second collector func_4974e at 72429–73223 has 36. At lines 71338, 71354, 71454 and 71467, func_48a3a resets ECX to zero before the collector calls. Those paths contain no backward branches through the append chain. The array is therefore appropriately sized for those examined paths, but alternate callers and runtime behavior are unverified.

Verification: original assembly read, append-site count completed. Not run: build, native instrumentation, Windows gameplay, alternate mode calls, lighting, performance tests. No game source or commercial assets changed.
