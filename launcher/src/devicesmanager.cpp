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

#include "devicesmanager.h"
#include "common.h"
#include "dirutility.h"
#include "mediainfo.h"

#include <QStorageInfo>
#include <QDebug>

DevicesManager::DevicesManager(QObject *parent) : QObject(parent)
{
    refreshDevicesList();
}

DevicesManager::~DevicesManager()
{
    _thread.wait();
    _thread.quit();
}

QStringList DevicesManager::availableDevicesName()
{
    return _devicesName;
}

QStringList DevicesManager::availableDevicesPath()
{
    return _devicesPath;
}

//
// Public slots
//

void DevicesManager::refreshDevicesList()
{
    _devicesName.clear();
    _devicesPath.clear();

    for(QStorageInfo info: QStorageInfo::mountedVolumes())
    {
        if(info.isReady() && info.isValid() && !info.isRoot() && info.rootPath().startsWith("/media"))
        {
            _devicesPath.append(info.rootPath());
            if(info.name().isEmpty())
                _devicesName.append(tr("%1 device").arg(bytesSizeToString(info.bytesTotal())));
            else
                _devicesName.append(info.name() + " (" + bytesSizeToString(info.bytesTotal()) + ")");
        }
    }

    emit devicesListRefreshed();
}

void DevicesManager::deleteLocalFiles()
{
    startWorker(new DeleteLocalFilesWorker());
}

void DevicesManager::copyDeviceFiles(const QString &devPath)
{
    startWorker(new CopyFilesWorker(devPath));
}

// Private
void DevicesManager::startWorker(DeviceWorker *worker)
{
    _thread.quit();
    _thread.wait();

    worker->moveToThread(&_thread);
    connect(&_thread, &QThread::started, worker, &DeviceWorker::start);
    connect(&_thread, &QThread::finished, worker, &QObject::deleteLater);

    connect(worker, &DeviceWorker::started, this, &DevicesManager::newTaskStarted);
    connect(worker, &DeviceWorker::updated, this, &DevicesManager::taskUpdate);
    connect(worker, &DeviceWorker::finished, this, &DevicesManager::taskFinished);

    _thread.start();
}

//
// Static
//

QString DevicesManager::bytesSizeToString(qint64 bytes)
{
    if(bytes < 0)
        return QString();

    if(bytes < Q_INT64_C(1000000))
        return QString::number(bytes/1000., 'g', 2) + QString(" ") + tr("kB");
    if(bytes < Q_INT64_C(1000000000))
        return QString::number(bytes/1000000., 'g', 2) + QString(" ") + tr("MB");
    if(bytes < Q_INT64_C(1000000000000))
        return QString::number(bytes/1000000000., 'g', 2) + QString(" ") + tr("GB");
    else
        return QString::number(bytes/1000000000000., 'g', 2) + QString(" ") + tr("TB");
}


//--------------------------------------------
//--- DeleteLocalFilesWorker -----------------
//--------------------------------------------

DeleteLocalFilesWorker::DeleteLocalFilesWorker(): DeviceWorker()
{

}

void DeleteLocalFilesWorker::start()
{
#ifdef READY_FOR_CARSYSTEM

    _stepsCount = DirUtility::countFilesRecursively(Common::musicDir());
    emit started(_stepsCount, QStringLiteral("delete"));
    removeFilesInDir(Common::musicDir(), true);
    emit finished(QStringLiteral("delete"));

#else
    _stepsCount = 0;
    qDebug() << "Deleting is only available on RPI !";
    emit started(0, QStringLiteral("delete"));
    emit finished(QStringLiteral("delete"));
#endif
}

//
// Private
void DeleteLocalFilesWorker::removeFilesInDir(const QString &path, bool firstDir)
{
    QDirIterator it(path, QDir::AllDirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot | QDir::Writable);
    while(it.hasNext())
    {
        it.next();

        if(it.fileInfo().isDir())
            removeFilesInDir(it.filePath(), false);
        else
        {
            QFile::remove(it.filePath());

            _currentStep += 1;
            emit updated(_currentStep, QStringLiteral("delete"));
        }
    }

    // Don't delete first dir
    if(!firstDir)
    {
        QDir dir(path);
        dir.rmdir(path);
    }
}

//--------------------------------------------
//--- CopyFilesWorker ------------------------
//--------------------------------------------

CopyFilesWorker::CopyFilesWorker(const QString &devPath): DeviceWorker()
{
    _devPath = devPath;
}

// TODO: finish
void CopyFilesWorker::start()
{
#ifdef READY_FOR_CARSYSTEM

    QStringList musicFilesFormats = MediaInfo::musicFilesFormats();

    _stepsCount = DirUtility::countFilesRecursively(_devPath, musicFilesFormats);
    emit started(_stepsCount, QStringLiteral("copy"));

    QDirIterator it(_devPath, musicFilesFormats,
                    QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Writable | QDir::Hidden,
                    QDirIterator::Subdirectories);

    while(it.hasNext())
    {
        it.next();
        _currentStep += 1;

        QFileInfo info = it.fileInfo();
        const QString relativeDir = info.canonicalPath().remove(_devPath);
        const QString newDir = Common::musicDir() + relativeDir;

        // Create directory
        QDir destDir(Common::musicDir());
        destDir.mkpath(newDir);

        // Copy file
        QFile file(info.canonicalFilePath());
        file.copy(newDir + "/" + info.fileName());

        emit updated(_currentStep, QStringLiteral("copy"));
    }

    emit finished(QStringLiteral("copy"));

#else
    _stepsCount = 0;
    qDebug() << "Copying is only available on RPI !";
    emit started(0, QStringLiteral("copy"));
    emit finished(QStringLiteral("copy"));
#endif
}








