CONFIG(READY_FOR_CARSYSTEM) {
    LIBS += -lVLCQtCore
}
!CONFIG(READY_FOR_CARSYSTEM) {
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib -lVLCQtCore
}
