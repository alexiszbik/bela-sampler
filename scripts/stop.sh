#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/.bela_config"
[ -f "$HOME/.belarc" ] && source "$HOME/.belarc"

echo "→ Stopping running Bela program on $BBB_ADDRESS..."
ssh "$BBB_ADDRESS" "make --no-print-directory QUIET=true -C $BBB_BELA_HOME stop"
