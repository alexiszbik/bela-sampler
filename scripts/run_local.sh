#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
HEADLESS_DIR="$REPO_ROOT/headless"
BUILD_DIR="$HEADLESS_DIR/build"
BINARY="$BUILD_DIR/sampler_headless"

MIDI_DEVICE="Sampler Virtual"
SAMPLES_FOLDER="$REPO_ROOT/samplesfolder"
PROGRAM_FOLDER="$REPO_ROOT/program"
EXTRA_ARGS=""

usage() {
	cat <<EOF
Usage: $(basename "$0") [options] [-- args passed to sampler_headless]

Build and run the headless sampler locally (RtAudio + RtMidi).
Run from the repo root so samplesfolder/ and program/ resolve.

Options:
  -m "device"   Virtual MIDI port name (default: "$MIDI_DEVICE")
  -s "folder"   Samples folder      (default: $SAMPLES_FOLDER)
  -p "folder"   Program folder      (default: $PROGRAM_FOLDER)
  -b            Build only, don't run
  -c            Clean build before building
  -h            Show this help

Examples:
  $(basename "$0")
  $(basename "$0") -m "UM-ONE" -c
  $(basename "$0") -- samplesfolder program "My Device"
EOF
}

BUILD_ONLY=0
CLEAN=0
while [ $# -gt 0 ]; do
	case "$1" in
		-m) shift; MIDI_DEVICE="$1" ;;
		-s) shift; SAMPLES_FOLDER="$1" ;;
		-p) shift; PROGRAM_FOLDER="$1" ;;
		-b) BUILD_ONLY=1 ;;
		-c) CLEAN=1 ;;
		-h|--help) usage; exit 0 ;;
		--) shift; EXTRA_ARGS="$*"; break ;;
		*)
			echo "Unknown option: $1" >&2
			usage >&2
			exit 1
			;;
	esac
	shift
done

echo "→ Configuring headless build..."
if [ "$CLEAN" = 1 ] && [ -d "$BUILD_DIR" ]; then
	echo "→ Cleaning previous build..."
	rm -rf "$BUILD_DIR"
fi

cmake -S "$HEADLESS_DIR" -B "$BUILD_DIR" -G "Unix Makefiles"

echo "→ Building..."
cmake --build "$BUILD_DIR" -j

if [ "$BUILD_ONLY" = 1 ]; then
	echo "✓ Build complete (binary: $BINARY)"
	exit 0
fi

[ -x "$BINARY" ] || { echo "Binary not found: $BINARY" >&2; exit 1; }

echo "→ Running headless sampler..."
echo "  samples: $SAMPLES_FOLDER"
echo "  program:  $PROGRAM_FOLDER"
echo "  midi:     $MIDI_DEVICE"
echo "  (Ctrl-C to quit, type 'reload' to rescan samples and reload programs)"
echo

cd "$REPO_ROOT"
exec "$BINARY" "$SAMPLES_FOLDER" "$PROGRAM_FOLDER" "$MIDI_DEVICE" $EXTRA_ARGS
