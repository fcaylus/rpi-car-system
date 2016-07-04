# Try to find the location of the mediainfo lib
exists(/usr/include/MediaInfo/MediaInfo.h) {
    INCLUDEPATH += /usr/include
} else:exists(/usr/local/include/MediaInfo/MediaInfo.h) {
    INCLUDEPATH += /usr/local/include
} else:exists(/opt/local/include/MediaInfo/MediaInfo.h) {
    INCLUDEPATH += /opt/local/include
}

exists(/usr/lib/libmediainfo*) {
    LIBS += -L/usr/lib -lmediainfo
} else:exists(/usr/local/lib/libmediainfo*) {
    LIBS += -L/usr/local/lib -lmediainfo
} else:exists(/usr/lib/x86_64-linux-gnu/libmediainfo*) {
    LIBS += -L/usr/lib/x86_64-linux-gnu -lmediainfo
} else:exists(/opt/local/lib/libmediainfo*) {
    LIBS += -L/opt/local/lib -lmediainfo
}

# Required by the library
DEFINES += UNICODE
