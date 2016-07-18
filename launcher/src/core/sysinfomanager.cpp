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

#include "sysinfomanager.h"
#include "dirutility.h"

#include <QSysInfo>
#include <QStorageInfo>
#include <VLCQtCore/Instance.h>

SysInfoManager::SysInfoManager(QObject *parent): QObject(parent)
{
}

bool SysInfoManager::isLittleEndian() const
{
    return QSysInfo::ByteOrder == QSysInfo::LittleEndian;
}

int SysInfoManager::wordSize() const
{
    return QSysInfo::WordSize;
}

QString SysInfoManager::buildAbi() const
{
    return QSysInfo::buildAbi();
}

QString SysInfoManager::cpuArch() const
{
    return QSysInfo::buildCpuArchitecture();
}

QString SysInfoManager::kernelType() const
{
    return QSysInfo::kernelType();
}

QString SysInfoManager::kernelVersion() const
{
    return QSysInfo::kernelVersion();
}

QString SysInfoManager::programVersion() const
{
    return QString(APPLICATION_VERSION);
}

int SysInfoManager::hardwareVersion() const
{
    return HARDWARE_VERSION;
}

QString SysInfoManager::vlcVersion() const
{
    return VlcInstance::version();
}

QString SysInfoManager::vlcqtVersion() const
{
    return VlcInstance::libVersion();
}

QString SysInfoManager::buildDate() const
{
    return QString(BUILD_DATE);
}

QString SysInfoManager::license() const
{
    return DirUtility::readFile(QStringLiteral(":/LICENSE.html"));
}

//
// Disk usage
//

qint64 SysInfoManager::bytesAvailable() const
{
    return QStorageInfo::root().bytesAvailable();
}

qint64 SysInfoManager::bytesTotal() const
{
    return QStorageInfo::root().bytesTotal();
}

qint64 SysInfoManager::bytesUsedConfig() const
{
    return DirUtility::dirSize(Common::configDir());
}

qint64 SysInfoManager::bytesUsedSystem() const
{
    return bytesTotal() - bytesAvailable() - bytesUsedConfig();
}


QString SysInfoManager::bytesAvailableString() const
{
    return Common::bytesSizeToString(bytesAvailable());
}

QString SysInfoManager::bytesTotalString() const
{
    return Common::bytesSizeToString(bytesTotal());
}

QString SysInfoManager::bytesUsedConfigString() const
{
    return Common::bytesSizeToString(bytesUsedConfig());
}

QString SysInfoManager::bytesUsedSystemString() const
{
    return Common::bytesSizeToString(bytesUsedSystem());
}

