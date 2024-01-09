#!/bin/bash
#
# Author: Aleksander Rozhkov <aleksprog@hotmail.com>
#
# Weprex software creation script for Linux - Modbus client.

set -e

# Preparing the environment
mkdir -p ../build
mkdir -p ../usr/bin
mkdir -p ../usr/lib
mkdir -p ../usr/share/application
mkdir -p ../usr/share/icons/hicolor/256x256
echo "[Desktop Entry]
Type=Application
Name=Weprex
Comment=Modbus client
Exec=weprex
Icon=weprex
Categories=Office;" >  ../usr/share/application/weprex.desktop
cp ../screenshots/chart.png ../usr/share/icons/hicolor/256x256/weprex.png
docker build --tag qt-linux-cmake .

# Building WEPREX from source
docker run --rm -v "$(pwd)"/..:/src --device /dev/fuse --cap-add SYS_ADMIN --security-opt apparmor:unconfined qt-linux-cmake qmake . -o build
docker run --rm -v "$(pwd)"/..:/src --device /dev/fuse --cap-add SYS_ADMIN --security-opt apparmor:unconfined qt-linux-cmake make -C build -j

# Packaging an application in AppImage
cp ../build/weprex ../usr/bin
docker run --rm -v "$(pwd)"/..:/src --device /dev/fuse --cap-add SYS_ADMIN --security-opt apparmor:unconfined qt-linux-cmake /usr/bin/linuxdeployqt ./usr/share/application/weprex.desktop -appimage -always-overwrite -verbose=2

# Copying an application to the linux_build folder and cleaning it up
mv ../Weprex* . -f
rm ../build ../usr ../.DirIcon ../AppRun ../weprex.desktop ../weprex.png --force -r
