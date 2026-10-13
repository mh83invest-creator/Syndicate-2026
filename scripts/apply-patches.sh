#!/usr/bin/env bash
set -euo pipefail
engine="${1:?Supply the upstream checkout path}"
patch_dir="${2:?Supply patch directory}"
if [[ ! -d "$engine/.git" ]]; then echo "Not a git source checkout: $engine" >&2; exit 2; fi
if [[ ! -d "$patch_dir" ]]; then echo "Missing patch directory: $patch_dir" >&2; exit 2; fi
shopt -s nullglob
patches=("$patch_dir"/*.patch)
if (( ${#patches[@]} == 0 )); then
  echo "No source patches registered — building unmodified upstream."
else
  for p in "${patches[@]}"; do
    echo "Checking patch: $p"
    git -C "$engine" apply --check "$p"
    git -C "$engine" apply "$p"
  done
  git -C "$engine" diff --stat
fi
