#######################################
# Project definition for Launcher App #
#######################################

TEMPLATE = app

QT += core gui qml quick
CONFIG += qtquickcompiler

# Default defines
APPLICATION_TARGET = launcher
APPLICATION_NAME = Launcher

DEFINES += LAUNCHER_APP

include(../common.pri)

#
# Thirdparty stuff
#

# PugiXml
include(../thirdparty/pugixml.pri)

# LibVLC
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib -lVLCQtCore

#
# Project sources
#

SOURCES += \
    src/main.cpp \
    src/soundmanager.cpp \
    src/mediainfo.cpp

HEADERS += \
    src/dirutility.h \
    src/soundmanager.h \
    src/mediainfo.h \
    src/passwordmanager.h \
    src/common.h

RESOURCES += \
    res.qrc

OTHER_FILES += \
    qml/qmldir \
    res.qrc

