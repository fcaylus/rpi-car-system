CONFIG(READY_FOR_CARSYSTEM) {
    LIBS += -lVLCQtCore
}
!CONFIG(READY_FOR_CARSYSTEM) {
    INCLUDEPATH += /usr/include
    LIBS += -L/usr/lib -lVLCQtCore
}
