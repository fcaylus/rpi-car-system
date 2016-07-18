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

#ifndef LANGUAGEMANAGER
#define LANGUAGEMANAGER

#include <QObject>
#include <QCoreApplication>
#include <QSettings>

#include "common.h"

class LanguageManager: public QObject
{
        Q_OBJECT

    private:
        QCoreApplication *_app;
        QSettings *_settings;
        QString _defLang;

    public slots:
        void changeLanguage(const QString &code)
        {
            _settings->setValue("locale", code);
            _app->exit(REBOOT_CODE);
        }

    public:
        LanguageManager(QCoreApplication *app, QSettings *settings, QString lang, QObject* parent = nullptr): QObject(parent)
        {
            _app = app;
            _settings = settings;
            _defLang = lang;
        }

        Q_INVOKABLE QString currentLanguage()
        {
            return _defLang;
        }

};

#endif // LANGUAGEMANAGER

