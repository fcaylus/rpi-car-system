#!/bin/bash
set -ev

cd "${TRAVIS_BUILD_DIR}"

export QTDIR="/opt/qt56"
export PATH="$QTDIR/bin:$PATH"
export LD_LIBRARY_PATH="$QTDIR/lib/x86_64-linux-gnu:$QTDIR/lib:$LD_LIBRARY_PATH"
export PKG_CONFIG_PATH="$QTDIR/lib/pkgconfig:$PKG_CONFIG_PATH"

if [ $DEBUG_BUILD -eq 1 ]; then
	qmake rpi-car-system.pro -r -spec linux-g++ CONFIG+=debug CONFIG+=declarative_debug CONFIG+=qml_debug
else
	qmake rpi-car-system.pro -r -spec linux-g++
fi

make
