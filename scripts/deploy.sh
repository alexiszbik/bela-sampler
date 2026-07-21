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
ssh "$BBB_ADDRESS" "mkdir -p '$REMOTE_PROJECT'"
rsync -ac --no-t --delete-after \
	--exclude='build/' \
	--exclude='.git/' \
	--exclude='scripts/' \
	--exclude='*.md' \
	"$PROJECT_DIR/" "$BBB_ADDRESS:$REMOTE_PROJECT/"

MAKE_TARGET=""
[ "$CLEAN" -eq 1 ] && MAKE_TARGET="projectclean"

MAKE_CMD="make --no-print-directory QUIET=true -C $BBB_BELA_HOME PROJECT='$PROJECT_NAME'"
[ -n "$MAKE_TARGET" ] && MAKE_CMD="$MAKE_CMD $MAKE_TARGET"
[ -n "$COMMAND_ARGS" ] && MAKE_CMD="$MAKE_CMD CL='$COMMAND_ARGS'"

if [ "$RUN" -eq 0 ]; then
	echo "→ Building on board..."
	ssh "$BBB_ADDRESS" "$MAKE_CMD" || exit 1
	echo "Done."
else
	echo "→ Building and running on board..."
	ssh -t "$BBB_ADDRESS" "$MAKE_CMD run" || exit 1
fi
