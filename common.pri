#############################################################################
##
## This file is part of RPI Car System
## Copyright (c) 2015 Fabien Caylus <toutjuste13@gmail.com>
##
## This file is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This file is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##
#############################################################################

include(version.pri)

CONFIG += c++11

# Add defines to compiler
DEFINES += \
    "APPLICATION_TARGET=\\\"$${APPLICATION_TARGET}\\\"" \
    "APPLICATION_NAME=\\\"$${APPLICATION_NAME}\\\""


RPI_INSTALL_PATH = /opt/rpi-car-system
CONFIG(READY_FOR_CARSYSTEM) {
    message("Build for the RPI")
    DEFINES += READY_FOR_CARSYSTEM
    target.path = $${RPI_INSTALL_PATH}
    INSTALLS += target
}

TARGET = $$quote($${APPLICATION_TARGET})

#
# Build config
#

BUILD_PATH = $${PWD}/build/$${APPLICATION_TARGET}
BIN_PATH = $${BUILD_PATH}/../bin
BUILD_STR = debug

CONFIG(debug, debug|release) {
    BUILD_STR = debug
    DEFINES += CORE_DEBUG
}
else {
    # Release
    BUILD_STR = release
    DEFINES += CORE_RELEASE
}

OBJECTS_DIR = $${BUILD_PATH}/$${BUILD_STR}/obj
UI_DIR = $${BUILD_PATH}/$${BUILD_STR}/ui
MOC_DIR = $${BUILD_PATH}/$${BUILD_STR}/moc
RCC_DIR = $${BUILD_PATH}/$${BUILD_STR}/rcc

DESTDIR = $${BIN_PATH}/$${BUILD_STR}
