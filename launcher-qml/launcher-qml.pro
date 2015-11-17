#######################################
# Project definition for Launcher App #
#######################################

TEMPLATE = app

QT += core qml quick
CONFIG += qtquickcompiler c++11

# Default defines
APPLICATION_TARGET = launcher-qml
APPLICATION_NAME = Launcher

# Add defines to compiler
DEFINES += "APPLICATION_TARGET=\\\"$${APPLICATION_TARGET}\\\"" \
    "APPLICATION_NAME=\\\"$${APPLICATION_NAME}\\\""

TARGET = $$quote($${APPLICATION_TARGET})

#
# Build config
#

BUILD_PATH = build
BIN_PATH = build/bin
BUILD_STR = debug

CONFIG(debug, debug|release) {
    BUILD_STR = debug
    DEFINES += CORE_DEBUG
    TARGET = $$join(TARGET,,,d)
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

INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib -lVLCQtCore

#
# Thirdparty sources
#

# PugiXML
SOURCES += thirdparty/pugixml-1.7/src/pugixml.cpp
HEADERS += \
    thirdparty/pugixml-1.7/src/pugixml.hpp \
    thirdparty/pugixml-1.7/src/pugiconfig.hpp
INCLUDEPATH += thirdparty/pugixml-1.7/src

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
    src/mediainfo.h

RESOURCES += \
    res.qrc

OTHER_FILES += \
    qml/qmldir \
    res.qrc

