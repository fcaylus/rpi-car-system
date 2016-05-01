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
#include <QSysInfo>

class SysInfoManager : public QObject
{
        Q_OBJECT
    public:
        SysInfoManager(QObject* parent = nullptr): QObject(parent) {}

        Q_INVOKABLE bool isLittleEndian()
        {
            return QSysInfo::ByteOrder == QSysInfo::LittleEndian;
        }

        Q_INVOKABLE int wordSize()
        {
            return QSysInfo::WordSize;
        }

        Q_INVOKABLE QString buildAbi()
        {
            return QSysInfo::buildAbi();
        }

        Q_INVOKABLE QString cpuArch()
        {
            return QSysInfo::buildCpuArchitecture();
        }

        Q_INVOKABLE QString kernelType()
        {
            return QSysInfo::kernelType();
        }

        Q_INVOKABLE QString kernelVersion()
        {
            return QSysInfo::kernelVersion();
        }
};

#endif // SYSINFOMANAGER_H
