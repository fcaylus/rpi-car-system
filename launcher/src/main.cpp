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
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QFont>
#include <QFontDatabase>

#include "core/languagemanager.h"
#include "core/passwordmanager.h"
#include "core/sysinfomanager.h"
#include "core/updatemanager.h"
#include "gui/tshandler.h"

#include "core/mediamanager/playlist.h"
#include "core/mediamanager/mediamanager.h"
#include "gui/models/musicqueuelistmodel.h"
#include "gui/models/metadatalistmodel.h"
#include "gui/models/musiclistmodel.h"
#include "gui/models/playlistlistmodel.h"
#include "gui/models/keyboardlayoutmodel.h"
#include "gui/musicplayer.h"

#include "gui/controls/button.h"
#include "gui/controls/progressbar.h"
#include "gui/controls/stackview.h"

static const QString settingsLocaleStr = "locale";

// Load the "heavy" stuff
void afterSplashScreen(QGuiApplication *app, QQuickView *view, QSettings *settings, const QString& locale)
{
    const QString appDirPath = app->applicationDirPath();

    // Remove useless files
    QFile::remove(appDirPath + QLatin1String("/arguments.txt"));
    QFile::remove(appDirPath + "/update-package-path");

    QCommandLineParser parser;
    parser.setApplicationDescription("RPI Car System main app");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption(QCommandLineOption("clear-usb-cache"));
    parser.addOption(QCommandLineOption("clear-settings"));
    parser.addOption(QCommandLineOption("clear-playlists"));
    parser.addOption(QCommandLineOption("clear-equalizer-confs"));
    parser.addOption(QCommandLineOption("clear-all-cache"));

    parser.process(app->arguments());

    if(parser.isSet("clear-all-cache"))
    {
        QDir dir(Common::configDir());
        dir.removeRecursively();
    }
    else
    {
        if(parser.isSet("clear-usb-cache"))
        {
            QDir dir(Common::configDir() + "/usbsources");
            dir.removeRecursively();
        }
        if(parser.isSet("clear-settings"))
        {
            QFile::remove(Common::configDir() + QLatin1String("/settings.ini"));
        }
        if(parser.isSet("clear-playlists"))
        {
            QDir dir(Playlist::playlistDirectory());
            dir.removeRecursively();
        }
        if(parser.isSet("clear-equalizer-confs"))
        {
            QDir dir(MusicPlayer::equalizerMainConfigDir());
            dir.removeRecursively();
        }
    }

    //
    // QML Stuff
    //

    const int fontId = QFontDatabase::addApplicationFont(":/fonts/OpenSans-Regular");
    app->setFont(QFont(QFontDatabase::applicationFontFamilies(fontId).at(0)));

    // Register some types into the QT metatype System
    qRegisterMetaType<MediaInfoList>("MediaInfoList");
    qRegisterMetaType<MetadataTypeList>("MetadataTypeList");

    qmlRegisterType<MusicListModel>("rpicarsystem.mediamanager", 1, 0, "MusicListModel");
    qmlRegisterType<MetadataListModel>("rpicarsystem.mediamanager", 1, 0, "MetadataListModel");
    qmlRegisterType<MusicQueueListModel>("rpicarsystem.mediamanager", 1, 0, "MusicQueueListModel");
    qmlRegisterType<PlaylistListModel>("rpicarsystem.mediamanager", 1, 0, "PlaylistListModel");
    qmlRegisterUncreatableType<MediaInfo>("rpicarsystem.mediamanager", 1, 0, "MediaInfo", "MediaInfo is only used for its enums.");

    qmlRegisterType<KeyboardLayoutModel>("rpicarsystem.keyboard", 1, 0, "KeyboardLayoutModel");
    qmlRegisterUncreatableType<KeyboardFirstRowModel>("rpicarsystem.keyboard", 1, 0, "KeyboardFirstRowModel", "Use KeyboardLayoutModel instead.");
    qmlRegisterUncreatableType<KeyboardSecondRowModel>("rpicarsystem.keyboard", 1, 0, "KeyboardSecondRowModel", "Use KeyboardLayoutModel instead.");
    qmlRegisterUncreatableType<KeyboardThirdRowModel>("rpicarsystem.keyboard", 1, 0, "KeyboardThirdRowModel", "Use KeyboardLayoutModel instead.");

    qmlRegisterType<Button>("rpicarsystem.controls", 1, 0, "AbstractButton");
    qmlRegisterType<ProgressBar>("rpicarsystem.controls", 1, 0, "AbstractProgressBar");
    qmlRegisterType<StackView>("rpicarsystem.controls", 1, 0, "AbstractStackView");

    MusicPlayer *musicPlayer = new MusicPlayer(settings);
    musicPlayer->init();

    MediaManager::instance()->initialScan();

    PasswordManager *passMgr = new PasswordManager();
    LanguageManager *langMgr = new LanguageManager(app, settings, locale);
    SysInfoManager *sysinfoMgr = new SysInfoManager();
    UpdateManager *updateMgr = new UpdateManager(app);

    TSHandler *tsHandler = new TSHandler(view);
    QThread *tsThread = new QThread();
    tsHandler->moveToThread(tsThread);
    QObject::connect(tsThread, &QThread::finished, tsHandler, &QObject::deleteLater);
    QObject::connect(tsThread, &QThread::started, tsHandler, &TSHandler::handle);
    tsThread->start();

    // Clean everything on shutdown
    QObject::connect(app, &QCoreApplication::aboutToQuit,
                     [tsHandler, tsThread, view, musicPlayer, settings, passMgr, langMgr, sysinfoMgr, updateMgr]() {
        qDebug() << "Cleaning up ...";

        tsThread->requestInterruption();
        tsThread->quit();
        tsThread->wait();
        tsThread->deleteLater();

        view->deleteLater();

        // Save settings
        musicPlayer->saveSettings();
        settings->sync();
        settings->deleteLater();

        passMgr->deleteLater();
        langMgr->deleteLater();
        sysinfoMgr->deleteLater();
        updateMgr->deleteLater();
        musicPlayer->deleteLater();
    });

    QQmlContext *context = view->rootContext();
    context->setContextProperty(QStringLiteral("musicPlayer"), musicPlayer);
    context->setContextProperty(QStringLiteral("mediaManager"), MediaManager::instance());
    context->setContextProperty(QStringLiteral("passwordManager"), passMgr);
    context->setContextProperty(QStringLiteral("languageManager"), langMgr);
    context->setContextProperty(QStringLiteral("sysinfoManager"), sysinfoMgr);
    context->setContextProperty(QStringLiteral("updateManager"), updateMgr);

    if(PasswordManager::passFileExists())
        view->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    else
        view->setSource(QUrl(QStringLiteral("qrc:/qml/main-firstboot.qml")));
}

