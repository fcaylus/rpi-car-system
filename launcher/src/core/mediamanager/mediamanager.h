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

#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include <QObject>
#include <QThread>

#include "mediasource.h"

class Playlist;

// This class is a singleton
class MediaManager : public QObject
{
        Q_OBJECT

        Q_PROPERTY(MediaSourceList sourcesList READ sourcesList NOTIFY sourcesListChanged)
        Q_PROPERTY(bool scanned READ scanned NOTIFY scannedChanged)

        //
        // Changes made to the cache need a reboot
        Q_PROPERTY(bool cacheEnabled READ cacheEnabled WRITE setCacheEnabled)
        // Tell if the cache must be saved across sessions (after each boot)
        Q_PROPERTY(bool isCachePersistent READ isCachePersistent WRITE setCachePersistent)

    public:

        static MediaManager* instance();

        // Getters
        MediaSourceList sourcesList() const;

        // This function will store a copy of the list inside the manager
        MediaInfoList availableMedias(MediaInfo::MetadataType onlyWithMeta = MediaInfo::UNKNOWN,
                                      QVariant onlyWithMetaValue = QVariant());

        QList<QVariantList> listMetadata(MetadataTypeList metadataToRetrieve,
                                         MediaInfo::MetadataType requiredMeta = MediaInfo::UNKNOWN,
                                         QVariant requiredMetaValue = QVariant());

        // Return the last MediaInfoList get with availableMedias
        MediaInfoList lastMediaList() const;

        // False until the initial scan finished
        bool scanned() const;
        bool cacheEnabled() const;
        bool isCachePersistent() const;

        MediaInfo* findMediaFromUri(const QString& uri, const QString& sourceId = QString());
        MediaInfo* findMediaFromId(const QString& id, const QString& sourceId);
        Playlist* findPlaylistFromFileName(const QString& playlistFileName);
        MediaSource* findSourceFromId(const QString& identifier);

        QList<Playlist* > availablePlaylists();

    public slots:

        // Must be called only one time
        void initialScan();

        void setCacheEnabled(bool enabled);
        void setCachePersistent(bool persistent);

        //
        // Playlist stuff
        //

        void createPlaylist(const QString& name);
        void removePlaylist(const QString& playlistFileName);
        void addMediaToPlaylist(const QString& playlistFileName, const QString& mediaUri);
        void removeMediaFromPlaylist(const QString& playlistFileName, const QString& mediaUri);

    signals:
        void sourceAboutToBeDeleted(const QString& sourceIdentifier);
        void sourcesListChanged();
        void availableMediasChanged();
        void availablePlaylistsChanged();
        void scannedChanged();

    private:
        static MediaManager _instance;

        // Default constructor
        MediaManager();
        ~MediaManager();

        // Prevent calling initialScan() several times
        bool _init = false;
        MediaSourceList _sources;
        MediaInfoList _lastList;
        QList<Playlist *> _playlists;

        bool _cacheEnabled = true;
        bool _cachePersistent = false;

        QThread _hotplugThread;

        Q_DISABLE_COPY(MediaManager)
};

#endif // MEDIAMANAGER_H
