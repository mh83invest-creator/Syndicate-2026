"""Read-only regression tests requiring user-owned mission ZIP."""
import argparse
from collections import Counter
from mission_inspector import inspect_archive, OBJECTIVE_COUNT, CP_COUNT

def run(archive):
    missions=inspect_archive(archive)
    assert len(missions)==60, len(missions)
    assert len(set(m['file'] for m in missions))==60
    assert all(m['decoded_bytes']==116010 for m in missions)
    assert all(len(m['objectives'])==OBJECTIVE_COUNT for m in missions)
    assert all(len(m['computer_player_objectives'])==CP_COUNT for m in missions)
    assert all(all(o['slot']==i for i,o in enumerate(m['objectives'])) for m in missions)
    assert all(all(c['slot']==i for i,c in enumerate(m['computer_player_objectives'])) for m in missions)
    first=next(m for m in missions if m['file']=='game01.dat')
    assert first['map_number']==1
    assert first['objectives'][0]['type']==2
    assert first['objectives'][0]['target_offset']==738
    counts=Counter(o['type'] for m in missions for o in m['objectives'])
    assert sum(counts.values())==480
    assert all(k<=16 for k in counts), counts
    print('PASS: 60/60 decoded, all 480 tactical slots and 7,680 CP slots decoded')
    print('Objective type frequencies:',dict(sorted(counts.items())))
    print('Distinct map numbers:',len(set(m['map_number'] for m in missions)))

if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('archive')
    run(parser.parse_args().archive)
