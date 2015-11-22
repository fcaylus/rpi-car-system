
CONFIG += c++11

# Add defines to compiler
DEFINES += "APPLICATION_TARGET=\\\"$${APPLICATION_TARGET}\\\"" \
    "APPLICATION_NAME=\\\"$${APPLICATION_NAME}\\\""

CONFIG(READY_FOR_CARSYSTEM) {
    DEFINES += READY_FOR_CARSYSTEM
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
