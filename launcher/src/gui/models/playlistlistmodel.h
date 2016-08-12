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

#ifndef PLAYLISTLISTMODEL_H
#define PLAYLISTLISTMODEL_H

#include <QAbstractListModel>
#include <QList>

class Playlist;

// Expose the list of available playlists to QML
class PlaylistListModel : public QAbstractListModel
{
        Q_OBJECT

    public:

        PlaylistListModel(QObject *parent = nullptr);

        int rowCount(const QModelIndex &parent) const override;
        QVariant data(const QModelIndex &index, int role) const override;
        QHash<int, QByteArray> roleNames() const override;

    public slots:

        void update();

    private:

        QList<Playlist *> _list;
};

#endif // PLAYLISTLISTMODEL_H
