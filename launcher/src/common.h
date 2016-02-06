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

#ifndef COMMON
#define COMMON

#include <QString>
#include <QCoreApplication>
#include <QProcess>
#include <QDebug>

// Pugi save format
#ifdef QT_DEBUG
#define PUGI_SAVE_FORMAT pugi::format_default
#else
#define PUGI_SAVE_FORMAT pugi::format_raw
#endif

namespace Common {

    static inline QString startProcessAndReadOutput(const QString& name, QStringList args = QStringList())
    {
        QProcess process;
        process.setProcessChannelMode(QProcess::MergedChannels);
        process.start(name, args);
        if(!process.waitForStarted())
            return QString();

        QByteArray data;
        while(process.waitForReadyRead())
            data.append(process.readAll());

        if(data.endsWith("\n"))
            data.remove(data.length() - 1, 1);

        return data;
    }

    static inline QString bytesSizeToString(qint64 bytes)
    {
        if(bytes < 0)
            return QString();

        if(bytes < Q_INT64_C(1000000))
            return QString::number(bytes/1000., 'f', 2) + QString(" ") + QCoreApplication::tr("kB");
        if(bytes < Q_INT64_C(1000000000))
            return QString::number(bytes/1000000., 'f', 2) + QString(" ") + QCoreApplication::tr("MB");
        if(bytes < Q_INT64_C(1000000000000))
            return QString::number(bytes/1000000000., 'f', 2) + QString(" ") + QCoreApplication::tr("GB");
        else
            return QString::number(bytes/1000000000000., 'f', 2) + QString(" ") + QCoreApplication::tr("TB");
    }

    static inline QString configDir()
    {
#ifdef READY_FOR_CARSYSTEM
        return QStringLiteral("/root/config");
#else
        return QCoreApplication::applicationDirPath();
#endif
    }

    static inline QString musicDir()
    {
#ifdef READY_FOR_CARSYSTEM
        return QStringLiteral("/root/music");
#else
        // Get standard music path
        const QString path = startProcessAndReadOutput("xdg-user-dir", QStringList({"MUSIC"}));
        return path.isEmpty() ? QString("/home/" + qgetenv("USER") + "/Music") : path;
#endif
    }
}

#endif // COMMON

