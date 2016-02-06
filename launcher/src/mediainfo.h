/*
 * This file is part of RPI Car System.
 * Copyright (c) 2016 Fabien Caylus <toutjuste13@gmail.com>
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

#ifndef MEDIAINFO_H
#define MEDIAINFO_H

#include <QString>
#include <QObject>
#include <QMap>
#include <QList>
#include <QUrl>

#include <VLCQtCore/Media.h>

class QXmlStreamWriter;

class MediaInfo: public QObject
{
        Q_OBJECT

    public:
        MediaInfo();
        MediaInfo(VlcMedia *media, QString path = QString());

        // Sort by trackNumber, and then, by fileName
        static bool sort(MediaInfo *m1, MediaInfo *m2);

        // Sort by title (used by the tracks view)
        static bool sortAlpha(MediaInfo *m1, MediaInfo *m2);

        // Getters
        QString title() const;
        QString artist() const;
        QString album() const;
        int trackNumber() const;
        QString coverFile() const;

        QString filePath() const;
        long fileSize() const;

        QUrl coverFileUrl();

        void toXml(QXmlStreamWriter* stream);

        static QString defaultCoverPath();
        static QString defaultArtistName();
        static QString defaultAlbumName();

        static QString artistMapFilePath();
        static QString albumMapFilePath();
        static QString trackListFilePath();
        static QString musicIndexFilePath();

        static QString playlistsDirectory();

        static QStringList musicFilesFormats();

    public slots:
        // Setters
        void setTitle(const QString& str);
        void setArtist(const QString& str);
        void setAlbum(const QString& str);
        void setTrackNumber(int nb);
        void setCoverFile(const QString& str);

        void setFilePath(const QString& str);
        void setFileSize(long size);

    private:

        QString _title;
        QString _artist;
        QString _album;
        int _trackNumber = 0;
        QString _coverFile;

        QString _filePath;
        long _fileSize = -1;
};

typedef QList<MediaInfo *> MediaList;
typedef QMap<QString, MediaList> MediaMap;
// For examples:
//      artists -> albums -> trakcs
//      albums -> artists -> tracks
typedef QMap<QString, MediaMap> SortedMediaMap;

#endif // MEDIAINFO_H
