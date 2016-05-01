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

###############################################
# Project definition for musicindex-generator #
###############################################

TEMPLATE = app

QT += core
QT -= gui

# Default defines
APPLICATION_TARGET = musicindex-generator
APPLICATION_NAME = MusicIndexGenerator

include(../common.pri)

#
# Thirdparty stuff
#

# VlcQt
include(../thirdparty/vlc-qt.pri)

#
# Project sources
#

INCLUDEPATH += ../launcher/src/

SOURCES += \
    src/main.cpp \
    ../launcher/src/mediainfo.cpp

HEADERS += \
    ../launcher/src/mediainfo.h \
    ../launcher/src/common.h \
    ../launcher/src/dirutility.h
