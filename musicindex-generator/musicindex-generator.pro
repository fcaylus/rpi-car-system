###############################################
# Project definition for musicindex-generator #
###############################################

TEMPLATE = app

QT += core
QT -= gui

# Default defines
APPLICATION_TARGET = musicindex-generator
APPLICATION_NAME = MusicIndexGenerator

include(../common.pri)

#
# Thirdparty stuff
#

# LibVLC
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib -lVLCQtCore

#
# Project sources
#

INCLUDEPATH += ../launcher/src/

SOURCES += \
    src/main.cpp \
    ../launcher/src/mediainfo.cpp

HEADERS += \
    ../launcher/src/mediainfo.h \
    ../launcher/src/common.h \
    ../launcher/src/dirutility.h
