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
