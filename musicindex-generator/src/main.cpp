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

#include "mediainfo.h"
#include "dirutility.h"
#include "common.h"

#include <QXmlStreamWriter>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include <QSet>
#include <QFileInfo>
#include <QDirIterator>
#include <QCoreApplication>

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>

#define EXIT_CODE_ERROR 1
#define EXIT_CODE_SUCCESS 0

// Do all the stuff
int checkForNewMusicFiles(VlcInstance *instance);
// Re-create the entire index file
int recreateMusicIndex(VlcInstance *instance, MediaList &mediaList);
// Just read form the index
int updateMediaList(MediaList& mediaList);
// Create the xml files (with albums and artists maps)
int createMediaMapsFromList(MediaList& mediaList);

static QString _indexFilePath;
static QString _albumMapFilePath;
static QString _artistMapFilePath;
static QString _trackListFilePath;

static QStringList _musicFileFormats;

int main(int argc, char** argv)
{
    // Only used for application path
    QCoreApplication app(argc, argv);
    Q_UNUSED(app)

    _musicFileFormats = MediaInfo::musicFilesFormats();

    _indexFilePath = MediaInfo::musicIndexFilePath();
    _albumMapFilePath = MediaInfo::albumMapFilePath();
    _artistMapFilePath = MediaInfo::artistMapFilePath();
    _trackListFilePath = MediaInfo::trackListFilePath();

    VlcInstance *instance = new VlcInstance(VlcCommon::args(), nullptr);
    const int result = checkForNewMusicFiles(instance);
    instance->deleteLater();
    return result;
}

int checkForNewMusicFiles(VlcInstance *instance)
{
    MediaList mediaList;

    // If the index file doesn't exist, create it
    if(!QFile::exists(_indexFilePath))
    {
        qDebug() << "Create index file !";
        recreateMusicIndex(instance, mediaList);

        while(!mediaList.isEmpty())
            delete mediaList.takeLast();

        return EXIT_CODE_SUCCESS;
    }

    // Here, the index file exist, but maybe it's not up to date
    // Refresh it !

    QFile indexFile(_indexFilePath);
    if(!indexFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "Cannot open the index file at:" << _indexFilePath;
        return EXIT_CODE_ERROR;
    }
    QTextStream in(&indexFile);

    const QString indexFileNewPath = _indexFilePath + QStringLiteral(".new");
    QFile indexFileNew(indexFileNewPath);
    if(!indexFileNew.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qCritical() << "Cannot open a temp index file at:" << indexFileNewPath;
        return EXIT_CODE_ERROR;
    }
    QTextStream out(&indexFileNew);


    // Check first line
    QString line = in.readLine();
    if(line != QStringLiteral("v2"))
    {
        qDebug() << "Index file is not in the correct version, recreate it !";

        indexFile.close();
        indexFileNew.close();
        indexFile.remove();
        indexFileNew.remove();

        recreateMusicIndex(instance, mediaList);

        while(!mediaList.isEmpty())
            delete mediaList.takeLast();

        return EXIT_CODE_SUCCESS;
    }
    out << line << "\n";

    //
    // First step, parse each line in the index to find files to remove and files to update
    //

    bool changeOccur = false; // Used to know if an update, delete or add operation has occured

    QString currentFilePath;
    QSet<QString> filesList;

    bool needToRemove = false;
    bool needToUpdate = false;

    int varCount = 0; // Used to check if the media have the correct number of argument
    while(!in.atEnd())
    {
        line = in.readLine();

        if(!line.isEmpty())
        {
            varCount += 1;
            if(varCount == 1)
            {
                currentFilePath = line;
                if(!QFile::exists(currentFilePath))
                {
                    needToRemove = true;
                    changeOccur = true;
                }
                else
                    filesList.insert(currentFilePath);
            }
            else if(varCount == 2 && !needToRemove)
            {
                const long size = line.toLong();
                const long realSize = QFileInfo(currentFilePath).size();
                if(size != realSize)
                {
                    // Need to update all values
                    needToUpdate = true;
                    changeOccur = true;

                    VlcMedia *media = new VlcMedia(currentFilePath, true, instance);
                    MediaInfo *info = new MediaInfo(media, currentFilePath);

                    out << realSize << "\n"
                        << info->title() << "\n"
                        << info->artist() << "\n"
                        << info->album() << "\n"
                        << info->trackNumber() << "\n"
                        << info->coverFile() << "\n";

                    delete info;
                    delete media;
                }
            }

            if(!needToRemove && !needToUpdate)
                out << line << "\n";
        }
        else
        {
            // We reach the end of the media
            if(!needToRemove)
                out << "\n";

            needToRemove = false;
            needToUpdate = false;
            varCount = 0;
        }
    }
    indexFile.flush();
    indexFile.close();

    //
    // Second step: list all files and check if some are missing from the index
    //

    QDirIterator it(Common::musicDir(), _musicFileFormats, QDir::Files | QDir::Readable | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        QString path = it.next();
        // If not in the index file
        if(!filesList.contains(path))
        {
            // Append the file to the index
            VlcMedia *media = new VlcMedia(path, true, instance);
            MediaInfo *info = new MediaInfo(media, path);

            out << path << "\n"
                << info->fileSize() << "\n"
                << info->title() << "\n"
                << info->artist() << "\n"
                << info->album() << "\n"
                << info->trackNumber() << "\n"
                << info->coverFile() << "\n\n";

            delete info;
            delete media;

            changeOccur = true;
        }
    }

    indexFileNew.flush();
    indexFileNew.close();

    // Remove the old file and copy the new one
    indexFile.remove();
    indexFileNew.rename(_indexFilePath);

    // Send the media list after update (only if there are changes)
    if(changeOccur)
    {
        qDebug() << "Some media has changed ! Update xml files ...";
        if(updateMediaList(mediaList) != EXIT_CODE_SUCCESS)
            return EXIT_CODE_ERROR;
        if(createMediaMapsFromList(mediaList) != EXIT_CODE_SUCCESS)
            return EXIT_CODE_ERROR;

        qDebug() << "Xml files have been updated !";
    }

    // Now delete the media list
    while(!mediaList.isEmpty())
        delete mediaList.takeLast();

    return changeOccur ? EXIT_CODE_SUCCESS : EXIT_CODE_ERROR;
}

