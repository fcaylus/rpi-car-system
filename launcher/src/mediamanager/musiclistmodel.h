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

#ifndef MUSICLISTMODEL_H
#define MUSICLISTMODEL_H

#include <QAbstractListModel>

#include "mediainfo.h"

class MusicListModel : public QAbstractListModel
{
        Q_OBJECT

        Q_PROPERTY(MediaInfo::MetadataType requiredMeta READ requiredMeta WRITE setRequiredMeta NOTIFY requiredMetaChanged)
        Q_PROPERTY(QVariant requiredMetaValue READ requiredMetaValue WRITE setRequiredMetaValue NOTIFY requiredMetaValueChanged)
        // false by default
        Q_PROPERTY(bool fromPlaylist READ fromPlaylist WRITE setFromPlaylist)
        Q_PROPERTY(QString playlistFileName READ playlistFileName WRITE setPlaylistFileName)

    public:

        MusicListModel(QObject *parent = nullptr);

        // Getter
        MediaInfo::MetadataType requiredMeta() const;
        QVariant requiredMetaValue() const;

        bool fromPlaylist() const;
        QString playlistFileName() const;

        int rowCount(const QModelIndex &parent) const;
        QVariant data(const QModelIndex &index, int role) const;
        QHash<int, QByteArray> roleNames() const;

    signals:

        void requiredMetaChanged();
        void requiredMetaValueChanged();

    public slots:

        void setRequiredMeta(MediaInfo::MetadataType meta);
        void setRequiredMetaValue(QVariant value);

        void setFromPlaylist(bool from);
        void setPlaylistFileName(const QString& fileName);

        void update();

    private:

        MediaInfo::MetadataType _meta = MediaInfo::UNKNOWN;
        QVariant _metaValue;

        bool _fromPlaylist = false;
        QString _playlistPath;

        MediaInfoList _list;
};

#endif // MUSICLISTMODEL_H
