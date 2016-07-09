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

#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include "mediamanager/mediainfo.h"

#include <VLCQtCore/Instance.h>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/Equalizer.h>

#include <QStack>
#include <QList>

class QSettings;

// Contains a configuration of the equalizer
struct EqualizerConfig {
    QList<float> frequencies;
    QList<float> amplifications;
    float preamplification;
    QString name;

    QString fileName;

    bool modified = false;
};
typedef QList<EqualizerConfig> EqualizerConfigList;

class MusicPlayer: public QObject
{
        Q_OBJECT

        Q_PROPERTY(bool initialized READ initialized NOTIFY initializedChanged)

        Q_PROPERTY(QString mediaTitle READ mediaTitle NOTIFY mediaTitleChanged)
        Q_PROPERTY(QString mediaArtist READ mediaArtist NOTIFY mediaArtistChanged)
        Q_PROPERTY(QString mediaAlbum READ mediaAlbum NOTIFY mediaAlbumChanged)
        Q_PROPERTY(QString mediaCover READ mediaCover NOTIFY mediaCoverChanged)
        Q_PROPERTY(int volume READ volume WRITE setVolume)
        Q_PROPERTY(QString formatedTime READ formatedTime NOTIFY formatedTimeChanged)
        Q_PROPERTY(int mediaIndex READ mediaIndex NOTIFY mediaIndexChanged)

