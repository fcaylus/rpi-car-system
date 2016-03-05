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

#include <ncurses.h>
#include <cstdlib>
#include <cstring>

#include <QDebug>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStorageInfo>

#include "decompressutil.h"

/**
 * An update archive is a .tar.xz file with the following structure:
 *
 *  - VERSION
 *  - sums.md5
 *  - symlinks.list
 *  - data/
 *      - etc/
 *      - lib/
 *      - opt/
 *      - sbin/
 *      - usr/
 *          - bin/
 *          - lib/
 *          - libexec/
 *          - qml/
 *          - sbin/
 *          - share/
 *
 *  sums.md5 contains the md5 sum of each file in the data dir.
 *  symlinks.list contains a list of all symlinks files (symlinks are not listed in the md5 list)
 *  data dir is entirely copied in the system root.
 *  If an update must modify any other directory, it must be specified as an hardware update.
 */

int applyUpdate(const QString& path);
int checkVersion(const QString& path);
int searchUpdates();

// Update functions
void printTextCentered(const char *text);
void showStatus(const char * text);
int showErrorStatus(const char * text, QString errorText);

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(QString(APPLICATION_VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription("RPI Car System updater program");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions({
        {"check-version", "Check version of the specified archive", "archive-path"}, // see returns code for more info
        {"apply-update", "Apply the update", "archive-path"},
        {"search-for-updates", "Search an update package in all connected USB devices"} // will print the path if found, else nothing
        });

    parser.process(app);

    if(parser.isSet("check-version"))
        return checkVersion(parser.value("check-version"));
    else if(parser.isSet("search-for-updates"))
        return searchUpdates();
    else if(parser.isSet("apply-update"))
        return applyUpdate(parser.value("apply-update"));
    else
    {
        qCritical() << "An option must be set !!!";
        parser.showHelp(UPDATER_CODE_NO_OPTION);
    }

    return UPDATER_CODE_COMMON_ERROR;
}

int checkVersion(const QString &path)
{
    qDebug() << "Checking version ...";

    QString errorText;
    const int retCode = DecompressUtil::checkVersion(path, &errorText);
    if(!errorText.isEmpty())
        qWarning() << qPrintable(errorText);
    else
        qWarning() << "Unknown error occured";

    return retCode;
}

// This function will only output files path to console
int searchUpdates()
{
    QStringList devicesList;
    DirUtility::listUSBDevices(&devicesList);

    bool found = false;

    for(QString devicePath : devicesList)
    {
        QDirIterator it(devicePath,
                        QStringList() << "*.tar.xz",
                        QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable,
                        QDirIterator::Subdirectories);

        while(it.hasNext())
        {
            it.next();

            // Check if it's an update package (search for VERSION and sums.md5 files)
            struct archive_entry *entry;
            struct archive *ar;
            struct archive *out;

            bool versionFound = false;
            bool md5SumsFound = false;
            bool symLinksFound = false;
            QString version;

            ar = archive_read_new();
            archive_read_support_filter_xz(ar);
            archive_read_support_format_tar(ar);

            out = archive_write_disk_new();
            archive_write_disk_set_standard_lookup(out);

            int ret = archive_read_open_filename(ar, it.filePath().toStdString().c_str(), 10240);

            if(ret != ARCHIVE_OK)
                continue;

            while(archive_read_next_header(ar, &entry) == ARCHIVE_OK)
            {
                if(std::strcmp(archive_entry_pathname(entry), "VERSION") == 0)
                {
                    // Extract VERSION file
                    archive_entry_set_pathname(entry, "/tmp/rpi-car-system-VERSION");
                    if(archive_write_header(out, entry) == ARCHIVE_OK)
                    {
                        if(DecompressUtilInternal::copyData(ar, out) < ARCHIVE_OK)
                            break;

                        if(archive_write_finish_entry(out) < ARCHIVE_OK)
                            break;

                        archive_write_close(out);
                        archive_write_free(out);

                        version = FileReader::readFile("/tmp/rpi-car-system-VERSION").trimmed();
                        QFile::remove("/tmp/rpi-car-system-VERSION");
                        versionFound = true;
                    }
                }
                else if(std::strcmp(archive_entry_pathname(entry), "sums.md5") == 0)
                    md5SumsFound = true;
                else if(std::strcmp(archive_entry_pathname(entry), "symlinks.list") == 0)
                    symLinksFound = true;
                else
                    archive_read_data_skip(ar);

                // It's an update package, output the version and the archive path
                if(versionFound && md5SumsFound && symLinksFound)
                {
                    // Use qInfo() since qDebug() is disabled on release builds
                    qInfo() << qPrintable(version + QLatin1Char(' ') + it.filePath());
                    found = true;
                    break;
                }
            }

            if(!versionFound)
            {
                archive_write_close(out);
                archive_write_free(out);
            }

            archive_read_close(ar);
            archive_read_free(ar);
        }
    }

    return found ? UPDATER_CODE_SUCCESS : UPDATER_CODE_COMMON_ERROR;
}

static bool useColors = false;

int applyUpdate(const QString &path)
{
    // Init ncurses
    initscr();
    attron(A_BOLD);

    // Init colors pair
    if(has_colors())
    {
        useColors = true;
        start_color();
        init_pair(1, COLOR_RED, COLOR_WHITE);
    }

    QString errorText;
    int ret;

    showStatus("Your system is going to be updated ...");
    sleep(1);
    showStatus("Do not remove your USB stick !");
    sleep(2);

    // Step 1
    showStatus("Checking update package version ...");
    ret = DecompressUtil::checkVersion(path, &errorText);
    if(ret != UPDATER_CODE_CHECKVER_OK)
        return showErrorStatus("Version check failed !", errorText);

    // Step 2
    showStatus("Checking available disk space ...");
    const qint64 availableSize = QStorageInfo::root().bytesAvailable();
    qint64 decompressedSize = 0;
    if(!DecompressUtil::decompressedSize(path, &decompressedSize, &errorText))
        return showErrorStatus("Checking size failed !", errorText);

    const qint64 archiveSize = QFileInfo(path).size();

    // Add a margin of 10Mo
    const qint64 neededSpace = archiveSize + decompressedSize + Q_UINT64_C(10000000);
    if(availableSize <= neededSpace)
        return showErrorStatus("Not enough space.", QString("You need at least %1 of free space.").arg(Common::bytesSizeToString(neededSpace)));

    // Step 3.1
    const QString outputDirPath = "/tmp/update_dir";
    QDir outputDir(outputDirPath);

    if(outputDir.exists())
    {
        showStatus("Removing previous update directory ...");
        outputDir.removeRecursively();
    }

    // Step 3.2
    showStatus("Extracting update package ...");
    outputDir.mkpath(".");
    if(!DecompressUtil::decompress(path, outputDirPath, &errorText))
        return showErrorStatus("Decompression error !", errorText);

    QStringList dirsToUpdate = {
        "/bin",
        "/etc",
        "/lib",
        "/opt",
        "/sbin",
        "/usr/bin",
        "/usr/lib",
        "/usr/libexec",
        "/usr/qml",
        "/usr/sbin",
        "/usr/share"
    };

    // Step 4 (the most critical)
    // If an error occured, there is no way to recover the system
    for(QString currentDir : dirsToUpdate)
    {
        showStatus(QString(QLatin1String("Removing local dir: ") + currentDir).toStdString().c_str());
        // Only compile on car system since root dirs are modified
#ifdef READY_FOR_CARSYSTEM
        if(!DirUtility::removeRecursively(currentDir))
            return showErrorStatus(QString(QStringLiteral("Can't remove local dir: ") + currentDir).toStdString().c_str(),
                                   "Your system is surely bricked !");
#endif

        showStatus(QString(QLatin1String("Copying new dir: ") + currentDir).toStdString().c_str());
#ifdef READY_FOR_CARSYSTEM
        if(!DirUtility::copyRecursively(outputDirPath + QLatin1String("/data") + currentDir, currentDir))
            return showErrorStatus(QString(QStringLiteral("Can't copy update dir: ") + currentDir).toStdString().c_str(),
                                   "Your system is surely bricked !");
#endif

    }

    // Step 5
    showStatus("Remove unnecessary update files ...");
    QDir(outputDirPath).removeRecursively();

    showStatus("Update finished !");
    sleep(2);
    showStatus("Your system will be reboot soon ...");
    sleep(2);

    endwin();
    return UPDATER_CODE_SUCCESS;
}

void printTextCentered(const char* text, int lineOffset)
{
    mvprintw(LINES/2 + lineOffset, (COLS-strlen(text))/2, "%s", text);
    move(LINES-1, COLS-1);
    refresh();
}

void showStatus(const char* text)
{
    printTextCentered(text, 0);
}

int showErrorStatus(const char *text, QString errorText)
{
    if(useColors)
        attron(COLOR_PAIR(1));

    printTextCentered(text, 0);

    if(useColors)
        attroff(COLOR_PAIR(1));

    printTextCentered(errorText.toStdString().c_str(), 3);

    sleep(10);
    endwin();
    return UPDATER_CODE_COMMON_ERROR;
}

