#!/usr/bin/env bash

# Run this script on a macOS x86_64 host to generate Qt x86_64 binaries.

set -e
set -x

# Download, verify, and extract sources.
curl -OL https://download.qt.io/archive/qt/5.9/5.9.9/single/qt-everywhere-opensource-src-5.9.9.tar.xz
shasum -a 256 qt-everywhere-opensource-src-5.9.9.tar.xz | grep -q 5ce285209290a157d7f42ec8eb22bf3f1d76f2e03a95fc0b99b553391be01642
tar xjf qt-everywhere-opensource-src-5.9.9.tar.xz
patch -p0 < "${BASH_SOURCE%/*}/qt-5.9.9.patch"

# Build the x86_64 variant.
mkdir qt-5.9.9-x86_64
cd qt-5.9.9-x86_64
../qt-everywhere-opensource-src-5.9.9/configure \
  --prefix=/ \
  -platform macx-clang \
  -device-option QMAKE_APPLE_DEVICE_ARCHS=x86_64 \
  -device-option QMAKE_MACOSX_DEPLOYMENT_TARGET=10.10 \
  -release \
  -opensource -confirm-license \
  -gui \
  -widgets \
  -no-gif \
  -no-icu \
  -no-pch \
  -no-angle \
  -no-opengl \
  -no-dbus \
  -no-harfbuzz \
  -skip declarative \
  -skip multimedia \
  -skip qtcanvas3d \
  -skip qtcharts \
  -skip qtconnectivity \
  -skip qtdeclarative \
  -skip qtgamepad \
  -skip qtlocation \
  -skip qtmultimedia \
  -skip qtnetworkauth \
  -skip qtpurchasing \
  -skip qtremoteobjects \
  -skip qtscript \
  -skip qtsensors \
  -skip qtserialbus \
  -skip qtserialport \
  -skip qtsvg \
  -skip qtwebchannel \
  -skip qtwebengine \
  -skip qtwebsockets \
  -skip qtxmlpatterns \
  -nomake examples \
  -nomake tests \
  -nomake tools
make -j 8
make install -j 8 INSTALL_ROOT=/tmp/qt-5.9.9-macosx10.10-x86_64
cd ..

# Create the final tarball containing x86_64 binaries.
tar cjf qt-5.9.9-macosx10.10-x86_64.tar.xz -C /tmp qt-5.9.9-macosx10.10-x86_64
