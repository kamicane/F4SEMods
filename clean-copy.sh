#!/usr/bin/env bash
set -euo pipefail

SRC_DIR="${1:-}"
DST_DIR="${2:-}"

if [[ -z "$SRC_DIR" || -z "$DST_DIR" ]]; then
	echo "Usage: $0 SRC_DIR DST_DIR" >&2
	exit 2
fi

if [[ ! -d "$SRC_DIR" ]]; then
	echo "Source directory '$SRC_DIR' does not exist. Nothing to copy." >&2
	exit 0
fi

if [[ -d "$DST_DIR" ]]; then
	echo "Removing existing '$DST_DIR'..."
	rm -rf -- "$DST_DIR"
fi

echo "Creating destination directory '$DST_DIR'..."
mkdir -p -- "$DST_DIR"

echo "Copying '$SRC_DIR' to '$DST_DIR'..."
# copy contents preserving attributes; trailing /.
cp -a -- "$SRC_DIR/." "$DST_DIR/"

echo "Done."
