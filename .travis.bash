#!/bin/bash
set -ev

if [ $NO_CROSS_COMPILE_DEBUG -eq 1 ]; then
	. /opt/qt55/bin/qt55-env.sh
	qmake rpi-car-system.pro -r -spec linux-g++ CONFIG+=debug CONFIG+=declarative_debug CONFIG+=qml_debug
	make
elif [ $NO_CROSS_COMPILE -eq 1 ]; then
	. /opt/qt55/bin/qt55-env.sh
	qmake rpi-car-system.pro -r -spec linux-g++
	make
elif [ $CROSS_COMPILE -eq 1 ]; then
	cd build-os
	./build-all.sh
fi
