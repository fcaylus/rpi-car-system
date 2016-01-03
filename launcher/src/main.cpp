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
#include <QQmlEngine>
#include <QQmlContext>
#include <QLibraryInfo>
#include <QUrl>
#include <QQuickView>
#include <QTranslator>
#include <QSettings>

#include "soundmanager.h"
#include "passwordmanager.h"
#include "languagemanager.h"
#include "filereader.h"
#include "devicesmanager.h"

#include <VLCQtCore/Common.h>

static const QString settingsLocaleStr = "locale";

int main(int argc, char *argv[])
{
    int resultCode = 0;
#ifdef READY_FOR_CARSYSTEM
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
#endif

    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(APPLICATION_NAME);
    QGuiApplication::setOrganizationName(APPLICATION_NAME);

#ifdef READY_FOR_CARSYSTEM
    qApp->addLibraryPath(QStringLiteral("/usr/lib/qt/plugins"));
#endif

    QSettings *settings = new QSettings(QGuiApplication::applicationDirPath() + QStringLiteral("/settings.ini"), QSettings::IniFormat);
    settings->setFallbacksEnabled(false);

    // Get locale (system locale by default)
    const QString locale = settings->value(settingsLocaleStr, QLocale::system().name().section('_', 0, 0)).toString();
    settings->setValue(settingsLocaleStr, locale);

    // Load default Qt translations
    QTranslator qtTranslator;
    qtTranslator.load(QStringLiteral("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    // Load app translations
    QTranslator appTranslator;
    appTranslator.load(QString(APPLICATION_TARGET "_") + locale + QStringLiteral(".qm"), QGuiApplication::applicationDirPath());
    app.installTranslator(&appTranslator);

    //
    // QML Stuff
    //

    VlcCommon::setPluginPath(app.applicationDirPath() + QStringLiteral("/plugins"));

    SoundManager *soundMgr = new SoundManager(settings);
    PasswordManager *passMgr = new PasswordManager();
    LanguageManager *langMgr = new LanguageManager(&app, settings, locale);
    FileReader *fileReader = new FileReader();
    DevicesManager *devicesMgr = new DevicesManager();

    QQuickView view;
    view.connect(view.engine(), &QQmlEngine::quit, &app, &QGuiApplication::quit);
    view.setResizeMode(QQuickView::SizeViewToRootObject);

    QQmlContext *context = view.rootContext();
    context->setContextProperty(QStringLiteral("soundManager"), soundMgr);
    context->setContextProperty(QStringLiteral("passwordManager"), passMgr);
    context->setContextProperty(QStringLiteral("languageManager"), langMgr);
    context->setContextProperty(QStringLiteral("devicesManager"), devicesMgr);
    context->setContextProperty(QStringLiteral("fileReader"), fileReader);
    context->setContextProperty(QStringLiteral("isPassFileCreated"), QVariant(PasswordManager::isPassFileExists()));
    context->setContextProperty(QStringLiteral("programVersion"), QVariant(QString(APPLICATION_VERSION)));
    context->setContextProperty(QStringLiteral("hardwareVersion"), QVariant(HARDWARE_VERSION));
    context->setContextProperty(QStringLiteral("vlcVersion"), QVariant(VlcInstance::version()));
    context->setContextProperty(QStringLiteral("vlcqtVersion"), QVariant(VlcInstance::libVersion()));

    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    if(QGuiApplication::platformName() == QLatin1String("eglfs"))
        view.showFullScreen();
    else
        view.show();

    // Launch app
    resultCode = app.exec();

    // Save settings
    soundMgr->saveSettings();

    soundMgr->deleteLater();
    passMgr->deleteLater();
    langMgr->deleteLater();
    fileReader->deleteLater();

    settings->sync();
    settings->deleteLater();

    return resultCode;
}
