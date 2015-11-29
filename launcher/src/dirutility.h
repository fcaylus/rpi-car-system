/*
 * This file is part of RPI Car System.
 * Copyright (c) 2015 Fabien Caylus <toutjuste13@gmail.com>
 *
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

