#!/bin/bash

VERSION="0.0.1"
PATH_ROOT=$(printf "%s" ~/Library/Arduino15/packages/LinkIt/tools/flash_tool/*/upload.py)
DEV_ROOT="/dev/cu.SLAB_USBtoUART"
DEV_FILENAME=$(basename "$DEV_ROOT")
DEV_PATH=$(dirname "$DEV_ROOT")

echo ""
echo "HITCON Badge Uploader - Version $VERSION"
echo ""

if [ ! -f "$PATH_ROOT" ]; then
    echo " [ERROR] path \"$PATH_ROOT\" does not exist. Did you install LinkIt 7697 library correctly?"
    exit
fi

if ! ls "$DEV_PATH" | grep "$DEV_FILENAME" > /dev/null 2>&1; then
    echo " [ERROR] path \"$DEV_ROOT\" does not exist. Did you install LinkIt 7697 driver and have it plugged-in correctly?"
    exit
fi

if [ ! -f "$1" ]; then
    echo " usage: $(basename "$0") your_binary.bin"
    exit
fi

"$PATH_ROOT" -n da97.bin -c "$DEV_ROOT" -f "$1"
