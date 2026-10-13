#!/usr/bin/env bash
# Reproducible upstream checkout for Syndicate 2026 source development.
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
destination="${1:-$root/.work/upstream}"
upstream="https://github.com/swfans/syndicatfx.git"
tag="0.0.6.1022"
if [[ -e "$destination" ]]; then
  echo "Refusing to overwrite existing directory: $destination" >&2
  exit 2
fi
mkdir -p "$(dirname "$destination")"
git clone --no-checkout "$upstream" "$destination"
git -C "$destination" checkout --detach "$tag"
git -C "$destination" rev-parse HEAD
echo "Source prepared at $destination"
if compgen -G "$root/patches/*.patch" >/dev/null; then
  for patch in "$root"/patches/*.patch; do
    echo "Applying $patch"
    git -C "$destination" apply --check "$patch"
    git -C "$destination" apply "$patch"
  done
fi
echo "Review git -C '$destination' status and diff before compiling."
