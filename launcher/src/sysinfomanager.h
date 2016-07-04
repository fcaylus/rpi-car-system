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

#ifndef SYSINFOMANAGER_H
#define SYSINFOMANAGER_H

#include <QObject>

class SysInfoManager : public QObject
{
        Q_OBJECT

        Q_PROPERTY(bool isLittleEndian READ isLittleEndian CONSTANT)
        Q_PROPERTY(int wordSize READ wordSize CONSTANT)
        Q_PROPERTY(QString buildAbi READ buildAbi CONSTANT)
        Q_PROPERTY(QString cpuArch READ cpuArch CONSTANT)
        Q_PROPERTY(QString kernelType READ kernelType CONSTANT)
        Q_PROPERTY(QString kernelVersion READ kernelVersion CONSTANT)

        Q_PROPERTY(QString programVersion READ programVersion CONSTANT)
        Q_PROPERTY(int hardwareVersion READ hardwareVersion CONSTANT)
        Q_PROPERTY(QString vlcVersion READ vlcVersion CONSTANT)
        Q_PROPERTY(QString vlcqtVersion READ vlcqtVersion CONSTANT)
        Q_PROPERTY(QString buildDate READ buildDate CONSTANT)

        Q_PROPERTY(qint64 bytesAvailable READ bytesAvailable CONSTANT)
        Q_PROPERTY(qint64 bytesTotal READ bytesTotal CONSTANT)
        Q_PROPERTY(qint64 bytesUsedConfig READ bytesUsedConfig CONSTANT)
        Q_PROPERTY(qint64 bytesUsedSystem READ bytesUsedSystem CONSTANT)

        Q_PROPERTY(QString bytesAvailableString READ bytesAvailableString CONSTANT)
        Q_PROPERTY(QString bytesTotalString READ bytesTotalString CONSTANT)
        Q_PROPERTY(QString bytesUsedConfigString READ bytesUsedConfigString CONSTANT)
        Q_PROPERTY(QString bytesUsedSystemString READ bytesUsedSystemString CONSTANT)

        Q_PROPERTY(QString license READ license CONSTANT)

    public:
        SysInfoManager(QObject* parent = nullptr);

        bool isLittleEndian() const;
        int wordSize() const;
        QString buildAbi() const;
        QString cpuArch() const;
        QString kernelType() const;
        QString kernelVersion() const;

        QString programVersion() const;
        int hardwareVersion() const;
        QString vlcVersion() const;
        QString vlcqtVersion() const;
        QString buildDate() const;

        QString license() const;

        qint64 bytesAvailable() const;
        qint64 bytesTotal() const;
        qint64 bytesUsedConfig() const;
        qint64 bytesUsedSystem() const;

        QString bytesAvailableString() const;
        QString bytesTotalString() const;
        QString bytesUsedConfigString() const;
        QString bytesUsedSystemString() const;
};

#endif // SYSINFOMANAGER_H
