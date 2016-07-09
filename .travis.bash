#!/bin/bash
set -ev

cd "${TRAVIS_BUILD_DIR}"

/opt/qt56/bin/qt56-env.sh

if [ $DEBUG_BUILD -eq 1 ]; then
	qmake rpi-car-system.pro -r -spec linux-g++ CONFIG+=debug CONFIG+=declarative_debug CONFIG+=qml_debug
else
	qmake rpi-car-system.pro -r -spec linux-g++
fi

make
