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

        Q_PROPERTY(QString title READ title WRITE setTitle)
        Q_PROPERTY(QString artist READ artist WRITE setArtist)
        Q_PROPERTY(QString album READ album WRITE setAlbum)
        Q_PROPERTY(QString coverFile READ coverFile WRITE setCoverFile)
        Q_PROPERTY(int trackNumber READ trackNumber WRITE setTrackNumber)

        Q_PROPERTY(QString filePath READ filePath WRITE setFilePath)
        Q_PROPERTY(long fileSize READ fileSize WRITE setFileSize)

        Q_PROPERTY(QUrl coverFileUrl READ coverFileUrl)

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
