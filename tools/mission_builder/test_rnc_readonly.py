"""Regression checks for the read-only Mission Builder RNC decoder.

Usage: python tools/mission_builder/test_rnc_readonly.py path/to/prototype.zip
"""
import argparse
from pathlib import PurePosixPath
from zipfile import ZipFile
from rnc_readonly import decode, FormatError, inspect_archive

def run(path):
    with ZipFile(path) as archive:
        matches=[name for name in archive.namelist() if PurePosixPath(name).name.lower()=="game01.dat"]
        if len(matches)!=1:
            raise AssertionError("expected exactly one game01.dat fixture")
        valid=archive.read(matches[0])
    scenarios={
        "short header":valid[:7],
        "bad signature":b"XXXX"+valid[4:],
        "truncated payload":valid[:-1],
        "payload mutation":valid[:30]+bytes([valid[30]^1])+valid[31:],
        "oversized output":valid[:4]+(116011).to_bytes(4,"big")+valid[8:],
        "zero blocks":valid[:17]+b"\0"+valid[18:],
    }
    for name, payload in scenarios.items():
        try:
            decode(payload)
        except FormatError:
            print("PASS rejected",name)
        else:
            raise AssertionError("unexpected acceptance: "+name)
    rows=inspect_archive(path)
    assert len(rows)==60, f"expected 60 mission fixtures, found {len(rows)}"
    assert all(row["ok"] and row["expanded"]==116010 for row in rows), [row for row in rows if not row["ok"]]
    print("PASS all 60 missions decompressed and verified expanded CRC")

if __name__=="__main__":
    parser=argparse.ArgumentParser()
    parser.add_argument("archive")
    run(parser.parse_args().archive)
