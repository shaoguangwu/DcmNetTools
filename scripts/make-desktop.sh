#!/bin/bash

# echo "scriptPath1: "$(cd `dirname $0`; pwd)
# echo "scriptPath3: "$(dirname $(readlink -f $0))
# echo "scriptPath4: "$(cd $(dirname ${BASH_SOURCE:-$0});pwd)

SCRIPT_PATH=$(cd `dirname $0`; pwd)
DESKTOP_FILE=$SCRIPT_PATH/dcmnettools.desktop

# delete file and remake it.
rm -f "$DESKTOP_FILE" && touch "$DESKTOP_FILE"

echo "[Desktop Entry]" >> "$DESKTOP_FILE"
echo "Comment=DcmNetTools" >> "$DESKTOP_FILE"
echo "Exec=$SCRIPT_PATH/DcmNetTools" >> "$DESKTOP_FILE"
echo "GenericName=DcmNetTools" >> "$DESKTOP_FILE"
echo "Name=DcmNetTools" >> "$DESKTOP_FILE"
echo "StartupNotify=false" >> "$DESKTOP_FILE"
echo "Terminal=false" >> "$DESKTOP_FILE"
echo "Type=Application" >> "$DESKTOP_FILE"
echo "Icon=$SCRIPT_PATH/logo.png" >> "$DESKTOP_FILE"
