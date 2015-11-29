/*
 * This file is part of RPI Car System.
 * Copyright (c) 2015 Fabien Caylus <toutjuste13@gmail.com>
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

#ifndef COMMON
#define COMMON

#include <QString>
#include <QCoreApplication>

namespace Common {
    static inline QString configDir()
    {
#ifdef READY_FOR_CARSYSTEM
        return QStringLiteral("/home/pi/config")
#else
        return QCoreApplication::applicationDirPath();
#endif
    }

    static inline QString musicDir()
    {
#ifdef READY_FOR_CARSYSTEM
        return QStringLiteral("/home/pi/music");
#else
        //return QStringLiteral("/media/fabien/Disque Multimedia/Music/Ma musique");
        return QStringLiteral("/home/fabien/Musique");
#endif
    }
}

#endif // COMMON