int recreateMusicIndex(VlcInstance *instance, MediaList &mediaList)
{
    qDebug() << _indexFilePath;
    QFile indexFile(_indexFilePath);
    if(!indexFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << "Cannot open index file at:" << _indexFilePath;
        return EXIT_CODE_ERROR;
    }
    QTextStream out(&indexFile);

    // Output the version
    out << "v2\n";

    QDirIterator it(Common::musicDir(), _musicFileFormats, QDir::Files | QDir::Readable | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        QString path = it.next();

        //
        // Structure of each media in the index file
        // (blank lines separate the media):
        //
        //    <filePath>
        //    <fileSize>  (used to check if the file has changed)
        //    <musicTitle>
        //    <musicArtist>
        //    <musicAlbum>
        //    <trackNumber>
        //    <coverFilePath>
        //

        VlcMedia *media = new VlcMedia(path, true, instance);
        MediaInfo *info = new MediaInfo(media, path);
        delete media;
        mediaList.append(info);

        out << path << "\n"
            << info->fileSize() << "\n"
            << info->title() << "\n"
            << info->artist() << "\n"
            << info->album() << "\n"
            << info->trackNumber() << "\n"
            << info->coverFile() << "\n\n";
    }

    out.flush();
    indexFile.close();

    return createMediaMapsFromList(mediaList);
}

int updateMediaList(MediaList &mediaList)
{
    qDebug() << "Update internal media list ...";

    QFile indexFile(_indexFilePath);
    if(!indexFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "Cannot open the index file at:" << _indexFilePath;
        return EXIT_CODE_ERROR;
    }
    QTextStream in(&indexFile);

    // Check first line
    QString line = in.readLine();
    if(line != QStringLiteral("v2"))
    {
        indexFile.close();
        qCritical() << "Index file is in bad version (" << line << " instead of v2)";
        return EXIT_CODE_ERROR;
    }

    QString fileName;
    QString artist;
    QString album;
    QString title;
    QString coverPath;
    int trackNumber = 0;
    long fileSize = -1;

    int varCount = 0;
    while(!in.atEnd())
    {
        line = in.readLine();
        if(!line.isEmpty())
        {
            varCount += 1;
            if(varCount == 1)
                fileName = line;
            else if(varCount == 2)
                fileSize =line.toLong();
            else if(varCount == 3)
                title = line;
            else if(varCount == 4)
                artist = line;
            else if(varCount == 5)
                album = line;
            else if(varCount == 6)
                trackNumber = line.toInt();
            else if(varCount == 7)
                coverPath = line;
        }
        else if(!fileName.isEmpty() && !artist.isEmpty()
                && !album.isEmpty() && !title.isEmpty()
                && !coverPath.isEmpty() && fileSize != -1
                && varCount == 7)
        {
            varCount = 0;

            MediaInfo *info = new MediaInfo();
            info->setAlbum(album);
            info->setArtist(artist);
            info->setCoverFile(coverPath);
            info->setFilePath(fileName);
            info->setTrackNumber(trackNumber);
            info->setFileSize(fileSize);
            info->setTitle(title);

            mediaList.append(info);

            // Reset vars
            fileName = "";
            fileSize = -1;
            title = "";
            artist = "";
            album = "";
            coverPath = "";
            trackNumber = 0;
        }
    }

    indexFile.close();
    return EXIT_CODE_SUCCESS;
}

