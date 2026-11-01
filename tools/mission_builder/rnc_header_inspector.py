"""Read-only RNC1 header and packed CRC inspector for a ZIP containing gameNN.dat files.

This does not decompress, modify, extract or write game resources.
"""
import argparse
import json
from pathlib import PurePosixPath
from zipfile import ZipFile


def rnc_crc16(payload):
    crc = 0
    for value in payload:
        crc ^= value
        for _ in range(8):
            crc = (crc >> 1) ^ (0xA001 if crc & 1 else 0)
    return crc


def inspect(data, filename):
    row = {"file": filename, "length": len(data), "valid": False}
    if len(data) < 18:
        row["error"] = "header shorter than 18 bytes"
        return row
    if data[:4] != b"RNC\x01":
        row["error"] = "not RNC method 1"
        return row
    unpacked = int.from_bytes(data[4:8], "big")
    packed = int.from_bytes(data[8:12], "big")
    expected_crc = int.from_bytes(data[14:16], "big")
    actual_crc = rnc_crc16(data[18:])
    row.update(uncompressed_bytes=unpacked, compressed_bytes=packed,
               packed_crc_expected=expected_crc, packed_crc_actual=actual_crc,
               block_count=data[17])
    row["valid"] = (len(data) == 18 + packed and expected_crc == actual_crc and data[17] != 0)
    if not row["valid"]:
        row["error"] = "packed length, packed CRC or block count mismatch"
    return row


def inspect_archive(path):
    with ZipFile(path) as archive:
        results = []
        for member in sorted(archive.namelist()):
            base = PurePosixPath(member).name.lower()
            if (len(base) == 10 and base.startswith("game")
                    and base[4:6].isdigit() and base.endswith(".dat")):
                results.append(inspect(archive.read(member), base))
        return results


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("archive")
    parser.add_argument("--json", action="store_true")
    args = parser.parse_args()
    results = inspect_archive(args.archive)
    if args.json:
        print(json.dumps(results, indent=2))
    else:
        passed = sum(item["valid"] for item in results)
        print(f"{passed}/{len(results)} RNC mission headers and packed CRCs valid")
        for item in results:
            if not item["valid"]:
                print(item)
    if not results or any(not item["valid"] for item in results):
        raise SystemExit(1)


if __name__ == "__main__":
    main()
