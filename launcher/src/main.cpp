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

#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <QLibraryInfo>
#include <QUrl>
#include <QQuickView>
#include <QTranslator>
#include <QSettings>
#include <QDir>

#include "devicesmanager.h"
#include "filereader.h"
#include "languagemanager.h"
#include "soundmanager.h"
#include "passwordmanager.h"
#include "sysinfomanager.h"
#include "updatemanager.h"

#include <VLCQtCore/Common.h>

#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <byteswap.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <errno.h>

#include <thread>

static const QString settingsLocaleStr = "locale";
static bool appRunning = true;

void handleTouchScreenInput(QObject *eventReceiver)
{
    QDir devDir("/dev");
    QStringList hidrawList = devDir.entryList(QStringList({"hidraw*"}), QDir::Files | QDir::System);
    for(QString hidrawPath : hidrawList)
    {
        int fd = open(QString("/dev/" + hidrawPath).toStdString().c_str(), O_RDWR);

        if(fd < 0)
        {
            qDebug() << strerror(errno);
            qDebug() << "Can't open hidraw device at:" << hidrawPath;
            continue;
        }

        struct hidraw_devinfo info;
        memset(&info, 0x0, sizeof(info));

        int ret = ioctl(fd, HIDIOCGRAWINFO, &info);
        if(ret < 0)
        {
            qDebug() << strerror(errno);
            close(fd);
            continue;
        }

        // Check product and vendor information
        // Only handle this device:
        // http://www.waveshare.com/wiki/7inch_HDMI_LCD_(B)

        //qDebug() << info.vendor << info.product;

        if(info.vendor == QString("0x0EEF").toInt(0, 16)
           && info.product == QString("0x0005").toInt(0, 16))
        {
            // Handle this device
            qDebug() << "Found a touchscreen";
            bool lastPressed = false;

            uint8_t buf[30];
            memset(buf, 0x0, sizeof(buf));

            while(appRunning)
            {
                ret = read(fd, buf, 25);

                // Messages format: [tag] [pressed] [x] [y]
                // sizes in Bytes:    1       1      2   2
                if(ret < 6)
                {
                    if(ret == -1)
                        qWarning() << "TS: Error while reading:" << strerror(errno);
                    else if(ret == 0)
                        qWarning() << "TS: End of file";
                    else
                        qWarning() << "TS: Read only" << ret << "bytes";

                    continue;
                }

                // false == 0 and true == 1
                bool pressed = buf[1] != 0;
                uint16_t x = ((uint16_t)buf[2] << 8) | buf[3];
                uint16_t y = ((uint16_t)buf[4] << 8) | buf[5];

                // Messages are in Big Endian encoding, so toggle endianness of x and y
                // if running on rpi (ARM is little endian)
//#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
                //x = bswap_16(x);
                //y = bswap_16(y);
//#endif

                if(pressed && !lastPressed)
                {
                    qDebug() << "TS pressed at: " << QPoint(x,y);
                    QCoreApplication::postEvent(eventReceiver,
                                                new QMouseEvent(QEvent::MouseButtonPress,
                                                                QPoint(x,y),
                                                                Qt::LeftButton,
                                                                Qt::LeftButton,
                                                                Qt::NoModifier));
                    lastPressed = true;
                }
                else if(lastPressed && !pressed)
                {
                    qDebug() << "TS released at: " << QPoint(x,y);
                    QCoreApplication::postEvent(eventReceiver,
                                                new QMouseEvent(QEvent::MouseButtonRelease,
                                                                QPoint(x,y),
                                                                Qt::LeftButton,
                                                                Qt::LeftButton,
                                                                Qt::NoModifier));
                    lastPressed = false;
                }
            }

            qDebug() << "Exit TS loop ...";

            close(fd);
            return;
        }

        close(fd);
        continue;
    }

    qWarning() << "No touch screen found !";
}

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

    VlcCommon::setPluginPath(appDirPath + QStringLiteral("/plugins"));

    SoundManager *soundMgr = new SoundManager(settings);
    PasswordManager *passMgr = new PasswordManager();
    LanguageManager *langMgr = new LanguageManager(app, settings, locale);
    FileReader *fileReader = new FileReader();
    DevicesManager *devicesMgr = new DevicesManager();
    SysInfoManager *sysinfoMgr = new SysInfoManager();
    UpdateManager *updateMgr = new UpdateManager(app);

    QQuickView *view = new QQuickView();
    view->connect(view->engine(), &QQmlEngine::quit, app, &QGuiApplication::quit);
    view->setResizeMode(QQuickView::SizeViewToRootObject);

    QQmlContext *context = view->rootContext();
    context->setContextProperty(QStringLiteral("soundManager"), soundMgr);
    context->setContextProperty(QStringLiteral("passwordManager"), passMgr);
    context->setContextProperty(QStringLiteral("languageManager"), langMgr);
    context->setContextProperty(QStringLiteral("devicesManager"), devicesMgr);
    context->setContextProperty(QStringLiteral("fileReader"), fileReader);
    context->setContextProperty(QStringLiteral("sysinfoManager"), sysinfoMgr);
    context->setContextProperty(QStringLiteral("updateManager"), updateMgr);

    context->setContextProperty(QStringLiteral("passFileCreated"), QVariant(PasswordManager::passFileExists()));
    context->setContextProperty(QStringLiteral("programVersion"), QVariant(QString(APPLICATION_VERSION)));
    context->setContextProperty(QStringLiteral("hardwareVersion"), QVariant(HARDWARE_VERSION));
    context->setContextProperty(QStringLiteral("vlcVersion"), QVariant(VlcInstance::version()));
    context->setContextProperty(QStringLiteral("vlcqtVersion"), QVariant(VlcInstance::libVersion()));
    context->setContextProperty(QStringLiteral("buildDate"), QVariant(QString(BUILD_DATE)));

    view->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));

#ifdef READY_FOR_CARSYSTEM
        view->showFullScreen();
#else
        view->show();
#endif

    // Launch Touch screen thread
    std::thread tsThread(handleTouchScreenInput, view);

    // Launch app
    const int resultCode = app->exec();
    appRunning = false;

    qDebug() << "Cleaning up ...";

    delete view;

    // Save settings
    soundMgr->saveSettings();

    settings->sync();
    delete settings;

    delete passMgr;
    delete langMgr;
    delete fileReader;
    delete devicesMgr;
    delete sysinfoMgr;
    delete updateMgr;

    delete soundMgr;

    delete app;

    tsThread.join();

    return resultCode;
}
