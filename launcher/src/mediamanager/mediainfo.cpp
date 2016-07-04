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

#include "mediainfo.h"
#include "mediasource.h"
#include "mediamanager.h"

#include <QDebug>
#include <QFileInfo>

MediaInfo::MediaInfo(MediaSource *source, const QString &mediaUri, QObject *parent): QObject(parent)
{
    _mediaUri = mediaUri;
    _source = source;

    if(cacheEnabled())
    {
        QVariantList data = _source->accessMediaMetadata(_mediaUri, ALL).toList();

        _cacheArtist = data.at(2).toString();
        _cacheAlbum = data.at(3).toString();
        _cacheTitle = data.at(4).toString();
        _cacheNumber = data.at(5).toInt();
        _cacheCoverUri = data.at(6).toString();
    }
}

//
// Getters
//

QVariant MediaInfo::metadata(MetadataType type) const
{
    if(_source)
        return _source->accessMediaMetadata(_mediaUri, type);
    return QVariant();
}

QString MediaInfo::album() const
{
    if(!_cacheAlbum.isEmpty())
        return _cacheAlbum;
    return metadata(ALBUM).toString();
}

QString MediaInfo::artist() const
{
    if(!_cacheArtist.isEmpty())
        return _cacheArtist;
    return metadata(ARTIST).toString();
}

QString MediaInfo::title() const
{
    if(!_cacheTitle.isEmpty())
        return _cacheTitle;
    return metadata(TITLE).toString();
}

QString MediaInfo::coverUri() const
{
    if(!_cacheCoverUri.isEmpty())
        return _cacheCoverUri;
    return metadata(COVER_URI).toString();
}

int MediaInfo::trackNumber() const
{
    if(_cacheNumber != -1)
        return _cacheNumber;
    return metadata(TRACK_NUMBER).toInt();
}

QString MediaInfo::mediaUri() const
{
    return _mediaUri;
}

QString MediaInfo::identifier() const
{
    return metadata(IDENTIFIER).toString();
}

bool MediaInfo::isLocalFile() const
{
    return _source->type() == MediaSource::LocalFileSystem;
}

bool MediaInfo::cacheEnabled() const
{
    return _source && _source->cacheSupported() && MediaManager::instance()->cacheEnabled();
}

QString MediaInfo::sourceId() const
{
    return _source->identifier();
}

// Static
bool MediaInfo::sort(MediaInfo* m1, MediaInfo* m2)
{
    if(m1->trackNumber() == m2->trackNumber())
        return QString::localeAwareCompare(QFileInfo(m1->mediaUri()).baseName(), QFileInfo(m2->mediaUri()).baseName()) < 0;

    return m1->trackNumber() < m2->trackNumber();
}

// Static
bool MediaInfo::sortAlpha(MediaInfo* m1, MediaInfo* m2)
{
    return QString::localeAwareCompare(m1->title(), m2->title()) < 0;
}

// Static
QString MediaInfo::defaultAlbumName()
{
    return tr("Unknown Album");
}

// Static
QString MediaInfo::defaultArtistName()
{
    return tr("Unknown Artist");
}

// Static
QString MediaInfo::defaultCoverPath()
{
    return QStringLiteral("qrc:/images/default_artwork");
}

// Static
QStringList MediaInfo::musicFilesFormats()
{
    // From: https://en.wikipedia.org/wiki/Audio_file_format
    return QStringList({QStringLiteral("*.mp3"), QStringLiteral("*.m4a"), QStringLiteral("*.m4p"), QStringLiteral("*.ogg"),
                        QStringLiteral("*.wav"), QStringLiteral("*.wma"), QStringLiteral("*.gsm"), QStringLiteral("*.amr"),
                        QStringLiteral("*.3gp"), QStringLiteral("*.mpc"), QStringLiteral("*.aac"), QStringLiteral("*.oga"),
                        QStringLiteral("*.tta"), QStringLiteral("*.vox"), QStringLiteral("*.ape"), QStringLiteral("*.awb"),
                        QStringLiteral("*.aiff"), QStringLiteral("*.flac"), QStringLiteral("*.opus"), QStringLiteral("*.webm"),
                        QStringLiteral("*.au"), QStringLiteral("*.wv")});
}
