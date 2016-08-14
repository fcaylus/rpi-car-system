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
system(mkdir -p "$${DESTDIR}"; echo "$${APPLICATION_VERSION}" > "$${DESTDIR}/VERSION")

# Copy licenses files to build dir
system(mkdir -p "$${DESTDIR}/licenses/files/pugixml-1.7")
system(cp "$$_PRO_FILE_PWD_/../thirdparty/pugixml/LICENSE.txt" "$${DESTDIR}/licenses/files/pugixml-1.7")
system(mkdir -p "$${DESTDIR}/licenses/files/MiniFlatFlags")
system(cp "$$_PRO_FILE_PWD_/thirdparty/MiniFlatFlags/LICENSE.txt" "$${DESTDIR}/licenses/files/MiniFlatFlags")
system(mkdir -p "$${DESTDIR}/licenses/files/OpenSansFont")
system(cp "$$_PRO_FILE_PWD_/fonts/LICENSE-OpenSans.txt" "$${DESTDIR}/licenses/files/OpenSansFont")
system(mkdir -p "$${DESTDIR}/licenses/files/MaterialIcons")
system(cp "$$_PRO_FILE_PWD_/imgs/LICENSE-MaterialIcons.txt" "$${DESTDIR}/licenses/files/MaterialIcons")

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
    src/core/sysinfomanager.cpp \
    src/core/mediamanager/mediamanager.cpp \
    src/core/mediamanager/playlist.cpp \
    src/core/mediamanager/usbsource.cpp \
    src/core/mediamanager/mediainfo.cpp \
    src/gui/musicplayer.cpp \
    src/gui/tshandler.cpp \
    src/gui/models/musiclistmodel.cpp \
    src/gui/models/metadatalistmodel.cpp \
    src/gui/models/musicqueuelistmodel.cpp \
    src/gui/models/playlistlistmodel.cpp \
    src/gui/controls/button.cpp \
    src/gui/controls/stackview.cpp \
    src/gui/controls/progressbar.cpp \
    src/gui/models/keyboardlayoutmodel.cpp \
    src/gui/models/licenseslistmodel.cpp

HEADERS += \
    src/core/dirutility.h \
    src/core/passwordmanager.h \
    src/core/common.h \
    src/core/languagemanager.h \
    src/core/sysinfomanager.h \
    src/core/updatemanager.h \
    src/core/mediamanager/mediamanager.h \
    src/core/mediamanager/playlist.h \
    src/core/mediamanager/mediasource.h \
    src/core/mediamanager/usbsource.h \
    src/core/mediamanager/mediainfo.h \
    src/gui/tshandler.h \
    src/gui/musicplayer.h \
    src/gui/models/musiclistmodel.h \
    src/gui/models/metadatalistmodel.h \
    src/gui/models/musicqueuelistmodel.h \
    src/gui/models/playlistlistmodel.h \
    src/gui/controls/stackview.h \
    src/gui/controls/progressbar.h \
    src/gui/controls/button.h \
    src/gui/models/keyboardlayoutmodel.h \
    src/gui/models/licenseslistmodel.h

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

    message($${DESTDIR}/VERSION)

    install_version.path = $${RPI_INSTALL_PATH}
    install_version.files = $${DESTDIR}/VERSION

    install_qm.path = $${RPI_INSTALL_PATH}/translations
    install_qm.files = i18n/$${APPLICATION_TARGET}_fr.qm

    install_licenses.path = $${RPI_INSTALL_PATH}/licenses
    install_licenses.files = $${DESTDIR}/licenses/*

    INSTALLS += install_utils install_version install_qm install_licenses
}
