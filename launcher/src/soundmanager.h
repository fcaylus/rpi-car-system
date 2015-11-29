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

        Q_PROPERTY(QStringList currentMediaQueueTitles READ currentMediaQueueTitles NOTIFY currentMediaQueueChanged)
        Q_PROPERTY(QStringList currentMediaQueueCovers READ currentMediaQueueCovers NOTIFY currentMediaQueueChanged)

    public:
        SoundManager(QSettings *settings); // ctor

        enum RepeatMode {
            NoRepeat = 0,
            RepeatAll = 1,
            RepeatOne = 2
        };
        Q_ENUM(RepeatMode)

    public slots:

        // xmlSourceFile and xmlSourceQuery are optional and used to fill the current media queue
        // ie: fill the media queue with the current album
        void playFromFile(const QString& path, QString xmlSourceFile = "", const QString& xmlSourceQuery = "");
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

        void setPlayerVisibility(bool visible);

        // History related
        void addHistoryEntry(QUrl source, QString sourceFile, QString sourceQuery, QString headerText);
        void removeLastHistoryEntry();

        void saveSettings();

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

    public:

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

        // List History related
        bool hasHistoryEntry();
        Q_INVOKABLE QUrl lastHistoryEntrySource();
        Q_INVOKABLE QString lastHistoryEntrySourceFile();
        Q_INVOKABLE QString lastHistoryEntrySourceQuery();
        Q_INVOKABLE QString lastHistoryEntryHeaderText();

        QStringList currentMediaQueueTitles();
        QStringList currentMediaQueueCovers();
        Q_INVOKABLE int currentIndex();

        // Convert time in ms to "hh:mm:ss"
        static QString timeToString(unsigned int time);

    private:

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

        QSettings *_settings;


        // Emits all signals for the qml part
        void emitNewMediaSignals();

        void randomizeQueue();

        Q_DISABLE_COPY(SoundManager)
};


#endif // SOUNDMANAGER_H
