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

#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <QFileInfo>
#include <QFile>
#include <QCryptographicHash>
#include <QObject>

#include "common.h"

class PasswordManager: public QObject
{
        Q_OBJECT
    public:
        PasswordManager(QObject* parent = nullptr): QObject(parent) {}

        static QString passwordFileName()
        {
            return QStringLiteral("rootpass");
        }

        static QString passwordFile()
        {
            return Common::configDir() + QStringLiteral("/") + passwordFileName();
        }

        static bool passFileExists()
        {
            return QFileInfo::exists(passwordFile());
        }

        Q_INVOKABLE bool createPasswordFile(const QString& password)
        {
            if(passFileExists())
                return false;

            QFile file(passwordFile());
            if(!file.open(QFile::ReadWrite))
                return false;

            file.write(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_512));
            file.close();
            return true;
        }

        Q_INVOKABLE bool checkPassword(const QString& password)
        {
            if(!passFileExists())
                return false;

            QFile file(passwordFile());
            if(!file.open(QFile::ReadOnly))
                return false;

            return file.read(1024) == QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_512);
        }
};

#endif // PASSWORDMANAGER_H
