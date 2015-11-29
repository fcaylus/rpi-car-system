/*
 * This file is part of RPI Car System.
 * Copyright (c) 2015 Fabien Caylus <toutjuste13@gmail.com>
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

#include <QGuiApplication>
#include <QtQml>
#include <QDirIterator>
#include <QTranslator>
#include <QSettings>

#include "soundmanager.h"
#include "passwordmanager.h"

#include <VLCQtCore/Common.h>

static const QString settingsLocaleStr = "locale";

void loadTranslations(const QString& path, const QString &locale, QCoreApplication *app)
{
    QDirIterator iterator(path, QStringList() << "*_" + locale + ".qm", QDir::Files, QDirIterator::Subdirectories);
    while(iterator.hasNext())
    {
        iterator.next();
        QTranslator *translator = new QTranslator(app);
        translator->load(iterator.fileName(), iterator.fileInfo().canonicalPath());
        app->installTranslator(translator);
    }
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setOrganizationName(APPLICATION_NAME);

    QSettings *settings = new QSettings(QCoreApplication::applicationDirPath() + QStringLiteral("/settings.ini"), QSettings::IniFormat);
    settings->setFallbacksEnabled(false);

    // Get locale (system locale by default)
    const QString locale = settings->value(settingsLocaleStr, QLocale::system().name().section('_', 0, 0)).toString();

    // Load default Qt translations
    QTranslator qtTranslator;
    qtTranslator.load(QStringLiteral("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    // Load translations in the "translations" dir
    loadTranslations(QCoreApplication::applicationDirPath() + QStringLiteral("/translations"), locale, &app);

    //
    // QML Stuff
    //

    qmlRegisterType<MediaInfo>();

    VlcCommon::setPluginPath(app.applicationDirPath() + QStringLiteral("/plugins"));

    SoundManager *soundMgr = new SoundManager(settings);
    PasswordManager *passMgr = new PasswordManager();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("soundManager"), soundMgr);
    engine.rootContext()->setContextProperty(QStringLiteral("passwordManager"), passMgr);
    engine.rootContext()->setContextProperty(QStringLiteral("isPassFileCreated"), QVariant(PasswordManager::isPassFileExists()));
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    const int result = app.exec();

    // Save settings
    settings->setValue(settingsLocaleStr, locale);// TODO: update media list strings if changed
    soundMgr->saveSettings();

    soundMgr->deleteLater();
    settings->sync();
    settings->deleteLater();
    return result;
}
