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

#ifndef MUSICQUEUELISTMODEL_H
#define MUSICQUEUELISTMODEL_H

#include <QAbstractListModel>

#include "../musicplayer.h"

// Used in QML by MusicQueuePopup.qml
class MusicQueueListModel : public QAbstractListModel
{
        Q_OBJECT

        Q_PROPERTY(MusicPlayer* player READ player WRITE setPlayer)

    public:

        MusicQueueListModel(QObject *parent = nullptr);

        // Getter
        MusicPlayer* player() const;

        int rowCount(const QModelIndex &parent) const;
        QVariant data(const QModelIndex &index, int role) const;
        QHash<int, QByteArray> roleNames() const;

    public slots:

        void setPlayer(MusicPlayer *player);
        void update();

    private:

        MusicPlayer *_player;
        MediaInfoList _list;
};

#endif // MUSICQUEUELISTMODEL_H
