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

#ifndef MEDIASOURCE_H
#define MEDIASOURCE_H

#include <QVariant>
#include <QObject>

#include "mediainfo.h"

// Represent an abstract source of media.
// Currently only UsbSource is implemented.
// Scan for new media should be non-blocking and operated in an another thread
class MediaSource : public QObject
{
        Q_OBJECT

        Q_PROPERTY(Type type READ type)

    public:

        enum Type {
            NotSupported = 0,
            // USB sticks, SD cards, HDD directly connected to the system
            LocalFileSystem
        };
        Q_ENUM(Type)

        explicit MediaSource(QObject *parent = nullptr): QObject(parent) {}

        //
        // Need to be re-implemented by subclasses
        //

        // Must be unique for each source ( [source type]-[id] )
        virtual QString identifier() = 0;
        virtual Type type() = 0;
        // True if there are cache functionnalities.
        // ie: informations about the media are retrieved at the MediaInfo creation.
        virtual bool cacheSupported() = 0;
        // Must be re-implemented if cacheSupported set to true
        virtual void deleteCache() {}


        virtual QVariant accessMediaMetadata(const QString& mediaUri, MediaInfo::MetadataType type) = 0;

        // onlyWith args are used to select only media with the <onlyWithMeta> metadata
        // set to <onlyWithMetaValue>.
        // If one of this argument is default-constructed, both are ignored.
        virtual MediaInfoList availableMedias(MediaInfo::MetadataType onlyWithMeta = MediaInfo::UNKNOWN,
                                              QVariant onlyWithMetaValue = QVariant()) = 0;

        // This function is usefull for example if you want to get all albums of a specified artist.
        virtual QList<QVariantList> listMetadata(MetadataTypeList metadataToRetrieve,
                                                 MediaInfo::MetadataType requiredMeta = MediaInfo::UNKNOWN,
                                                 QVariant requiredMetaValue = QVariant()) = 0;

    signals:

        void newMediaAvailable();
        void disconnected();
};

typedef QList<MediaSource *> MediaSourceList;

#endif // MEDIASOURCE_H
