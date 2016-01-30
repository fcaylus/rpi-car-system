/*
 * This file is part of RPI Car System.
 * Copyright (c) 2016 Fabien Caylus <toutjuste13@gmail.com>
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
#include <QDirIterator>
#include <QFileInfo>
#include <QStorageInfo>

#include "common.h"

namespace DirUtility
{
    static inline QString fileNameFromPath(QString path)
    {
        QFileInfo info(path);
        return info.fileName();
    }

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

    static inline uint countFilesRecursively(QDirIterator *it)
    {
        uint count = 0;
        while(it->hasNext())
        {
            it->next();
            count += 1;
        }
        return count;
    }

    static inline uint countFilesRecursively(const QString& dirPath, const QStringList& filters)
    {
        QDirIterator it(dirPath,
                        filters,
                        QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Writable | QDir::Hidden,
                        QDirIterator::Subdirectories);
        return countFilesRecursively(&it);
    }

    static inline uint countFilesRecursively(const QString& dirPath)
    {
        QDirIterator it(dirPath,
                        QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Writable | QDir::Hidden,
                        QDirIterator::Subdirectories);
        return countFilesRecursively(&it);
    }

    static inline qint64 dirSize(const QString& dirPath)
    {
        qint64 size = 0;
        QDirIterator it(dirPath,
                        QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable | QDir::Hidden,
                        QDirIterator::Subdirectories);
        while(it.hasNext())
        {
            it.next();
            size += it.fileInfo().size();
        }

        return size;
    }

    static inline void listUSBDevices(QStringList *pathList, QStringList *nameList = nullptr)
    {
        if(pathList == nullptr)
            return;

        pathList->clear();
        if(nameList)
            nameList->clear();

        for(QStorageInfo info: QStorageInfo::mountedVolumes())
        {
            if(info.isReady() && info.isValid() && !info.isRoot() && info.rootPath().startsWith("/media"))
            {
                pathList->append(info.rootPath());
                if(nameList)
                {
                    if(info.name().isEmpty())
                        nameList->append(QCoreApplication::tr("%1 device").arg(Common::bytesSizeToString(info.bytesTotal())));
                    else
                    {
                        QString name = info.name();
                        // This code handle \xHH characters (for example \x20 for a space)
                        while(name.contains("\\x"))
                        {
                            int index = name.indexOf("\\x");
                            char ch = name.mid(index + 2, 2).toInt(nullptr, 16);
                            name.replace(name.indexOf("\\x"), 4, QChar(ch));
                        }
                        nameList->append(name + " (" + Common::bytesSizeToString(info.bytesTotal()) + ")");
                    }
                }
            }
        }
    }

    // Get a unique filename
    // Use the date-time and a random number
    // (NOTE: it's not guaranted to be unique)
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

