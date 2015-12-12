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

#include "mediainfo.h"
#include "dirutility.h"
#include "common.h"

#include <VLCQtCore/MetaManager.h>

#include <QCoreApplication>
#include <QFileInfo>
#include <QXmlStreamWriter>

MediaInfo::MediaInfo()
{

}

MediaInfo::MediaInfo(VlcMedia *media, QString path)
{
    _filePath = path;
    if(_filePath.isEmpty())
        _filePath = DirUtility::filePathForURI(media->currentLocation());

    VlcMetaManager meta(media);

    _coverFile = DirUtility::filePathForURI(meta.artwork());
    _title = meta.title();
    _artist = meta.artist();
    _album = meta.album();
    _trackNumber = meta.number();
    _fileSize = QFileInfo(_filePath).size();

    if(_title.isEmpty())
        _title = _filePath;
    if(_artist.isEmpty())
        _artist = defaultArtistName();
    if(_album.isEmpty())
        _album = defaultAlbumName();
    if(_coverFile.isEmpty())
        _coverFile = defaultCoverPath();
}

// Statics
QString MediaInfo::defaultAlbumName()
{
    return tr("Unknown Album");
}

QString MediaInfo::defaultArtistName()
{
    return tr("Unknown Artist");
}

QString MediaInfo::defaultCoverPath()
{
    return QStringLiteral("qrc:/images/default_artwork");
}

// Getters

QString MediaInfo::album() const
{
    return _album;
}

QString MediaInfo::artist() const
{
    return _artist;
}

QString MediaInfo::title() const
{
    return _title;
}

QString MediaInfo::coverFile() const
{
    return _coverFile;
}

int MediaInfo::trackNumber() const
{
    return _trackNumber;
}

QString MediaInfo::filePath() const
{
    return _filePath;
}

long MediaInfo::fileSize() const
{
    return _fileSize;
}

QUrl MediaInfo::coverFileUrl()
{
    // Check if it's a resource image
    if(_coverFile.startsWith("qrc"))
        return QUrl(_coverFile);
    return QUrl::fromLocalFile(_coverFile);
}

// Setters
void MediaInfo::setTitle(const QString& str)
{
    _title = str;
}

void MediaInfo::setArtist(const QString& str)
{
    _artist = str;
}

void MediaInfo::setAlbum(const QString& str)
{
    _album = str;
}

void MediaInfo::setTrackNumber(int nb)
{
    _trackNumber = nb;
}

void MediaInfo::setCoverFile(const QString& str)
{
    _coverFile = str;
}

void MediaInfo::setFilePath(const QString& str)
{
    _filePath = str;
}

void MediaInfo::setFileSize(long size)
{
    _fileSize = size;
}

void MediaInfo::toXml(QXmlStreamWriter *stream)
{
    stream->writeStartElement(QStringLiteral("track"));
    stream->writeTextElement(QStringLiteral("title"), _title);
    stream->writeTextElement(QStringLiteral("path"), _filePath);
    stream->writeTextElement(QStringLiteral("cover"), coverFileUrl().toString());
    stream->writeTextElement(QStringLiteral("nb"), QString::number(_trackNumber));
    stream->writeEndElement();
}

//
// Static functions
//

bool MediaInfo::sort(MediaInfo* m1, MediaInfo* m2)
{
    if(m1->trackNumber() == m2->trackNumber())
        return QString::localeAwareCompare(QFileInfo(m1->filePath()).baseName(), QFileInfo(m2->filePath()).baseName()) < 0;

    return m1->trackNumber() < m2->trackNumber();
}

bool MediaInfo::sortAlpha(MediaInfo* m1, MediaInfo* m2)
{
    return QString::localeAwareCompare(m1->title(), m2->title()) < 0;
}

QString MediaInfo::albumMapFilePath()
{
    return Common::configDir() + QStringLiteral("/album-map.xml");
}

QString MediaInfo::artistMapFilePath()
{
    return Common::configDir() + QStringLiteral("/artist-map.xml");
}

QString MediaInfo::trackListFilePath()
{
    return Common::configDir() + QStringLiteral("/track-list.xml");
}

QString MediaInfo::musicIndexFilePath()
{
    return Common::configDir() + QStringLiteral("/music-index.txt");
}

QStringList MediaInfo::musicFilesFormats()
{
    // From: https://en.wikipedia.org/wiki/Audio_file_format
    return QStringList({QStringLiteral("*.mp3"), QStringLiteral("*.m4a"), QStringLiteral("*.m4p"), QStringLiteral("*.ogg"),
                        QStringLiteral("*.wav"), QStringLiteral("*.wma"), QStringLiteral("*.dct"), QStringLiteral("*.dss"),
                        QStringLiteral("*.act"), QStringLiteral("*.ivs"), QStringLiteral("*.gsm"), QStringLiteral("*.dvf"),
                        QStringLiteral("*.amr"), QStringLiteral("*.mmf"), QStringLiteral("*.3gp"), QStringLiteral("*.mpc"),
                        QStringLiteral("*.msv"), QStringLiteral("*.aac"), QStringLiteral("*.oga"), QStringLiteral("*.raw"),
                        QStringLiteral("*.sln"), QStringLiteral("*.tta"), QStringLiteral("*.vox"), QStringLiteral("*.ape"),
                        QStringLiteral("*.awb"), QStringLiteral("*.aiff"), QStringLiteral("*.flac"), QStringLiteral("*.opus"),
                        QStringLiteral("*.webm"), QStringLiteral("*.au"), QStringLiteral("*.ra"), QStringLiteral("*.rm"),
                        QStringLiteral("*.wv")});
}

