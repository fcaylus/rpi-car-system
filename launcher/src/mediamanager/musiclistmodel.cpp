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

#include "musiclistmodel.h"
#include "mediamanager.h"
#include "playlist.h"

#include <algorithm>

MusicListModel::MusicListModel(QObject *parent) : QAbstractListModel(parent)
{
    connect(MediaManager::instance(), &MediaManager::availableMediasChanged, this, &MusicListModel::update);
}

MediaInfo::MetadataType MusicListModel::requiredMeta() const
{
    return _meta;
}

QVariant MusicListModel::requiredMetaValue() const
{
    return _metaValue;
}

bool MusicListModel::fromPlaylist() const
{
    return _fromPlaylist;
}

QString MusicListModel::playlistFileName() const
{
    return _playlistPath;
}

void MusicListModel::setRequiredMeta(MediaInfo::MetadataType meta)
{
    _meta = meta;
    emit requiredMetaChanged();
}

void MusicListModel::setRequiredMetaValue(QVariant value)
{
    _metaValue = value;
    emit requiredMetaValueChanged();
}

void MusicListModel::setFromPlaylist(bool from)
{
    _fromPlaylist = from;
}

void MusicListModel::setPlaylistFileName(const QString &fileName)
{
    _playlistPath = fileName;
}

int MusicListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _list.count();
}

QVariant MusicListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= _list.count())
        return QVariant();

    if(role == Qt::UserRole + 1)
        return _fromPlaylist;
    else if(role == Qt::UserRole + 2)
        return _playlistPath;

    MediaInfo *info = _list.value(index.row());
    MediaInfo::MetadataType type = static_cast<MediaInfo::MetadataType>(role - Qt::UserRole - 2);

    if(type == MediaInfo::TITLE)
        return info->title();
    else if(type == MediaInfo::COVER_URI)
        return info->coverUri();
    else if(type == MediaInfo::MEDIA_URI)
        return info->mediaUri();

    return QVariant();
}

QHash<int, QByteArray> MusicListModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[Qt::UserRole + 2 + MediaInfo::TITLE] = "title";
    roles[Qt::UserRole + 2 + MediaInfo::COVER_URI] = "cover";
    roles[Qt::UserRole + 2 + MediaInfo::MEDIA_URI] = "mediaUri";
    roles[Qt::UserRole + 1] = "fromPlaylist";
    roles[Qt::UserRole + 2] = "playlistFile";

    return roles;
}

void MusicListModel::update()
{
    if(!_list.isEmpty())
    {
        // Reset the model
        beginResetModel();
        _list.clear();
        endResetModel();
    }

    MediaInfoList newList;
    if(_fromPlaylist)
    {
        Playlist* playlist = MediaManager::instance()->findPlaylistFromFileName(_playlistPath);
        if(playlist)
            newList = playlist->availableMedias();
    }
    else
    {
        newList = MediaManager::instance()->availableMedias(_meta, _metaValue);
    }

    if(newList.isEmpty())
        return;

    beginInsertRows(QModelIndex(), 0, newList.size() - 1);
    _list = newList;
    endInsertRows();
}



