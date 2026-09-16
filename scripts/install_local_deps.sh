#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
HEADLESS_DIR="$REPO_ROOT/headless"
BUILD_DIR="$HEADLESS_DIR/build"

usage() {
	cat <<EOF
Usage: $(basename "$0") [options]

Configure the headless build and fetch third-party dependencies (RtAudio, RtMidi, dr_libs).
Requires network access on first run.

Options:
  -c    Clean build directory before configuring
  -h    Show this help

After this, use ./scripts/run_local.sh to build and run offline.
EOF
}

CLEAN=0
while [ $# -gt 0 ]; do
	case "$1" in
		-c) CLEAN=1 ;;
		-h|--help) usage; exit 0 ;;
		*)
			echo "Unknown option: $1" >&2
			usage >&2
			exit 1
			;;
	esac
	shift
done

if [ "$CLEAN" = 1 ] && [ -d "$BUILD_DIR" ]; then
	echo "→ Cleaning build directory..."
	rm -rf "$BUILD_DIR"
fi

echo "→ Configuring headless build (fetching dependencies if needed)..."
cmake -S "$HEADLESS_DIR" -B "$BUILD_DIR" -G "Unix Makefiles"

echo "✓ Dependencies ready (build dir: $BUILD_DIR)"
