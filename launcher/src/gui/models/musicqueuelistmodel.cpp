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

#include "musicqueuelistmodel.h"

#include <QDebug>

MusicQueueListModel::MusicQueueListModel(QObject *parent) : QAbstractListModel(parent)
{
}

MusicPlayer* MusicQueueListModel::player() const
{
    return _player;
}

void MusicQueueListModel::setPlayer(MusicPlayer *player)
{
    _player = player;
}

int MusicQueueListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _list.size();
}

QVariant MusicQueueListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= _list.size())
        return QVariant();

    if(role == Qt::UserRole + 1)
        return index.row();

    MediaInfo *info = _list.value(index.row());
    MediaInfo::MetadataType type = static_cast<MediaInfo::MetadataType>(role - Qt::UserRole - 1);

    if(type == MediaInfo::TITLE)
        return info->title();
    else if(type == MediaInfo::COVER_URI)
        return info->coverUri();

    return QVariant();
}

QHash<int, QByteArray> MusicQueueListModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[Qt::UserRole + 1 + MediaInfo::TITLE] = "title";
    roles[Qt::UserRole + 1 + MediaInfo::COVER_URI] = "cover";
    roles[Qt::UserRole + 1] = "index";

    return roles;
}

void MusicQueueListModel::update()
{
    if(!_list.isEmpty())
    {
        beginResetModel();
        _list.clear();
        endResetModel();
    }

    if(_player)
    {
        MediaInfoList newList = _player->mediaQueue();
        beginInsertRows(QModelIndex(), 0, newList.size() - 1);
        _list = newList;
        endInsertRows();
    }
}
