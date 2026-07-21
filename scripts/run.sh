#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/.bela_config"
[ -f "$HOME/.belarc" ] && source "$HOME/.belarc"

COMMAND_ARGS=""

usage() {
	cat <<EOF
Usage: $(basename "$0") [-c "args"]

  Run the project already deployed on the board.

Options:
  -c "args"   Pass command-line arguments to the Bela program
  -h          Show this help
EOF
}

while [ $# -gt 0 ]; do
	case "$1" in
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

MAKE_CMD="make --no-print-directory QUIET=true -C $BBB_BELA_HOME PROJECT='$PROJECT_NAME'"
[ -n "$COMMAND_ARGS" ] && MAKE_CMD="$MAKE_CMD CL='$COMMAND_ARGS'"

echo "→ Running $PROJECT_NAME on $BBB_ADDRESS..."
ssh -t "$BBB_ADDRESS" "$MAKE_CMD run"
