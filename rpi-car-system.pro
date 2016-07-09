#############################################################################
##
## This file is part of RPI Car System
## Copyright (c) 2016 Fabien Caylus <tranqyll.dev@gmail.com>
##
## This file is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This file is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##
#############################################################################

!unix|macx {
    error(You are not in a linux system !)
}

lessThan(QT_MAJOR_VERSION, 5)|lessThan(QT_MINOR_VERSION, 6) {
    error(Minimum Qt version is 5.6.0, please update your package.)
}

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    launcher \
    updater

