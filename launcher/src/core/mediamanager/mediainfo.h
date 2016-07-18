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

#ifndef MEDIAINFO_H
#define MEDIAINFO_H

#include <QObject>
#include <QString>
#include <QList>
#include <QVariant>
#include <QStringList>

class MediaSource;

class MediaInfo: public QObject
{
        Q_OBJECT

        Q_PROPERTY(QString title READ title)
        Q_PROPERTY(QString artist READ artist)
        Q_PROPERTY(QString album READ album)
        Q_PROPERTY(QString coverUri READ coverUri)
        Q_PROPERTY(int trackNumber READ trackNumber)

        // The mediaUri is the canonical file resource id of the media.
        // It must be unique during the entire session and accross different sources (from boot to shutdown).
        // To have an identifier that remains unique across session, use the identifier property.
        Q_PROPERTY(QString mediaUri READ mediaUri)
        Q_PROPERTY(QString identifier READ identifier)

        Q_PROPERTY(bool isLocalFile READ isLocalFile)

    public:
        enum MetadataType {
            UNKNOWN = 0,
            // Return all available metadata in the order they are defined after
            ALL,
            MEDIA_URI,
            IDENTIFIER,
            ARTIST,
            ALBUM,
            TITLE,
            TRACK_NUMBER,
            COVER_URI
        };
        Q_ENUM(MetadataType)

        static QString defaultCoverPath();
        static QString defaultArtistName();
        static QString defaultAlbumName();
        static QStringList musicFilesFormats();

        // Sort by trackNumber, and then, by fileName
        static bool sort(MediaInfo *m1, MediaInfo *m2);
        // Sort by title
        static bool sortAlpha(MediaInfo *m1, MediaInfo *m2);

        // Ctor
        MediaInfo(MediaSource *source, const QString& mediaUri, QObject *parent = nullptr);

        // Getters
        // If cache is supported, return cached informations
        // Else, get the information from metadata()
        QString title() const;
        QString artist() const;
        QString album() const;
        int trackNumber() const;
        QString coverUri() const;

        QString mediaUri() const;
        QString identifier() const;

        bool isLocalFile() const;
        bool cacheEnabled() const;

        QVariant metadata(MetadataType type) const;

        QString sourceId() const;

    private:

        QString _mediaUri;
        MediaSource *_source;

        QString _cacheTitle;
        QString _cacheArtist;
        QString _cacheAlbum;
        int _cacheNumber = -1;
        QString _cacheCoverUri;
};

typedef QList<MediaInfo *> MediaInfoList;
typedef QList<MediaInfo::MetadataType> MetadataTypeList;

#endif // MEDIAINFO_H
