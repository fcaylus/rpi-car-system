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

#######################################
# Project definition for Launcher App #
#######################################

TEMPLATE = app

QT += core gui qml quick
CONFIG += qtquickcompiler

APPLICATION_TARGET = launcher
APPLICATION_NAME = Launcher

DEFINES += LAUNCHER_APP

include(../common.pri)

#
# Thirdparty stuff
#

# PugiXml
include(../thirdparty/pugixml.pri)

# TODO: will be used by update system
# Quazip
#LIBS += -lquazip

# VLCQt
include(../thirdparty/vlc-qt.pri)

#
# Project sources
#

SOURCES += \
    src/main.cpp \
    src/soundmanager.cpp \
    src/mediainfo.cpp \
    src/devicesmanager.cpp

HEADERS += \
    src/dirutility.h \
    src/soundmanager.h \
    src/mediainfo.h \
    src/passwordmanager.h \
    src/common.h \
    src/languagemanager.h \
    src/filereader.h \
    src/devicesmanager.h

RESOURCES += \
    res.qrc

OTHER_FILES += \
    qml/qmldir \
    res.qrc

TRANSLATIONS += \
    i18n/$${APPLICATION_TARGET}_fr.ts

# For translations
lupdate_only{
SOURCES += \
    qml/*.qml \
    qml/music/*.qml \
    qml/keyboard/*.qml \
    qml/settings/*.qml
}

#
# Install stuff
#

CONFIG(READY_FOR_CARSYSTEM) {
    # Generate translations
    system($${HOST_BIN_DIR}/lrelease -nounfinished -removeidentical i18n/$${APPLICATION_TARGET}_fr.ts)

    install_utils.path = $${RPI_INSTALL_PATH}
    install_utils.files = util/*
    install_qm.path = $${RPI_INSTALL_PATH}/translations
    install_qm.files = i18n/$${APPLICATION_TARGET}_fr.qm
    INSTALLS += install_utils install_qm
}