int createMediaMapsFromList(MediaList &mediaList)
{
    qDebug() << "Create xml maps ...";
    SortedMediaMap artistMediaMap;
    MediaMap albumMediaMap;

    MediaMap artistMapTemp;

    //
    // First step, list all artist and all albums

    for(MediaInfo *info : mediaList)
    {
        // Not yet in the list
        if(!albumMediaMap.contains(info->album()))
        {
            MediaList trackList;
            trackList.append(info);
            albumMediaMap[info->album()] = trackList;
        }
        else
            albumMediaMap[info->album()].append(info);

        if(!artistMapTemp.contains(info->artist()))
        {
            MediaList trackList;
            trackList.append(info);
            artistMapTemp[info->artist()] = trackList;
        }
        else
            artistMapTemp[info->artist()].append(info);
    }

    //
    // Second step, sort albums in artist map

    for(MediaMap::iterator it = artistMapTemp.begin(); it != artistMapTemp.end(); ++it)
    {
        MediaMap tempAlbumMap;

        for(MediaInfo *info : it.value())
        {
            if(!tempAlbumMap.contains(info->album()))
            {
                MediaList trackList;
                trackList.append(info);
                tempAlbumMap[info->album()] = trackList;
            }
            else
                tempAlbumMap[info->album()].append(info);
        }

        // Sort the map
        for(MediaMap::iterator it2 = tempAlbumMap.begin(); it2 != tempAlbumMap.end(); ++it2)
            std::sort(it2.value().begin(), it2.value().end(), MediaInfo::sort);

        // Copy
        artistMediaMap[it.key()] = tempAlbumMap;
    }

    //
    // Third step, order tracks in album list
    for(MediaMap::iterator it = albumMediaMap.begin(); it != albumMediaMap.end(); ++it)
        std::sort(it.value().begin(), it.value().end(), MediaInfo::sort);

    //
    // Last step, order media list
    std::sort(mediaList.begin(), mediaList.end(), MediaInfo::sortAlpha);


    //
    // Here, all maps are created, we just have to write them to files
    //

    QFile albumMapFile(_albumMapFilePath + QStringLiteral(".new"));
    if(!albumMapFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << albumMapFile.errorString();
        return EXIT_CODE_ERROR;
    }

    QFile artistMapFile(_artistMapFilePath + QStringLiteral(".new"));
    if(!artistMapFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << artistMapFile.errorString();
        return EXIT_CODE_ERROR;
    }

    QFile trackListFile(_trackListFilePath + QStringLiteral(".new"));
    if(!trackListFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << trackListFile.errorString();
        return EXIT_CODE_ERROR;
    }

    QXmlStreamWriter streamAlbum(&albumMapFile);
    QXmlStreamWriter streamArtist(&artistMapFile);
    QXmlStreamWriter streamTrack(&trackListFile);

#ifdef QT_DEBUG
    streamAlbum.setAutoFormatting(true);
    streamArtist.setAutoFormatting(true);
    streamTrack.setAutoFormatting(true);
#endif

    //
    // Write album

    streamAlbum.writeStartDocument();
    streamAlbum.writeStartElement(QStringLiteral("albums"));

    for(MediaMap::iterator it = albumMediaMap.begin(); it != albumMediaMap.end(); ++it)
    {
        streamAlbum.writeStartElement(QStringLiteral("album"));
        streamAlbum.writeAttribute(QStringLiteral("name"), it.key());
        // Use the first cover file as album cover
        streamAlbum.writeAttribute(QStringLiteral("cover"), it.value().first()->coverFileUrl().toString());

        for(MediaInfo *info: it.value())
            info->toXml(&streamAlbum);

        streamAlbum.writeEndElement();
    }
    streamAlbum.writeEndElement();
    streamAlbum.writeEndDocument();
    albumMapFile.close();

    //
    // Write artist

    streamArtist.writeStartDocument();
    streamArtist.writeStartElement(QStringLiteral("artists"));

    for(SortedMediaMap::iterator it = artistMediaMap.begin(); it != artistMediaMap.end(); ++it)
    {
        streamArtist.writeStartElement(QStringLiteral("artist"));
        streamArtist.writeAttribute(QStringLiteral("name"), it.key());

        for(MediaMap::iterator it2 = it.value().begin(); it2 != it.value().end(); ++it2)
        {
            streamArtist.writeStartElement(QStringLiteral("album"));
            streamArtist.writeAttribute(QStringLiteral("name"), it2.key());

            streamArtist.writeAttribute(QStringLiteral("cover"), it2.value().first()->coverFileUrl().toString());

            for(MediaInfo *info: it2.value())
                info->toXml(&streamArtist);

            streamArtist.writeEndElement();
        }
        streamArtist.writeEndElement();
    }
    streamArtist.writeEndElement();
    streamArtist.writeEndDocument();
    artistMapFile.close();

    //
    // Write tracks

    streamTrack.writeStartDocument();
    streamTrack.writeStartElement(QStringLiteral("tracks"));

    for(MediaInfo *info: mediaList)
        info->toXml(&streamTrack);

    streamTrack.writeEndElement();
    streamTrack.writeEndDocument();
    trackListFile.close();

    //
    // Update real files
    //

    DirUtility::removeIfExists(_albumMapFilePath);
    QFile::rename(_albumMapFilePath + ".new", _albumMapFilePath);
    DirUtility::removeIfExists(_artistMapFilePath);
    QFile::rename(_artistMapFilePath + ".new", _artistMapFilePath);
    DirUtility::removeIfExists(_trackListFilePath);
    QFile::rename(_trackListFilePath + ".new", _trackListFilePath);

    return EXIT_CODE_SUCCESS;
}
