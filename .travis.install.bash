#!/bin/bash
set -ev

export QTDIR="/opt/qt55"
export PATH="$QTDIR/bin:$PATH"
export LD_LIBRARY_PATH="$QTDIR/lib/x86_64-linux-gnu:$QTDIR/lib:$LD_LIBRARY_PATH"
export PKG_CONFIG_PATH="$QTDIR/lib/pkgconfig:$PKG_CONFIG_PATH"

mkdir tempbuild
cd tempbuild
wget https://github.com/vlc-qt/vlc-qt/archive/1.0.1.tar.gz
tar xf 1.0.1.tar.gz
cd vlc-qt-1.0.1
cmake -DCMAKE_INSTALL_PREFIX=/usr .
make
make install

