CONFIG(READY_FOR_CARSYSTEM) {
    # Cross compilation
    LIBS += -lVLCQtCore
} else {
    # Build on local machine
    INCLUDEPATH += /usr/include
    LIBS += -L/usr/lib -lVLCQtCore
}
