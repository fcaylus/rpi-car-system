#############################################################################
##
## This file is part of RPI Car System
## Copyright (c) 2016 Fabien Caylus <tranqyll.dev@gmail.com>
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

# Create version file in build dir
QMAKE_POST_LINK = echo "$${APPLICATION_VERSION}" > "$${DESTDIR}/VERSION";

#
# Thirdparty stuff
#

# Workaround for installation where an another Qt distro is installed in /usr
# Disable this trick when cross compiling
!CONFIG(READY_FOR_CARSYSTEM) {
    QTDIR = $$getenv(QTDIR)
    !isEmpty(QTDIR) {
        LIBS = -L$$QTDIR/lib $$LIBS
    }
}

# PugiXml
include(../thirdparty/pugixml.pri)

# VLCQt
include(../thirdparty/vlc-qt.pri)

# libudev
CONFIG(READY_FOR_CARSYSTEM) {
    # Cross compilation
    LIBS += -ludev
} else {
    INCLUDEPATH += /usr/include
    LIBS += -L/usr/lib -ludev
}

# MediaInfoLib
include(../thirdparty/mediainfolib.pri)

#
# Project sources
#

SOURCES += \
    src/main.cpp \
    src/tshandler.cpp \
    src/mediamanager/mediamanager.cpp \
    src/mediamanager/playlist.cpp \
    src/mediamanager/usbsource.cpp \
    src/mediamanager/mediainfo.cpp \
    src/musicplayer.cpp \
    src/mediamanager/musiclistmodel.cpp \
    src/mediamanager/metadatalistmodel.cpp \
    src/musicqueuelistmodel.cpp \
    src/mediamanager/playlistlistmodel.cpp \
    src/sysinfomanager.cpp \
    src/controls/basicbutton.cpp \
    src/controls/abstractprogressbar.cpp \
    src/controls/abstractstackview.cpp

HEADERS += \
    src/dirutility.h \
    src/passwordmanager.h \
    src/common.h \
    src/languagemanager.h \
    src/sysinfomanager.h \
    src/updatemanager.h \
    src/tshandler.h \
    src/mediamanager/mediamanager.h \
    src/mediamanager/playlist.h \
    src/mediamanager/mediasource.h \
    src/mediamanager/usbsource.h \
    src/mediamanager/mediainfo.h \
    src/musicplayer.h \
    src/mediamanager/musiclistmodel.h \
    src/mediamanager/metadatalistmodel.h \
    src/musicqueuelistmodel.h \
    src/mediamanager/playlistlistmodel.h \
    src/controls/basicbutton.h \
    src/controls/abstractprogressbar.h \
    src/controls/abstractstackview.h

RESOURCES += \
    res.qrc

OTHER_FILES += \
    qml/qmldir \
    res.qrc

TRANSLATIONS += \
    i18n/$${APPLICATION_TARGET}_fr.ts

# For translations
lupdate_only {
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
