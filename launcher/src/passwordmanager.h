#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <QFileInfo>
#include <QFile>
#include <QCryptographicHash>

#include "common.h"

namespace PasswordManager {

    static inline QString passwordFileName()
    {
        return QStringLiteral("rootpass");
    }

    static inline QString passwordFile()
    {
        return Common::configDir() + QStringLiteral("/") + passwordFileName();
    }

    static inline bool isPassFileExists()
    {
        return QFileInfo::exists(passwordFile());
    }

    static inline bool createPasswordFile(const QString& password)
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

    static inline bool checkPassword(const QString& password)
    {
        if(!isPassFileExists())
            return false;

        QFile file(passwordFile());
        if(!file.open(QFile::ReadOnly))
            return false;

        return file.read(1024) == QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_512);
    }
}

#endif // PASSWORDMANAGER_H
