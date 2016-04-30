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

#ifndef DEVICESMANAGER_H
#define DEVICESMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QThread>

class DeviceWorker;

/*
 * This class manages external devices like USB sticks
 */

class DevicesManager : public QObject
{
        Q_OBJECT

    public:
        explicit DevicesManager(QObject *parent = nullptr);
        ~DevicesManager();

        // Getters
        Q_INVOKABLE QStringList availableDevicesPath();
        Q_INVOKABLE QStringList availableDevicesName();

        // Root storage sizes
        Q_INVOKABLE qint64 rootStorageSize();
        Q_INVOKABLE qint64 rootStorageAvailableSize();
        Q_INVOKABLE qint64 musicDirSize();
        Q_INVOKABLE QString rootStorageSizeStr();
        Q_INVOKABLE QString rootStorageAvailableSizeStr();
        Q_INVOKABLE QString musicDirSizeStr();

        Q_INVOKABLE QString sizeStringFromBytes(qint64 bytes);

    public slots:

        void refreshDevicesList();
        void deleteLocalFiles();
        void copyDeviceFiles(const QString& devPath);

    signals:

        void devicesListRefreshed();

        void newTaskStarted(uint taskLength, QString taskName);
        void taskFinished(QString taskName);
        void taskUpdate(uint value, QString taskName);

    private:
        QStringList _devicesPath;
        QStringList _devicesName;

        QThread _thread;

        void startWorker(DeviceWorker *worker);
};

class DeviceWorker: public QObject
{
        Q_OBJECT

    protected:
        uint _stepsCount = 0;
        uint _currentStep = 0;

    public:
        DeviceWorker(): QObject(){}
        ~DeviceWorker()
        {
            disconnect();
        }

        uint stepsCount() const
        {
            return _stepsCount;
        }

    public slots:
        virtual void start() = 0;

    signals:

        void started(uint stepsCount, QString taskName);
        void updated(uint currentStep, QString taskName);
        void finished(QString taskName);
};

class DeleteLocalFilesWorker: public DeviceWorker
{
        Q_OBJECT

    public:
        DeleteLocalFilesWorker();
        virtual void start();

    private:
        void removeFilesInDir(const QString& path, bool firstDir);
};

class CopyFilesWorker: public DeviceWorker
{
        Q_OBJECT

    public:
        CopyFilesWorker(const QString& devPath);
        virtual void start();

    private:
        QString _devPath;
};



#endif // DEVICESMANAGER_H
