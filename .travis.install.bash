#!/bin/bash
set -ev

. /opt/qt56/bin/qt56-env.sh

mkdir tempbuild
cd tempbuild
wget https://github.com/vlc-qt/vlc-qt/archive/1.0.1.tar.gz
tar xf 1.0.1.tar.gz
cd vlc-qt-1.0.1
cmake -DCMAKE_INSTALL_PREFIX=/usr .
make
sudo make install

