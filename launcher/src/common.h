#ifndef COMMON
#define COMMON

#include <QString>
#include <QCoreApplication>

namespace Common {
    static inline QString configDir()
    {
#ifdef READY_FOR_CARSYSTEM
        return QStringLiteral("/home/pi/config")
#else
        return QCoreApplication::applicationDirPath();
#endif
    }

    static inline QString musicDir()
    {
#ifdef READY_FOR_CARSYSTEM
        return QStringLiteral("/home/pi/music");
#else
        //return QStringLiteral("/media/fabien/Disque Multimedia/Music/Ma musique");
        return QStringLiteral("/home/fabien/Musique");
#endif
    }
}

#endif // COMMON

