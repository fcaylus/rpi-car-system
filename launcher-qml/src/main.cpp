#include <QGuiApplication>
#include <QtQml>

#include "soundmanager.h"

#include <VLCQtCore/Common.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<MediaInfo>();

    VlcCommon::setPluginPath(app.applicationDirPath() + QStringLiteral("/plugins"));

    SoundManager *soundMgr = new SoundManager();
    std::thread searchThread = soundMgr->checkForNewMusicFilesThread();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("soundManager"), soundMgr);
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    const int result = app.exec();

    searchThread.join();
    soundMgr->deleteLater();
    return result;
}
