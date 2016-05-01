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

#ifndef UPDATERCODES_H
#define UPDATERCODES_H

// Common
#define UPDATER_CODE_NO_OPTION 10
#define UPDATER_CODE_COMMON_ERROR 1
#define UPDATER_CODE_SUCCESS 0

// Check version
#define UPDATER_CODE_CHECKVER_OK 0
#define UPDATER_CODE_CHECKVER_BADHARDWARE 100
#define UPDATER_CODE_CHECKVER_OLDER 200
#define UPDATER_CODE_CHECKVER_NOFILE 300

#endif // UPDATERCODES_H

