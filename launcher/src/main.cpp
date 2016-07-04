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

#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <QLibraryInfo>
#include <QUrl>
#include <QQuickView>
#include <QQuickItem>
#include <QTranslator>
#include <QSettings>
#include <QDir>
#include <QThread>

#include "languagemanager.h"
#include "passwordmanager.h"
#include "sysinfomanager.h"
#include "updatemanager.h"
#include "tshandler.h"

#include "musicplayer.h"
#include "musicqueuelistmodel.h"
#include "mediamanager/mediamanager.h"
#include "mediamanager/metadatalistmodel.h"
#include "mediamanager/musiclistmodel.h"
#include "mediamanager/playlistlistmodel.h"

static const QString settingsLocaleStr = "locale";

int main(int argc, char *argv[])
{
#ifdef READY_FOR_CARSYSTEM
    QCoreApplication::addLibraryPath(QStringLiteral("/usr/lib/qt/plugins"));
    qDebug() << "Car system version" << APPLICATION_VERSION;
#endif

    QGuiApplication *app = new QGuiApplication(argc, argv);
    QGuiApplication::setApplicationName(APPLICATION_NAME);
    QGuiApplication::setOrganizationName(APPLICATION_NAME);

    const QString appDirPath = app->applicationDirPath();

    // Try to remove previous update file
    QFile::remove(appDirPath + "/update-package-path");

    QSettings *settings = new QSettings(appDirPath + QStringLiteral("/settings.ini"), QSettings::IniFormat);
    settings->setFallbacksEnabled(false);

    // Get locale (system locale by default)
    const QString locale = settings->value(settingsLocaleStr, QLocale::system().name().section('_', 0, 0)).toString();
    settings->setValue(settingsLocaleStr, locale);

    // Load translations
    QTranslator appTranslator;
    appTranslator.load(QString(APPLICATION_TARGET "_") + locale + QStringLiteral(".qm"), appDirPath);
    app->installTranslator(&appTranslator);

    //
    // QML Stuff
    //

    // Register some types into the QT metatype System
    qRegisterMetaType<MediaInfoList>("MediaInfoList");
    qRegisterMetaType<MetadataTypeList>("MetadataTypeList");

    qmlRegisterType<MusicListModel>("rpicarsystem.mediamanager", 1, 0, "MusicListModel");
    qmlRegisterType<MetadataListModel>("rpicarsystem.mediamanager", 1, 0, "MetadataListModel");
    qmlRegisterType<MusicQueueListModel>("rpicarsystem.mediamanager", 1, 0, "MusicQueueListModel");
    qmlRegisterType<PlaylistListModel>("rpicarsystem.mediamanager", 1, 0, "PlaylistListModel");
    qmlRegisterUncreatableType<MediaInfo>("rpicarsystem.mediamanager", 1, 0, "MediaInfo", "MediaInfo is only used for its enums.");

    MusicPlayer *musicPlayer = new MusicPlayer(settings);
    MediaManager::instance()->initialScan();

    PasswordManager *passMgr = new PasswordManager();
    LanguageManager *langMgr = new LanguageManager(app, settings, locale);
    SysInfoManager *sysinfoMgr = new SysInfoManager();
    UpdateManager *updateMgr = new UpdateManager(app);

    QQuickView *view = new QQuickView();
    view->connect(view->engine(), &QQmlEngine::quit, app, &QGuiApplication::quit);
    view->setResizeMode(QQuickView::SizeViewToRootObject);

    QQmlContext *context = view->rootContext();
    context->setContextProperty(QStringLiteral("musicPlayer"), musicPlayer);
    context->setContextProperty(QStringLiteral("mediaManager"), MediaManager::instance());
    context->setContextProperty(QStringLiteral("passwordManager"), passMgr);
    context->setContextProperty(QStringLiteral("languageManager"), langMgr);
    context->setContextProperty(QStringLiteral("sysinfoManager"), sysinfoMgr);
    context->setContextProperty(QStringLiteral("updateManager"), updateMgr);

    context->setContextProperty(QStringLiteral("passFileCreated"), QVariant(PasswordManager::passFileExists()));

    view->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));

#ifdef READY_FOR_CARSYSTEM
        view->showFullScreen();
#else
        view->show();
#endif

    TSHandler *tsHandler = new TSHandler(view);
    QThread tsThread;
    tsHandler->moveToThread(&tsThread);
    QObject::connect(&tsThread, &QThread::finished, tsHandler, &QObject::deleteLater);
    QObject::connect(&tsThread, &QThread::started, tsHandler, &TSHandler::handle);
    tsThread.start();

    // Launch app
    const int resultCode = app->exec();

    qDebug() << "Cleaning up ...";

    tsHandler->requestStop();
    tsThread.terminate();

    delete view;

    // Save settings
    musicPlayer->saveSettings();

    settings->sync();
    delete settings;

    delete passMgr;
    delete langMgr;
    delete sysinfoMgr;
    delete updateMgr;

    delete musicPlayer;

    delete app;

    return resultCode;
}
