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

#ifndef TSHANDLER_H
#define TSHANDLER_H

#include <QObject>
#include <QQuickView>
#include <QMutex>

// Handle all touch screen events
class TSHandler : public QObject
{
        Q_OBJECT
    public:
        TSHandler(QQuickView *view, QObject *parent = 0);

    public slots:
        // Will do all the stuff
        void handle();

        void requestStop();

    private:
        QQuickView *_view;
        QMutex _mutex;

        // Contains file descriptor
        int _fd;

        bool _needToQuit = false;
        bool needToQuit();
};

#endif // TSHANDLER_H
