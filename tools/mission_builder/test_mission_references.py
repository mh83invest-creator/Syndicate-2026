"""Read-only regression test for source-established objective target pools."""
import argparse
from collections import Counter
from mission_inspector import inspect_archive

# Offsets relative to level__things_base (syndre.sx 85710-85736).
POOLS = {
    1: (2, 23554, 92, "people"),
    2: (2, 23554, 92, "people"),
    3: (2, 23554, 92, "people"),
    5: (38242, 56674, 36, "weapons"),
    14: (23554, 26242, 42, "vehicles"),
    15: (23554, 26242, 42, "vehicles"),
}

def validate(missions):
    counts = Counter()
    failures = []
    for mission in missions:
        for obj in mission['objectives']:
            kind = obj['type']
            if kind not in POOLS or not obj['target_offset']:
                continue
            start, end, stride, name = POOLS[kind]
            offset = obj['target_offset']
            if not (start <= offset < end and (offset-start) % stride == 0):
                failures.append((mission['file'], obj['slot'], kind, offset, name))
            counts[kind] += 1
    return counts, failures

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('archive')
    args = parser.parse_args()
    missions = inspect_archive(args.archive)
    counts, failures = validate(missions)
    print('Mission count:', len(missions))
    print('Checked by type:', dict(sorted(counts.items())))
    print('Total references:', sum(counts.values()))
    print('Unaligned/out-of-pool:', len(failures))
    if failures:
        for issue in failures:
            print('FAIL', issue)
        raise SystemExit(1)
    assert len(missions) == 60
    assert dict(counts) == {2:22, 1:25, 5:4, 14:8, 15:3, 3:5}, counts
    assert sum(counts.values()) == 67
    print('PASS: all 67 selected nonzero objective references align')
