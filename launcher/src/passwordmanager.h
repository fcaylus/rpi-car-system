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
        PasswordManager() {}

        static QString passwordFileName()
        {
            return QStringLiteral("rootpass");
        }

        static QString passwordFile()
        {
            return Common::configDir() + QStringLiteral("/") + passwordFileName();
        }

        static bool isPassFileExists()
        {
            return QFileInfo::exists(passwordFile());
        }

        Q_INVOKABLE bool createPasswordFile(const QString& password)
        {
            if(isPassFileExists())
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
            if(!isPassFileExists())
                return false;

            QFile file(passwordFile());
            if(!file.open(QFile::ReadOnly))
                return false;

            return file.read(1024) == QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_512);
        }
};

#endif // PASSWORDMANAGER_H
