"""Read-only structural diagnostics for original Syndicate mission fixtures."""
import argparse
from collections import Counter
from pathlib import PurePosixPath
from zipfile import ZipFile
from mission_inspector import inspect_mission
from rnc_readonly import decode

THINGS_BASE = 0x8006
POOLS = {1:(2,23554,92,1), 2:(2,23554,92,1), 3:(2,23554,92,1),
         5:(38242,56674,36,4), 14:(23554,26242,42,2), 15:(23554,26242,42,2)}

def examine_mission(raw, filename):
    data = decode(raw)
    mission = inspect_mission(raw, filename)
    cp = mission['computer_player_objectives']
    row = {'file': filename, 'child_edges': 0, 'parent_edges': 0,
           'out_of_range_links': [], 'nonreciprocal_child_links': [],
           'cycles': [], 'target_mismatches': [], 'entity_type_zero': []}
    for slot, record in enumerate(cp):
        for field in ('child', 'parent'):
            dest = record[field]
            if not dest: continue
            row[field+'_edges'] += 1
            if dest < 1 or dest >= len(cp):
                row['out_of_range_links'].append((slot, field, dest))
        child = record['child']
        if 1 <= child < len(cp) and cp[child]['parent'] != slot:
            row['nonreciprocal_child_links'].append((slot, child, cp[child]['parent']))
    seen_cycles = set()
    for slot in range(1, len(cp)):
        visited, path, current = {}, [], slot
        while 1 <= current < len(cp):
            if current in visited:
                cycle = tuple(path[visited[current]:])
                if frozenset(cycle) not in seen_cycles:
                    seen_cycles.add(frozenset(cycle))
                    row['cycles'].append(cycle)
                break
            visited[current] = len(path)
            path.append(current)
            current = cp[current]['child']
    for obj in mission['objectives']:
        kind, offset = obj['type'], obj['target_offset']
        if kind not in POOLS or not offset: continue
        start, stop, stride, expected_type = POOLS[kind]
        if not (start <= offset < stop and (offset-start)%stride == 0):
            row['target_mismatches'].append((obj['slot'],kind,offset,'unaligned'))
            continue
        actual = data[THINGS_BASE+offset+0x18]
        if actual == 0:
            row['entity_type_zero'].append((obj['slot'],kind,offset))
        elif actual != expected_type:
            row['target_mismatches'].append((obj['slot'],kind,offset,actual))
    return row

def examine_archive(path):
    import re
    with ZipFile(path) as archive:
        names = sorted(n for n in archive.namelist()
                       if re.fullmatch(r'game\d\d\.dat',PurePosixPath(n).name.lower()))
        return [examine_mission(archive.read(n),PurePosixPath(n).name.lower()) for n in names]

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('archive')
    rows=examine_archive(parser.parse_args().archive)
    counts=Counter()
    for row in rows:
        for field in ('child_edges','parent_edges'):
            counts[field]+=row[field]
        for field in ('out_of_range_links','nonreciprocal_child_links','cycles',
                      'target_mismatches','entity_type_zero'):
            counts[field]+=len(row[field])
    print('missions',len(rows), 'results',dict(sorted(counts.items())))
    for row in rows:
        for field in ('nonreciprocal_child_links','entity_type_zero',
                      'out_of_range_links','cycles','target_mismatches'):
            if row[field]: print(row['file'],field,row[field])
    assert len(rows)==60
    for field,expected in {'child_edges':123,'parent_edges':120,
                           'nonreciprocal_child_links':2,'entity_type_zero':1,
                           'out_of_range_links':0,'cycles':0,'target_mismatches':0}.items():
        assert counts[field]==expected,(field,counts[field],expected)
    print('PASS original fixtures; anomalies retained as diagnostics, not auto-repaired')

if __name__=='__main__':main()
