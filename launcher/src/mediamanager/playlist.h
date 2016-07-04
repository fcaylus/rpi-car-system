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

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>

#include "mediainfo.h"
#include "mediasource.h"

/*
 * Playlist objects act as a custom source, linked to other sources.
 * During a session, medias inside the playlist are identified by their mediaUri.
 * But, inside the playlist file, the media's identifier is used.
 */
class Playlist : public MediaSource
{
        Q_OBJECT

        Q_PROPERTY(QString fileName READ fileName)
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    public:
        Playlist(const QString& fileName, const QString& name, QObject *parent = nullptr);

        // Getters
        QString name() const;
        QString fileName() const;

        QString identifier();
        Type type();
        bool cacheSupported();

        // No media have a playlist as source, so this function should never be called
        // Always return an empty QVariant
        QVariant accessMediaMetadata(const QString& mediaUri, MediaInfo::MetadataType type);

        MediaInfoList availableMedias(MediaInfo::MetadataType onlyWithMeta = MediaInfo::UNKNOWN,
                                      QVariant onlyWithMetaValue = QVariant());

        // Not implemented since it's never used, always return an empty list
        QList<QVariantList> listMetadata(MetadataTypeList metadataToRetrieve,
                                         MediaInfo::MetadataType requiredMeta = MediaInfo::UNKNOWN,
                                         QVariant requiredMetaValue = QVariant());

        static QString playlistDirectory();
        static QList<Playlist *> findAllPlaylists(QObject *parent = nullptr);

    signals:

        void nameChanged();

    public slots:

        void setName(const QString& newName);

        void addMedia(MediaInfo *info);
        void removeMedia(MediaInfo *info);

    private:

        QString _name;
        QString _fileName;
};

#endif // PLAYLIST_H