int main(int argc, char *argv[])
{
    QGuiApplication *app = new QGuiApplication(argc, argv);
    QGuiApplication::setApplicationName(APPLICATION_NAME);
    QGuiApplication::setOrganizationName(APPLICATION_NAME);

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, Common::configDir());
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, Common::configDir());
    QSettings *settings = new QSettings();
    settings->setFallbacksEnabled(false);

    // Get locale
    const QString locale = settings->value(settingsLocaleStr, "fr").toString();
    settings->setValue(settingsLocaleStr, locale);

    // Load translations
    QTranslator appTranslator;
    appTranslator.load(QString(APPLICATION_TARGET "_") + locale, QCoreApplication::applicationDirPath() + QStringLiteral("/translations"));
    app->installTranslator(&appTranslator);

    QQuickView *view = new QQuickView();
    view->connect(view->engine(), &QQmlEngine::quit, app, &QGuiApplication::quit);
    view->setResizeMode(QQuickView::SizeViewToRootObject);

    QObject::connect(app, &QGuiApplication::applicationStateChanged, [app, view, settings, &locale](Qt::ApplicationState state) {
        if(state == Qt::ApplicationActive) {
            QObject::disconnect(app, &QGuiApplication::applicationStateChanged, 0, 0);
            afterSplashScreen(app, view, settings, locale);
        }
    });

    view->setSource(QStringLiteral("qrc:/qml/Splash.qml"));

#ifdef READY_FOR_CARSYSTEM
    view->showFullScreen();
#else
    view->show();
#endif

    // Launch app
    const int resultCode = app->exec();
    delete app;
    return resultCode;
}
