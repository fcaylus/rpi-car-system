#include <QGuiApplication>
#include <QtQml>

#include "soundmanager.h"
#include "passwordmanager.h"

#include <VLCQtCore/Common.h>

int main(int argc, char *argv[])
{
    // Set input module
    //qputenv("QT_IM_MODULE", QByteArray("mockup"));

    QGuiApplication app(argc, argv);
    //app.addLibraryPath(app.applicationDirPath() + QStringLiteral("/plugins"));
    //app.addLibraryPath(app.applicationDirPath() + QStringLiteral("/plugins/platforminputcontexts"));

    qmlRegisterType<MediaInfo>();

    VlcCommon::setPluginPath(app.applicationDirPath() + QStringLiteral("/plugins"));

    SoundManager *soundMgr = new SoundManager();
    PasswordManager *passMgr = new PasswordManager();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("soundManager"), soundMgr);
    engine.rootContext()->setContextProperty(QStringLiteral("passwordManager"), passMgr);
    engine.rootContext()->setContextProperty(QStringLiteral("isPassFileCreated"), QVariant(PasswordManager::isPassFileExists()));
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    const int result = app.exec();
    soundMgr->deleteLater();
    return result;
}