        Q_PROPERTY(int time READ time NOTIFY timeChanged) // Time between 0 and 100
        Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)

        Q_PROPERTY(bool random READ random WRITE setRandom NOTIFY randomChanged)
        Q_PROPERTY(RepeatMode repeatMode READ repeatMode WRITE setRepeatMode NOTIFY repeatModeChanged)

        // Tell the QML interface if at least one music has been played
        Q_PROPERTY(bool started READ started NOTIFY startedChanged)
        Q_PROPERTY(MainViewType lastMainViewType READ lastMainViewType)
        Q_PROPERTY(bool hasHistoryEntry READ hasHistoryEntry NOTIFY hasHistoryEntryChanged)

        Q_PROPERTY(MediaInfoList mediaQueue READ mediaQueue NOTIFY mediaQueueChanged)

    public:
        MusicPlayer(QSettings *settings, VlcInstance *instance = nullptr); // ctor
        ~MusicPlayer();

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

        struct ListViewHistoryEntry {
            QUrl source;
            MediaInfo::MetadataType meta;
            QVariant metaValue;
            QString headerText;
            bool inPlaylist;
            QString playlistFile;
        };

        // Statics
        static QString equalizerDefaultConfigDir();
        static QString equalizerCustomConfigDir();
        static QString equalizerMainConfigDir();

        static QStringList vlcInstanceArgs();

    public slots:

        // Init the manager
        // This is the first thing to do
        void init();

        void play(MediaInfoList musicList, int index);
        // Play the specified media in the music list
        void play(MediaInfoList musicList, const QString& currentMediaUri);
        // Play the media. The music list is retrieve from the current media manager list
        void play(const QString &currentMediaUri);
        // Play the first media in the playlist
        void playFromPlaylist(const QString& playlistFileName);
        void playFromPlaylist(const QString& playlistFileName, const QString& mediaUri);

        void playNextIndex();
        void playPreviousIndex();
        void playFromIndex(const int& idx);

        void setVolume(int volume);

        void forward();
        void rewind();

        void pauseMusic();
        void resumeMusic();

        void nextMusic();
        void previousMusic();

        void setRepeatMode(RepeatMode mode);
        void setRandom(bool random);

        //
        // Equalizer stuff
        void setEqualizerConfig(int idx);
        void resetEqualizerConfig(int idx);
        void updateEqualizerConfig();

        void setEqualizerPreamp(float preamp);
        void setEqualizerAmp(int freqId, float amp);
        void increaseEqualizerPreamp(float inc);
        void increaseEqualizerAmp(int freqId, float inc);

        //
        // History stuff
        void addHistoryEntry(QUrl source, MediaInfo::MetadataType meta, QVariant metaValue,
                             const QString& headerText, bool inPlaylist, const QString& playlistFile);
        void removeLastHistoryEntry();

        //
        // GUI stuff
        void setPlayerVisibility(bool visible);
        void saveSettings();

    private slots:
        void onEndReachedHandler();

    signals:

        void mediaTitleChanged();
        void mediaArtistChanged();
        void mediaAlbumChanged();
        void mediaCoverChanged();
        void mediaIndexChanged();

        void formatedTimeChanged();
        void timeChanged();

        void isPlayingChanged();
        void endReached();

        void randomChanged();
        void repeatModeChanged();

        void mediaQueueChanged();
        void newMediaListPlayed();

        void newEqualizerConfigLoaded();
        void equalizerConfigChanged();

        void hasHistoryEntryChanged();
        void startedChanged();

        void initializedChanged();

    public:
        bool initialized() const;

        // Getters
        QString mediaTitle() const;
        QString mediaArtist() const;
        QString mediaAlbum() const;
        QString mediaCover() const;

        bool isPlaying();
        int volume() const;

        bool random() const;
        RepeatMode repeatMode() const;

        QString formatedTime();
        int time();

        MediaInfoList mediaQueue() const;
        int mediaIndex() const;

        bool started() const;
        Q_INVOKABLE bool isPlayerVisible();
        MainViewType lastMainViewType() const;

        //
        // Equalizer stuff
        Q_INVOKABLE int nbOfFrequenciesAvailable();
        Q_INVOKABLE QStringList equalizerConfigListNames();
        Q_INVOKABLE QString equalizerConfigName();
        Q_INVOKABLE int currentEqualizerConfigId();
        Q_INVOKABLE bool isEqualizerConfigModified(int idx);

        // Return freqs for the current preset
        Q_INVOKABLE float equalizerFrequency(int idx);
        Q_INVOKABLE QString equalizerFrequencyString(int idx);
        Q_INVOKABLE float equalizerAmplification(int idx);
        Q_INVOKABLE QString equalizerAmplificationString(int idx);
        Q_INVOKABLE float equalizerPreamplification();
        Q_INVOKABLE QString equalizerPreamplificationString();

        //
        // History stuff
        bool hasHistoryEntry();
        Q_INVOKABLE QUrl lastHistoryEntrySource();
        Q_INVOKABLE MediaInfo::MetadataType lastHistoryEntryMeta();
        Q_INVOKABLE QVariant lastHistoryEntryMetaValue();
        Q_INVOKABLE QString lastHistoryEntryHeaderText();
        Q_INVOKABLE bool lastHistoryEntryInPlaylist();
        Q_INVOKABLE QString lastHistoryEntryPlaylistFile();

    private:
        bool _init = false;

        VlcInstance *_vlcInstance = nullptr;
        VlcMediaPlayer *_vlcMediaPlayer = nullptr;
        VlcEqualizer *_vlcEqualizer = nullptr;
        VlcAudio *_vlcAudio = nullptr;
        VlcMedia *_currentMedia = nullptr;

        MediaInfoList _currentMediaList;
        MediaInfoList _currentMediaListRandomized;

        int _currentMediaIndex = 0;

        // Play parameters
        bool _playRandom = false;
        RepeatMode _repeatMode = NoRepeat;

        bool _started = false;
        bool _playerVisibility = false;
        MainViewType _lastMainViewType = MainViewType::Artists;
        // List view history
        QStack<ListViewHistoryEntry> _listViewHistory;

        QSettings *_settings;

        EqualizerConfigList _equalizerList;
        int _equalizerCurrentConfig = 0;

        // Emits all signals for the qml part
        void emitNewMediaSignals();

        void randomizeQueue();

        void checkForDefaultEqualizerConfigs();
        EqualizerConfig parseEqualizerConfigFile(const QString filePath, const QString fileName);
        void initEqualizerConfigList();
        void saveEqualizerConfigs();

        Q_DISABLE_COPY(MusicPlayer)
};

#endif // MUSICPLAYER_H
