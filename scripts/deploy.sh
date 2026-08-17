#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

# shellcheck source=.bela_config
source "$SCRIPT_DIR/.bela_config"
[ -f "$HOME/.belarc" ] && source "$HOME/.belarc"

RUN=1
CLEAN=0
MAKE_ARGS=""
COMMAND_ARGS=""

usage() {
	cat <<EOF
Usage: $(basename "$0") [options]

  Sync this project to the Bela Gem, build and run it.

Options:
  -n          Build only, do not run
  --clean     Clean project objects on the board before building
  -c "args"   Pass command-line arguments to the Bela program
  -h          Show this help

Environment:
  BBB_HOSTNAME   Board address (default: bela.local)
  PROJECT_NAME   Remote project folder name (default: untitled)
EOF
}

while [ $# -gt 0 ]; do
	case "$1" in
		-n) RUN=0 ;;
		--clean) CLEAN=1 ;;
		-c)
			shift
			COMMAND_ARGS="$1"
			;;
		-h|--help)
			usage
			exit 0
			;;
		*)
			echo "Unknown option: $1" >&2
			usage >&2
			exit 1
			;;
	esac
	shift
done

REMOTE_PROJECT="$BBB_PROJECT_HOME/$PROJECT_NAME"

echo "→ Syncing to $BBB_ADDRESS:$REMOTE_PROJECT"
ssh "$BBB_ADDRESS" "mkdir -p '$REMOTE_PROJECT' && rm -rf '$REMOTE_PROJECT/external' '$REMOTE_PROJECT/vendor'"
rsync -ac --no-t --delete-after \
	--exclude='build/' \
	--exclude='.git/' \
	--exclude='scripts/' \
	--exclude='external/' \
	--exclude='juce/' \
	--exclude='*.md' \
	"$PROJECT_DIR/" "$BBB_ADDRESS:$REMOTE_PROJECT/"
# juce/ holds the desktop simulator (including juce/platform/); never deployed to Bela.
# platform/ at repo root is Bela-safe only (SamplerBootstrap, SamplerLog).

# Build from Bela root (PROJECT_DIR = projects/Sampler). Do not use make -C $REMOTE_PROJECT:
# Bela resolves PROJECT_DIR as abspath(projects/$(PROJECT)) relative to cwd and breaks in-tree.
SAMPLER_CPPFLAGS="-DSAMPLER_BELA=1 -I$REMOTE_PROJECT/platform -I$REMOTE_PROJECT/src/program -I$REMOTE_PROJECT/src/playback -I$REMOTE_PROJECT/src/playback/sample -I$REMOTE_PROJECT/src/engine -I$REMOTE_PROJECT/src/mix -I$REMOTE_PROJECT/src/dsp -I$REMOTE_PROJECT/src/dsp/delay -I$REMOTE_PROJECT/src/midi"
MAKE_BASE="make --no-print-directory QUIET=true -C '$BBB_BELA_HOME' PROJECT='$PROJECT_NAME' CPPFLAGS='$SAMPLER_CPPFLAGS'"
[ -n "$COMMAND_ARGS" ] && MAKE_BASE="$MAKE_BASE CL='$COMMAND_ARGS'"

run_make() {
	local target="$1"
	if [ -n "$target" ]; then
		ssh "$BBB_ADDRESS" "$MAKE_BASE $target"
	else
		ssh "$BBB_ADDRESS" "$MAKE_BASE"
	fi
}

if [ "$CLEAN" -eq 1 ]; then
	echo "→ Cleaning project on board..."
	run_make projectclean
fi

if [ "$RUN" -eq 0 ]; then
	echo "→ Building on board..."
	run_make
	echo "Done."
else
	echo "→ Building and running on board..."
	ssh -t "$BBB_ADDRESS" "$MAKE_BASE run"
fi
