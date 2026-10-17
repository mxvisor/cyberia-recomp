#!/usr/bin/env bash
# Run the Windows build of the port (cyberia) under Wine.
# Options, environment and the layout of the data: SR-SDK/tools/run_common.sh (--help).
set -euo pipefail
GAME_REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$GAME_REPO/SR-SDK/tools/run_common.sh"

PROG=cyberia
ENVP=CYB
DATA_DEFAULT="$GAME_REPO/build/disc"     # make disc: the image with game/portable.cfg over it
DATA_MARKER=CYB.EXE                    # a file the data directory must hold
CD_DEFAULT="$GAME_REPO/../iso/CD/CYBERIA_RUS.cue"
WINE_NAME=cyberia-wine

run_wine "$@"
