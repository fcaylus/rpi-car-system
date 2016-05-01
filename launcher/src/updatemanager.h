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

#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QThread>
#include <QDebug>
#include <QMutex>

#include "common.h"
#include "filereader.h"
#include "../../updater/src/updatercodes.h"

class SearchUpdateThread : public QThread
{
        Q_OBJECT

    signals:
        void searchFinished(QStringList files, QStringList versions, QList<int> status);

    public:
        SearchUpdateThread(QObject *parent): QThread(parent) {}

        void run() Q_DECL_OVERRIDE
        {
            QStringList updateFiles;
            QStringList updateVersions;
            QList<int> updateVersionStatus;

            // Get all update packages
            int returnCode = 0;
            QStringList pkgList = Common::startProcessAndReadOutput(QCoreApplication::applicationDirPath() + "/updater",
                                                                    QStringList({"--search-for-updates"}),
                                                                    &returnCode).split("\n");

            if(returnCode == UPDATER_CODE_SUCCESS)
            {
                for(QString pkgPath : pkgList)
                {
                    int firstSpaceIdx = pkgPath.indexOf(" ", 0);
                    QString version = pkgPath.left(firstSpaceIdx);
                    QString path = pkgPath.right(pkgPath.size() - firstSpaceIdx - 1);

                    int versionStatus;
                    Common::startProcessAndReadOutput(QCoreApplication::applicationDirPath() + "/updater",
                                                      QStringList({"--check-version", path}),
                                                      &versionStatus);

                    updateFiles.append(path);
                    updateVersions.append(version);
                    updateVersionStatus.append(versionStatus);
                }
            }

            emit searchFinished(updateFiles, updateVersions, updateVersionStatus);
        }
};

// This manager lists available updates, their version and calls the updater if needed
class UpdateManager : public QObject
{
        Q_OBJECT

    private:
        QStringList _updateFiles;
        QStringList _updateVersions;
        // See "updater/src/updatercodes.h" for values
        QList<int> _updateVersionStatus;

        QMutex _mutex;
        bool _isRunning = false;

        QCoreApplication *_app;

    signals:
        void refreshFinished();

    public slots:

        // Non-blocking method
        // Users must wait for refreshFinished() signal
        void refreshUpdateList()
        {            
            SearchUpdateThread *th = new SearchUpdateThread(this);
            connect(th, &SearchUpdateThread::searchFinished, [this](QStringList files, QStringList versions, QList<int> status){
                _updateFiles = files;
                _updateVersions = versions;
                _updateVersionStatus = status;

                emit refreshFinished();
            });
            connect(th, &QThread::finished, [this, th](){
                th->deleteLater();
                _mutex.lock();
                _isRunning = false;
                _mutex.unlock();
            });

            _mutex.lock();
            _isRunning = true;
            _mutex.unlock();

            th->start();
        }

        // Shutdown the app
        // Launch script will handle the return code and start the updater
        void launchUpdate(const QString& updateFile)
        {
            FileReader::writeFile(QCoreApplication::applicationDirPath() + "/update-package-path", updateFile);
            _app->exit(UPDATE_CODE);
        }

    public:

        UpdateManager(QCoreApplication *app, QObject* parent = nullptr): QObject(parent)
        {
            _app = app;
        }

        // Getters
        Q_INVOKABLE QStringList updateFiles()
        {
            return _updateFiles;
        }

        Q_INVOKABLE QStringList updateVersions()
        {
            return _updateVersions;
        }

        Q_INVOKABLE QList<int> updateVersionStatus()
        {
            return _updateVersionStatus;
        }

        Q_INVOKABLE bool isRunning()
        {
            _mutex.lock();
            bool val = _isRunning;
            _mutex.unlock();
            return val;
        }
};

#endif // UPDATEMANAGER_H
