"""Read-only Syndicate mission objective inspector. No game asset writes."""
import argparse
import json
import re
from pathlib import PurePosixPath
from zipfile import ZipFile
from rnc_readonly import decode, FormatError

LEVEL_SIZE = 116010
OBJECTIVE_OFFSET = 0x1BD32
OBJECTIVE_COUNT = 8
OBJECTIVE_SIZE = 14
CP_OFFSET = 0x1BDAA
CP_COUNT = 128
CP_SIZE = 15
MAP_OFFSET = 0x1BD28

def u16(data, start):
    return int.from_bytes(data[start:start+2], 'little')

def inspect_mission(data, filename):
    unpacked = decode(data)
    if len(unpacked) != LEVEL_SIZE:
        raise FormatError('unexpected decoded level size')
    objectives = []
    for slot in range(OBJECTIVE_COUNT):
        offset = OBJECTIVE_OFFSET + slot * OBJECTIVE_SIZE
        objectives.append({
            'slot': slot,
            'status': int.from_bytes(unpacked[offset:offset+4], 'little'),
            'type': u16(unpacked, offset+4),
            'target_offset': u16(unpacked, offset+6),
            'x': u16(unpacked, offset+8),
            'y': u16(unpacked, offset+10),
            'z_or_parameter': u16(unpacked, offset+12),
        })
    cp = []
    for slot in range(CP_COUNT):
        offset = CP_OFFSET + slot * CP_SIZE
        cp.append({
            'slot': slot,
            'child': int.from_bytes(unpacked[offset:offset+2], 'little', signed=True),
            'parent': u16(unpacked, offset+2),
            'field_4': int.from_bytes(unpacked[offset+4:offset+6], 'little', signed=True),
            'flags': unpacked[offset+6],
            'action_type': unpacked[offset+7],
            'action': unpacked[offset+8],
            'target_offset': u16(unpacked, offset+9),
            'field_b': int.from_bytes(unpacked[offset+11:offset+13], 'little', signed=True),
            'field_d': int.from_bytes(unpacked[offset+13:offset+15], 'little', signed=True),
        })
    return {'file': filename, 'decoded_bytes': len(unpacked),
            'map_number': u16(unpacked, MAP_OFFSET), 'objectives': objectives,
            'computer_player_objectives': cp}

def inspect_archive(path):
    missions=[]
    with ZipFile(path) as archive:
        members = sorted(x for x in archive.namelist()
                         if re.fullmatch(r'game\d\d\.dat', PurePosixPath(x).name.lower()))
        for name in members:
            missions.append(inspect_mission(archive.read(name), PurePosixPath(name).name.lower()))
    return missions

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('archive', help='ZIP containing gameNN.dat files')
    parser.add_argument('--mission', help='Filter e.g. game01.dat')
    parser.add_argument('--all-cp', action='store_true', help='Include all 128 CP objective entries')
    parser.add_argument('--json', action='store_true', help='Output JSON')
    args=parser.parse_args()
    missions=inspect_archive(args.archive)
    if args.mission:
        missions=[m for m in missions if m['file']==args.mission.lower()]
        if not missions: parser.error('requested mission not found')
    if not args.all_cp:
        for m in missions:
            m['computer_player_objectives']=[x for x in m['computer_player_objectives']
                                             if any(v for k,v in x.items() if k!='slot')]
    if args.json:
        print(json.dumps(missions, indent=2))
    else:
        print(f'Validated {len(missions)} mission(s); decode length {LEVEL_SIZE:,} bytes')
        for m in missions:
            used=[o for o in m['objectives'] if o['type']]
            print(f"{m['file']}: map {m['map_number']}, {len(used)}/8 nonzero objective types, "
                  f"{len(m['computer_player_objectives'])}/128 nonzero CP records")
            for o in used:
                print('  slot {slot}: type={type} status={status} target={target_offset} '
                      'x={x} y={y} z/parameter={z_or_parameter}'.format(**o))

if __name__=='__main__':
    main()
