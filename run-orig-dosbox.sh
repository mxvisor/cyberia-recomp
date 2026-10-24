#!/usr/bin/env bash
# Run the ORIGINAL cyberia under DOSBox (the reference for the port).
# Options, environment and the layout of the data: SR-SDK/tools/run_orig_common.sh (--help).
set -euo pipefail
GAME_REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$GAME_REPO/SR-SDK/tools/run_orig_common.sh"

ENVP=CYB
DOS_CMD=CYB.EXE

run_orig_dosbox "$@"
