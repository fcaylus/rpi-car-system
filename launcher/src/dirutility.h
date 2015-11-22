#ifndef DIRUTILITY_H
#define DIRUTILITY_H

#include <ctime>
#include <random>

#include <QString>
#include <QUrl>
#include <QFile>

namespace DirUtility
{
    // Return the file path for the specified URI
    static inline QString filePathForURI(QString uri)
    {
        if(uri.startsWith(QStringLiteral("file://")))
            uri.remove(0, 7);

        return QUrl::fromPercentEncoding(uri.toUtf8());
    }

    static inline bool removeIfExists(const QString& path)
    {
        QFile file(path);
        if(file.exists())
            return file.remove();
        return true;
    }

    // Get a unique filename
    // Use the date-time and a random number
    // It's not guaranted to be unique, but there is a lot of chance
    // (if all temp files are generated with this method)
    static inline QString uniqueFileName()
    {
        // Generate datetime
        time_t now = std::time(nullptr);
        struct tm tstruct = *std::localtime(&now);
        char buf[100];
        std::strftime(buf, sizeof(buf), "%Y%m%d-%H%M%S", &tstruct);

        // Generate random number
        std::minstd_rand generator;
        std::uniform_int_distribution<> distrib(0, 10000000);
        const int randNb = distrib(generator);

        return QString("tempfile-") + QString(buf) + QString::number(randNb);
    }
}

#endif // DIRUTILITY_H

