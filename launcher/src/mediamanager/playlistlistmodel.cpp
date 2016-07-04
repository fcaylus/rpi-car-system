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

#include "playlistlistmodel.h"
#include "mediamanager.h"
#include "playlist.h"

#include <algorithm>

PlaylistListModel::PlaylistListModel(QObject *parent) : QAbstractListModel(parent)
{
    connect(MediaManager::instance(), &MediaManager::availablePlaylistsChanged, this, &PlaylistListModel::update);
}

int PlaylistListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _list.count();
}

QVariant PlaylistListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() == _list.count() - 1)
    {
        if(role == Qt::UserRole + 1)
            return tr("New Playlist");
        else if(role == Qt::UserRole + 2)
            return QLatin1String("%%new%%");
        else if(role == Qt::UserRole + 3)
            return false;

        return QVariant();
    }

    if(index.row() < 0 || index.row() >= _list.count() - 1)
        return QVariant();

    if(role == Qt::UserRole + 1)
        return _list.at(index.row())->name();
    else if(role == Qt::UserRole + 2)
        return _list.at(index.row())->fileName();
    else if(role == Qt::UserRole + 3)
        return true;

    return QVariant();
}

QHash<int, QByteArray> PlaylistListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole + 1] = "name";
    roles[Qt::UserRole + 2] = "fileName";
    roles[Qt::UserRole + 3] = "isRealPlaylist";
    return roles;
}

void PlaylistListModel::update()
{
    beginResetModel();
    _list.clear();
    endResetModel();

    QList<Playlist *> newList = MediaManager::instance()->availablePlaylists();
    newList.append(nullptr);

    // The last item allow user to add a new playlist
    beginInsertRows(QModelIndex(), 0, newList.size() - 1);
    _list = newList;

    std::sort(_list.begin(), _list.end()-1, [](Playlist *p1, Playlist *p2){
        if(p1 && p2)
            return QString::localeAwareCompare(p1->name(), p2->name()) < 0;
        return true;
    });

    endInsertRows();
}

