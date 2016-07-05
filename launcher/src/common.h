/*
 * This file is part of RPI Car System.
 * Copyright (c) 2016 Fabien Caylus <tranqyll.dev@gmail.com>
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

//
// Some exit codes (don't forget to modify launcher.sh also)
//

#define REBOOT_CODE   10 // Launcher will immediately reboot
#define UPDATE_CODE   20 // Updater will be launched
#define SHUTDOWN_CODE 30 // System will be shutdown

namespace Common {

    static inline QString startProcessAndReadOutput(const QString& name, QStringList args = QStringList(), int *returnCode = nullptr)
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

        if(returnCode != nullptr)
        {
            process.waitForFinished();
            if(process.exitStatus() == QProcess::NormalExit)
                *returnCode = process.exitCode();
        }

        return data;
    }

    // Return the unit, rounded up to <specified> decimals with the correct suffix
    // Accept any type for nb (fox example, qint64 or float)
    template<typename T>
    static inline QString unitToString(T nb, const QString& suffix, const int nbOfDecimals, bool removeTrailingZeros = true)
    {
        QString str;
        if(nb < 1000)
            str = QString("%1 %2").arg(QString::number(nb/1., 'f', nbOfDecimals), suffix);
        else if(nb < 1000000)
            str = QString("%1 %2%3").arg(QString::number(nb/1000., 'f', nbOfDecimals), "k", suffix);
        else if(nb < 1000000000)
            str = QString("%1 %2%3").arg(QString::number(nb/1000000., 'f', nbOfDecimals), "M", suffix);
        else if(nb < Q_INT64_C(1000000000000))
            str = QString("%1 %2%3").arg(QString::number(nb/1000000000., 'f', nbOfDecimals), "G", suffix);
        else
            str = QString("%1 %2%3").arg(QString::number(nb/1000000000000., 'f', nbOfDecimals), "T", suffix);

        // Remove trailing zero
        if(removeTrailingZeros && str.contains("."))
        {
            while(str.contains("0 "))
                str.remove(str.indexOf("0 "), 1);

            if(str.contains(". "))
                str.remove(str.indexOf(". "), 1);
        }

        // Check for "-0" strings (instead of "0")
        if((removeTrailingZeros && str.startsWith("-0 ")) ||
           (!removeTrailingZeros && str.contains(QRegExp(QString("-0.0{%1}").arg(nbOfDecimals)))))
           str.remove(0, 1);

        return str;
    }

    static inline QString bytesSizeToString(qint64 bytes)
    {
        return unitToString(bytes, QCoreApplication::translate("common.h", "B", "Unit of Bytes"), 2);
    }

    // Convert time in ms to "hh:mm:ss"
    static inline QString timeToString(unsigned int time)
    {
        const unsigned int timeInSeconds = time / 1000;
        const unsigned int hours = timeInSeconds / 3600;
        const unsigned int minutes = (timeInSeconds % 3600) / 60;
        const unsigned int secondes = timeInSeconds - hours * 3600 - minutes * 60;

        QString result = "";

        if(hours != 0)
            result += QString::number(hours) + QString(":");

        if(minutes < 10 && hours != 0)
            result += QString("0") + QString::number(minutes);
        else
            result += QString::number(minutes);

        result += ":";

        if(secondes < 10)
            result += QString("0") + QString::number(secondes);
        else
            result += QString::number(secondes);

        return result;
    }

    static inline QString configDir()
    {
#ifdef READY_FOR_CARSYSTEM
        return QStringLiteral("/root/config");
#else
        return QCoreApplication::applicationDirPath() + QLatin1String("/config");
#endif
    }
}

#endif // COMMON

