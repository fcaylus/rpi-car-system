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

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "mediainfo.h"

#include <VLCQtCore/Instance.h>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtCore/Media.h>

#include <QUrl>
#include <QStack>

class QSettings;

struct ListViewHistoryEntry {
    QUrl source;
    QString sourceFile;
    QString sourceQuery;
    QString headerText;
};

// Singleton sound manager
class SoundManager: public QObject
{
        Q_OBJECT

        Q_PROPERTY(bool initialized READ initialized())

        Q_PROPERTY(QString mediaTitle READ mediaTitle NOTIFY mediaTitleChanged)
        Q_PROPERTY(QString mediaArtist READ mediaArtist NOTIFY mediaArtistChanged)
        Q_PROPERTY(QString mediaAlbum READ mediaAlbum NOTIFY mediaAlbumChanged)
        Q_PROPERTY(QUrl mediaCover READ mediaCover NOTIFY mediaCoverChanged)
        Q_PROPERTY(int volume READ volume WRITE setVolume)
        Q_PROPERTY(QString formatedTime READ formatedTime NOTIFY formatedTimeChanged)

        Q_PROPERTY(int time READ time NOTIFY timeChanged) // Time between 0 and 100
        Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)

        Q_PROPERTY(bool random READ random WRITE setRandom NOTIFY randomChanged)
        Q_PROPERTY(RepeatMode repeatMode READ repeatMode WRITE setRepeatMode NOTIFY repeatModeChanged)

        Q_PROPERTY(bool mediaListReady READ mediaListReady NOTIFY mediaListReadyChanged)

        Q_PROPERTY(QUrl trackListFilePath READ trackListFilePath NOTIFY mediaListReadyChanged)
        Q_PROPERTY(QUrl artistMapFilePath READ artistMapFilePath NOTIFY mediaListReadyChanged)
        Q_PROPERTY(QUrl albumMapFilePath READ albumMapFilePath NOTIFY mediaListReadyChanged)

        Q_PROPERTY(bool hasHistoryEntry READ hasHistoryEntry NOTIFY hasHistoryEntryChanged)

        // Tell the QML interface if at least one music has been played
        Q_PROPERTY(bool started READ started NOTIFY startedChanged)
        Q_PROPERTY(MainViewType lastMainViewType READ lastMainViewType)

        Q_PROPERTY(QStringList currentMediaQueueTitles READ currentMediaQueueTitles NOTIFY currentMediaQueueChanged)
        Q_PROPERTY(QStringList currentMediaQueueCovers READ currentMediaQueueCovers NOTIFY currentMediaQueueChanged)

        Q_PROPERTY(QStringList playlistNames READ playlistNames)
        Q_PROPERTY(QStringList playlistFiles READ playlistFiles)

    public:
        SoundManager(QSettings *settings, QObject *parent = nullptr); // ctor
        ~SoundManager();

        enum RepeatMode {
            NoRepeat = 0,
            RepeatAll = 1,
            RepeatOne = 2
        };
        Q_ENUM(RepeatMode)

        enum MainViewType {
            Artists = 0,
            Albums = 1,
            Tracks = 2,
            Playlists = 3
        };
        Q_ENUM(MainViewType)

    public slots:

        // Init the manager
        // This is the first thing to do
        void init();

        // xmlSourceFile and xmlSourceQuery are used to fill the current media queue
        // if path is empty, play the first music in the sourceQuery
        void playFromFile(QString path, QString xmlSourceFile = "", const QString& xmlSourceQuery = "");
        void playNextIndex();
        void playPreviousIndex();
        void playFromIndex(const int& idx);
        void playFromPlaylist(const QString& playlistFile);

        void setVolume(int volume);

        void forward();
        void rewind();

        void pauseMusic();
        void resumeMusic();

        void nextMusic();
        void previousMusic();

        void setRepeatMode(RepeatMode mode);
        void setRandom(bool random);

        // Playlist stuff
        void addSongToPlaylist(QString playlistFileName, QString musicTitle, QString musicPath, QString musicCover);
        void removePlaylistFile(const QString& fileName);
        void removeFromPlaylist(const QString& playlistFilePath, const QString& musicFile);

        // History stuff
        void addHistoryEntry(QUrl source, QString sourceFile, QString sourceQuery, QString headerText);
        void removeLastHistoryEntry();

        void setPlayerVisibility(bool visible);
        void saveSettings();

    private slots:
        void onEndReachedHandler();

    signals:

        void mediaTitleChanged();
        void mediaArtistChanged();
        void mediaAlbumChanged();
        void mediaCoverChanged();

        void formatedTimeChanged();
        void timeChanged();

        void isPlayingChanged();
        void endReached();

        void randomChanged();
        void repeatModeChanged();

        void mediaListReadyChanged();

        void hasHistoryEntryChanged();
        void startedChanged();

        void currentMediaQueueChanged();
        void newMediaPlayedFromFile();

    public:

        bool initialized() const;

        // Getters
        QString mediaTitle() const;
        QString mediaArtist() const;
        QString mediaAlbum() const;
        QUrl mediaCover() const;

        bool isPlaying();
        int volume() const;

        bool random() const;
        RepeatMode repeatMode() const;

        QString formatedTime();
        int time();

        bool mediaListReady();

        QUrl artistMapFilePath() const;
        QUrl albumMapFilePath() const;
        QUrl trackListFilePath() const;

        bool started() const;
        Q_INVOKABLE bool isPlayerVisible();
        MainViewType lastMainViewType() const;

        //
        // History stuff
        bool hasHistoryEntry();
        Q_INVOKABLE QUrl lastHistoryEntrySource();
        Q_INVOKABLE QString lastHistoryEntrySourceFile();
        Q_INVOKABLE QString lastHistoryEntrySourceQuery();
        Q_INVOKABLE QString lastHistoryEntryHeaderText();

        QStringList currentMediaQueueTitles();
        QStringList currentMediaQueueCovers();
        Q_INVOKABLE int currentIndex();

        //
        // Playlist stuff
        QStringList playlistNames();
        QStringList playlistFiles();
        Q_INVOKABLE QUrl playlistUrl(QString file);
        // Return the created file name
        Q_INVOKABLE QString createNewPlaylist(QString name);

        // Convert time in ms to "hh:mm:ss"
        static QString timeToString(unsigned int time);

    private:
        bool _init = false;

        VlcInstance *_vlcInstance = nullptr;
        VlcMediaPlayer *_vlcMediaPlayer = nullptr;
        VlcAudio *_vlcAudio = nullptr;
        VlcMedia *_currentMedia = nullptr;

        MediaList _currentMediaList;
        MediaList _currentMediaListRandomized;
        int _currentMediaIndex = 0;

        // Play parameters
        bool _playRandom = false;
        RepeatMode _repeatMode = NoRepeat;

        bool _mediaListReady = false;

        bool _started = false;
        bool _playerVisibility = false;

        // List view history
        QStack<ListViewHistoryEntry> _listViewHistory;
        MainViewType _lastMainViewType = MainViewType::Artists;

        QStringList _playlistNames;
        QStringList _playlistFiles;

        QSettings *_settings;

        // Emits all signals for the qml part
        void emitNewMediaSignals();

        void randomizeQueue();
        void updatePlaylistsData();

        Q_DISABLE_COPY(SoundManager)
};


#endif // SOUNDMANAGER_H
