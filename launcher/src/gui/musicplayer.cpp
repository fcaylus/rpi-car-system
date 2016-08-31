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

#include "musicplayer.h"

#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QProcess>
#include <QSettings>
#include <QDateTime>
#include <QUrl>
#include <QStringList>

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Audio.h>
#include <VLCQtCore/Error.h>

#include <algorithm>

#include "../core/dirutility.h"
#include "../core/common.h"

#include "../core/mediamanager/mediamanager.h"
#include "../core/mediamanager/playlist.h"

static const QString settingsGroupStr = "music";
static const QString settingsRandomStr = "random";
static const QString settingsRepeatModeStr = "repeat";
static const QString settingsLastMainViewType = "lastviewtype";
static const QString settingsVolumeStr = "volume";
static const QString settingsEqualizerStr = "eqconfig";

// Public constructor
MusicPlayer::MusicPlayer(QSettings *settings): QObject()
{
    _settings = settings;
}

// Static
QStringList MusicPlayer::vlcInstanceArgs()
{
#ifdef QT_DEBUG
    return VlcCommon::args() << "--verbose=1" << "--no-plugins-cache";
#else
    return VlcCommon::args() << "--verbose=0" << "--no-plugins-cache";
#endif
}

void MusicPlayer::init()
{
    if(initialized())
        return;

    // Load the engine
    _vlcInstance = new VlcInstance(vlcInstanceArgs());
    _vlcMediaPlayer = new VlcMediaPlayer(_vlcInstance);
    _vlcEqualizer = new VlcEqualizer(_vlcMediaPlayer);
    _vlcAudio = new VlcAudio(_vlcMediaPlayer);

    connect(_vlcMediaPlayer, &VlcMediaPlayer::timeChanged, this, &MusicPlayer::timeChanged);
    connect(_vlcMediaPlayer, &VlcMediaPlayer::timeChanged, this, &MusicPlayer::formatedTimeChanged);
    connect(_vlcMediaPlayer, &VlcMediaPlayer::lengthChanged, this, &MusicPlayer::formatedTimeChanged);

    connect(_vlcMediaPlayer, &VlcMediaPlayer::playing, this, &MusicPlayer::isPlayingChanged);
    connect(_vlcMediaPlayer, &VlcMediaPlayer::paused, this, &MusicPlayer::isPlayingChanged);
    connect(_vlcMediaPlayer, &VlcMediaPlayer::stopped, this, &MusicPlayer::isPlayingChanged);

    // On end handler
    connect(_vlcMediaPlayer, &VlcMediaPlayer::end, this, &MusicPlayer::onEndReachedHandler, Qt::QueuedConnection);

    // On error handler
    connect(_vlcMediaPlayer, &VlcMediaPlayer::error, [this](){
        qWarning() << VlcError::errmsg();
    });

    //
    // Load settings
    //

    _settings->beginGroup(settingsGroupStr);
    setRandom(_settings->value(settingsRandomStr, false).toBool());
    setRepeatMode(static_cast<RepeatMode>(_settings->value(settingsRepeatModeStr, 0).toInt()));
    _lastMainViewType = static_cast<MainViewType>(_settings->value(settingsLastMainViewType, 0).toInt());
    setVolume(_settings->value(settingsVolumeStr, -1).toInt());
    _equalizerCurrentConfig = _settings->value(settingsEqualizerStr, 0).toInt();
    _settings->endGroup();

    //
    // Load equalizer configs
    //

    checkForDefaultEqualizerConfigs();
    initEqualizerConfigList();
    // Check if the equalizer idx is in bound, else set to 0
    if(_equalizerCurrentConfig < 0 || _equalizerCurrentConfig >= _equalizerList.size())
        _equalizerCurrentConfig = 0;
    setEqualizerConfig(_equalizerCurrentConfig);

    //
    // Connect to the MediaManager events
    //

    connect(MediaManager::instance(), &MediaManager::sourceAboutToBeDeleted, this, [this](const QString& sourceIdentifier) {
        // Check if the media list is not empty
        if(_playRandom ? _currentMediaListRandomized.empty() : _currentMediaList.empty())
            return;

        // Remove all medias from the deleted source in the current queue
        MediaInfo *currentInfo = _playRandom ? _currentMediaListRandomized[_currentMediaIndex] : _currentMediaList[_currentMediaIndex];
        bool currentInfoAffected = false;
        bool somethingChanged = false;

        int i=0;
        while (i < _currentMediaList.size())
        {
            if(_currentMediaList.at(i)->sourceId() == sourceIdentifier)
            {
                if(_currentMediaList.at(i) == currentInfo)
                    currentInfoAffected = true;

                somethingChanged = true;
                _currentMediaList.removeAt(i);
            }
            else
                i++;
        }

        while (i < _currentMediaListRandomized.size())
        {
            if(_currentMediaListRandomized.at(i)->sourceId() == sourceIdentifier)
            {
                somethingChanged = true;
                _currentMediaListRandomized.removeAt(i);
            }
            else
                i++;
        }

        if(somethingChanged)
        {
            // Check if there is no more media
            if(_currentMediaList.isEmpty())
            {
                _vlcMediaPlayer->stop();
                delete _currentMedia;
                _currentMedia = nullptr;

                // Hide the "playing" view
                _started = false;
                emit startedChanged();
            }
            else
            {
                if(currentInfoAffected)
                {
                    if(_currentMediaIndex >= _currentMediaList.size())
                        _currentMediaIndex = 0;
                    emit mediaQueueChanged();
                    playFromIndex(_currentMediaIndex);
                }
                else
                {
                    // Update the index, but continue to play the current media
                    _currentMediaIndex = _playRandom ? _currentMediaListRandomized.indexOf(currentInfo) : _currentMediaList.indexOf(currentInfo);
                    emit mediaQueueChanged();
                }
            }
        }
    });

    _init = true;
    qDebug() << "MusicPlayer initialized !";

    emit initializedChanged();
}

bool MusicPlayer::initialized() const
{
    return _init;
}

MusicPlayer::~MusicPlayer()
{
    delete _vlcAudio;
    delete _currentMedia;
    delete _vlcEqualizer;
    delete _vlcMediaPlayer;
    delete _vlcInstance;
}

//
// Private slot
//

void MusicPlayer::onEndReachedHandler()
{
    emit isPlayingChanged();
    emit endReached();

    if(_repeatMode == RepeatOne)
        playFromIndex(_currentMediaIndex);
    else if(_repeatMode == RepeatAll)
        playFromIndex((_currentMediaIndex + 1) % _currentMediaList.size());
}

//
// Public methods
//

void MusicPlayer::play(MediaInfoList musicList, int index)
{
    if(!initialized())
        return;

    if(musicList.isEmpty())
        return;

    if(_currentMedia != nullptr)
    {
        delete _currentMedia;
        _currentMedia = nullptr;
    }

    _currentMediaListRandomized.clear();
    _currentMediaList.clear();
    _currentMediaIndex = 0;

    _currentMediaList = musicList;
    _currentMedia = new VlcMedia(musicList.at(index)->mediaUri(), musicList.at(index)->isLocalFile(), _vlcInstance);
    _currentMediaIndex = index;

    // Only play if a media was found
    if(_currentMedia != nullptr)
    {
        if(_playRandom)
            randomizeQueue();

        emitNewMediaSignals();
        emit mediaQueueChanged();

        //_vlcMediaPlayer->stop();
        _vlcMediaPlayer->open(_currentMedia);
        _vlcMediaPlayer->play();

        if(!_started)
        {
            _started = true;
            emit startedChanged();
        }

        // Must be after startedChanged() signal
        emit newMediaListPlayed();
    }
}

void MusicPlayer::play(MediaInfoList musicList, const QString &currentMediaUri)
{
    int idx = 0;
    for(int i=0; i < musicList.size(); ++i)
    {
        if(musicList.at(i)->mediaUri() == currentMediaUri)
        {
            idx = i;
            break;
        }
    }

    return play(musicList, idx);
}

void MusicPlayer::play(const QString &currentMediaUri)
{
    return play(MediaManager::instance()->lastMediaList(), currentMediaUri);
}

void MusicPlayer::playFromPlaylist(const QString &playlistFileName)
{
    Playlist* playlist = MediaManager::instance()->findPlaylistFromFileName(playlistFileName);
    if(playlist)
        play(playlist->availableMedias(), 0);
}

void MusicPlayer::playFromPlaylist(const QString &playlistFileName, const QString &mediaUri)
{
    Playlist* playlist = MediaManager::instance()->findPlaylistFromFileName(playlistFileName);
    if(playlist)
        play(playlist->availableMedias(), mediaUri);
}

void MusicPlayer::playNextIndex()
{
    if(_repeatMode == RepeatOne)
        playFromIndex(_currentMediaIndex);
    else if(_repeatMode == RepeatAll)
        playFromIndex((_currentMediaIndex + 1) % _currentMediaList.size());
    else if(_currentMediaIndex + 1 < _currentMediaList.size())
        playFromIndex(_currentMediaIndex + 1);
}

void MusicPlayer::playPreviousIndex()
{
    if(_repeatMode == RepeatOne)
        playFromIndex(_currentMediaIndex);
    else if(_repeatMode == RepeatAll)
        playFromIndex(_currentMediaIndex - 1 >= 0 ? _currentMediaIndex - 1: _currentMediaList.size() - 1);
    else if(_currentMediaIndex - 1 >= 0)
        playFromIndex(_currentMediaIndex - 1);
    else if(isPlaying())// Restart the media
        _vlcMediaPlayer->setTime(0);
}

void MusicPlayer::playFromIndex(const int& idx)
{
    if(_currentMediaList.isEmpty())
        return;

    _currentMediaIndex = idx;

    if(_currentMedia != nullptr)
        delete _currentMedia;

    MediaInfo *info = nullptr;

    if(_playRandom)
        info = _currentMediaListRandomized[_currentMediaIndex];
    else
        info = _currentMediaList[_currentMediaIndex];

    _currentMedia = new VlcMedia(info->mediaUri(), info->isLocalFile(), _vlcInstance);

    emitNewMediaSignals();
    _vlcMediaPlayer->open(_currentMedia);
    _vlcMediaPlayer->play();
}

void MusicPlayer::pauseMusic()
{
    _vlcMediaPlayer->pause();
}

void MusicPlayer::resumeMusic()
{
    _vlcMediaPlayer->resume();

    // Re-play last music
    if(_vlcMediaPlayer->state() == Vlc::Ended)
    {
        _vlcMediaPlayer->open(_currentMedia);
        _vlcMediaPlayer->play();
    }
}

void MusicPlayer::nextMusic()
{
    playNextIndex();
}

void MusicPlayer::previousMusic()
{
    // Re-start if the media reach the end
    if(_vlcMediaPlayer->state() == Vlc::Ended)
    {
        _vlcMediaPlayer->open(_currentMedia);
        _vlcMediaPlayer->play();
    }
    // Re-start the media if we reach 3 seconds
    else if(isPlaying() && _vlcMediaPlayer->time() > 3000)
        _vlcMediaPlayer->setTime(0);
    // Try to play the previous index
    else
        playPreviousIndex();
}

void MusicPlayer::forward()
{
    if(!isPlaying())
        return;

    const int newTime = _vlcMediaPlayer->time() + 30000;
    if(newTime > _vlcMediaPlayer->length())
        _vlcMediaPlayer->setTime(_vlcMediaPlayer->length());
    else
        _vlcMediaPlayer->setTime(newTime);
}

void MusicPlayer::rewind()
{
    if(!isPlaying())
        return;

    const int newTime = _vlcMediaPlayer->time() - 30000;
    if(newTime < 0)
        _vlcMediaPlayer->setTime(0);
    else
        _vlcMediaPlayer->setTime(newTime);
}

void MusicPlayer::setRandom(bool random)
{
    if(random == _playRandom)
        return;

    if(random)
        randomizeQueue();
    else if(!_currentMediaList.isEmpty())
    {
        // Update index
        MediaInfo *currentInfo = _currentMediaListRandomized[_currentMediaIndex];
        if(_currentMedia != nullptr)
            _currentMediaIndex = _currentMediaList.indexOf(currentInfo);
    }

    _playRandom = random;
    emit randomChanged();
    emit mediaQueueChanged();
}

void MusicPlayer::setRepeatMode(RepeatMode mode)
{
    if(mode == _repeatMode)
        return;

    _repeatMode = mode;
    emit repeatModeChanged();
}

void MusicPlayer::setVolume(int volume)
{
    if(volume >= 0 && volume <= 100)
        _vlcAudio->setVolume(volume);
}

void MusicPlayer::setPlayerVisibility(bool visible)
{
    _playerVisibility = visible;
}

// Getters
QString MusicPlayer::mediaAlbum() const
{
    if(_currentMediaList.size() > _currentMediaIndex)
        return _playRandom ? _currentMediaListRandomized[_currentMediaIndex]->album()
                           : _currentMediaList[_currentMediaIndex]->album();
    return MediaInfo::defaultAlbumName();
}

QString MusicPlayer::mediaArtist() const
{
    if(_currentMediaList.size() > _currentMediaIndex)
        return _playRandom ? _currentMediaListRandomized[_currentMediaIndex]->artist()
                           : _currentMediaList[_currentMediaIndex]->artist();
    return MediaInfo::defaultArtistName();
}

QString MusicPlayer::mediaTitle() const
{
    if(_currentMediaList.size() > _currentMediaIndex)
        return _playRandom ? _currentMediaListRandomized[_currentMediaIndex]->title()
                           : _currentMediaList[_currentMediaIndex]->title();
    return QString();
}

QString MusicPlayer::mediaCover() const
{
    if(_currentMediaList.size() > _currentMediaIndex)
        return _playRandom ? _currentMediaListRandomized[_currentMediaIndex]->coverUri()
                           : _currentMediaList[_currentMediaIndex]->coverUri();
    return MediaInfo::defaultCoverPath();
}

int MusicPlayer::time()
{
    if(_vlcMediaPlayer->time() == _vlcMediaPlayer->length())
        emit isPlayingChanged();

    return _vlcMediaPlayer->length() == 0 ? 0 : _vlcMediaPlayer->time() * 100 / _vlcMediaPlayer->length();
}

QString MusicPlayer::formatedTime()
{
    return Common::timeToString(_vlcMediaPlayer->time()) + QString(" / ") + Common::timeToString(_vlcMediaPlayer->length());
}

int MusicPlayer::volume() const
{
    return _vlcAudio->volume();
}

bool MusicPlayer::isPlaying()
{
    return _vlcMediaPlayer->state() == Vlc::Playing;
}

bool MusicPlayer::random() const
{
    return _playRandom;
}

MusicPlayer::RepeatMode MusicPlayer::repeatMode() const
{
    return _repeatMode;
}

bool MusicPlayer::started() const
{
    return _started;
}

bool MusicPlayer::isPlayerVisible()
{
    return _playerVisibility;
}

MusicPlayer::MainViewType MusicPlayer::lastMainViewType() const
{
    return _lastMainViewType;
}

MediaInfoList MusicPlayer::mediaQueue() const
{
    return _playRandom ? _currentMediaListRandomized : _currentMediaList;
}

int MusicPlayer::mediaIndex() const
{
    return _currentMediaIndex;
}

// Private
void MusicPlayer::emitNewMediaSignals()
{
    emit mediaAlbumChanged();
    emit mediaArtistChanged();
    emit mediaTitleChanged();
    emit mediaCoverChanged();
    emit mediaIndexChanged();
}

void MusicPlayer::randomizeQueue()
{
    if(_currentMediaList.isEmpty())
        return;

    MediaInfo *currentInfo = _currentMediaList[_currentMediaIndex];

    _currentMediaListRandomized = _currentMediaList;
    std::random_device rng;
    std::mt19937 urng(rng());
    std::shuffle(_currentMediaListRandomized.begin(), _currentMediaListRandomized.end(), urng);

    // Update current index
    if(_currentMedia != nullptr)
        _currentMediaIndex = _currentMediaListRandomized.indexOf(currentInfo);
}

//
// Equalizer stuff
//

// Static
QString MusicPlayer::equalizerCustomConfigDir()
{
    return Common::configDir() + QStringLiteral("/equalizer/custom");
}

// Static
QString MusicPlayer::equalizerDefaultConfigDir()
{
    return Common::configDir() + QStringLiteral("/equalizer/default");
}

// Static
QString MusicPlayer::equalizerMainConfigDir()
{
    return Common::configDir() + QStringLiteral("/equalizer");
}

// Private
void MusicPlayer::checkForDefaultEqualizerConfigs()
{
    const QString defaultDirPath = equalizerDefaultConfigDir();
    const QString customDirPath = equalizerCustomConfigDir();
    const int presetCount = _vlcEqualizer->presetCount();

    bool create = false;
    if(!QDir(defaultDirPath).exists())
        create = true;
    else
    {
        QStringList filesList = QDir(defaultDirPath).entryList(QStringList({QStringLiteral("*.eqconfig")}), QDir::Writable | QDir::Readable | QDir::Files);
        if(filesList.size() < presetCount)
            create = true;
    }

    // We need to create the default configs
    if(create)
    {
        QDir defaultDir(defaultDirPath);
        defaultDir.removeRecursively();
        defaultDir.mkpath(".");

        QDir customDir(customDirPath);
        if(!customDir.exists())
            customDir.mkpath(".");

        for(int i=0; i < presetCount; ++i)
        {
            QFile presetFile(defaultDirPath + QString("/preset%1.eqconfig").arg(i, 3, 10, QChar('0')));
            if(!presetFile.open(QFile::WriteOnly | QFile::Text))
                continue;

            QTextStream out(&presetFile);
            //
            // File structure:
            //
            // <file version>
            // <preset name>
            // <modified>
            // <preamplication>
            // <freq_i=amp_i>
            // ...
            //
            out << "v1\n" << _vlcEqualizer->presetNameAt(i) << "\nfalse\n" << _vlcEqualizer->preamplification() << "\n";

            _vlcEqualizer->loadFromPreset(i);
            for(int j=0, freqNb=_vlcEqualizer->bandCount(); j < freqNb; ++j)
                out << _vlcEqualizer->bandFrequency(j) << "=" << _vlcEqualizer->amplificationForBandAt(j) << "\n";

            out.flush();
            presetFile.close();

            // Check if the coresponding custom file exists
            if(!QFile::exists(customDirPath + QString("/preset%1.eqconfig").arg(i, 3, 10, QChar('0'))))
                presetFile.copy(customDirPath + QString("/preset%1.eqconfig").arg(i, 3, 10, QChar('0')));
        }

        // Reset the equalizer
        delete _vlcEqualizer;
        _vlcEqualizer = new VlcEqualizer(_vlcMediaPlayer);
    }
}

// Private
void MusicPlayer::initEqualizerConfigList()
{
    _equalizerList.clear();

    const QString path = equalizerCustomConfigDir();
    QStringList filesList = QDir(path).entryList(QStringList({QStringLiteral("*.eqconfig")}), QDir::Writable | QDir::Readable | QDir::Files, QDir::Name);
    for(QString fileName: filesList)
    {
        QFile file(path + "/" + fileName);
        EqualizerConfig eq = parseEqualizerConfigFile(path + "/" + fileName, fileName);

        // Check frequencies count
        if((uint)eq.frequencies.size() == _vlcEqualizer->bandCount())
            _equalizerList.append(eq);
    }
}

// Private
EqualizerConfig MusicPlayer::parseEqualizerConfigFile(const QString filePath, const QString fileName)
{
    EqualizerConfig eq;
    QFile file(filePath);
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&file);
        if(in.readLine() == "v1")
        {
            eq.fileName = fileName;
            eq.name = in.readLine();
            eq.modified = (in.readLine() == "true" ? true : false);
            eq.preamplification = in.readLine().toDouble();

            while(!in.atEnd())
            {
                QString line = in.readLine();
                QStringList values = line.split("=");
                if(values.size() == 2)
                {
                    eq.frequencies.append(values[0].toDouble());
                    eq.amplifications.append(values[1].toDouble());
                }
            }
        }
        file.close();
    }

    return eq;
}

// Private
void MusicPlayer::saveEqualizerConfigs()
{
    for(EqualizerConfig eq: _equalizerList)
    {
        if(eq.modified)
        {
            const QString path = equalizerCustomConfigDir() + "/" + eq.fileName;
            QFile file(path);
            if(file.open(QFile::WriteOnly | QFile::Text))
            {
                QTextStream out(&file);
                out << "v1\n" << eq.name << "\ntrue\n" << eq.preamplification << "\n";
                for(int i=0, size = eq.amplifications.size(); i < size; ++i)
                    out << eq.frequencies[i] << "=" << eq.amplifications[i] << "\n";

                out.flush();
                file.close();
            }
        }
    }
}

void MusicPlayer::resetEqualizerConfig(int idx)
{
    if(idx < 0 || idx >= _equalizerList.size())
        return;

    EqualizerConfig eq = _equalizerList[idx];
    const QString customPath = equalizerCustomConfigDir() + "/" + eq.fileName;
    const QString defaultPath = equalizerDefaultConfigDir() + "/" + eq.fileName;
    if(QFile::exists(defaultPath))
    {
        QFile::remove(customPath);
        QFile::copy(defaultPath, customPath);
    }

    // Update the eq config object
    _equalizerList[idx] = parseEqualizerConfigFile(customPath, eq.fileName);
    if(idx == _equalizerCurrentConfig)
        updateEqualizerConfig();
}

void MusicPlayer::setEqualizerConfig(int idx)
{
    if(idx >= 0 && idx < _equalizerList.size())
    {
        _equalizerCurrentConfig = idx;
        updateEqualizerConfig();
        emit newEqualizerConfigLoaded();
    }
}

void MusicPlayer::updateEqualizerConfig()
{
    EqualizerConfig eq = _equalizerList.at(_equalizerCurrentConfig);
    _vlcEqualizer->setPreamplification(eq.preamplification);

    for(int i=0; i < eq.frequencies.size(); ++i)
        _vlcEqualizer->setAmplificationForBandAt(eq.amplifications[i], i);

    _vlcEqualizer->setEnabled(true);
    emit equalizerConfigChanged();
}

// Setters
void MusicPlayer::setEqualizerPreamp(float preamp)
{
    _equalizerList[_equalizerCurrentConfig].preamplification = preamp;
    _equalizerList[_equalizerCurrentConfig].modified = true;
    _vlcEqualizer->setPreamplification(preamp);
    _vlcEqualizer->setEnabled(true);
    emit equalizerConfigChanged();
}

void MusicPlayer::setEqualizerAmp(int freqId, float amp)
{
    _equalizerList[_equalizerCurrentConfig].amplifications[freqId] = amp;
    _equalizerList[_equalizerCurrentConfig].modified = true;
    _vlcEqualizer->setAmplificationForBandAt(freqId, amp);
    _vlcEqualizer->setEnabled(true);
    emit equalizerConfigChanged();
}

void MusicPlayer::increaseEqualizerPreamp(float inc)
{
    float v = equalizerPreamplification() + inc;
    if(v > 20.0f)
        v = 20.0f;
    else if (v < -20.0f)
        v = -20.0f;

    setEqualizerPreamp(v);
}

void MusicPlayer::increaseEqualizerAmp(int freqId, float inc)
{
    float v = equalizerAmplification(freqId) + inc;
    if(v > 20.0f)
        v = 20.0f;
    else if (v < -20.0f)
        v = -20.0f;

    setEqualizerAmp(freqId, v);
}

int MusicPlayer::currentEqualizerConfigId()
{
    return _equalizerCurrentConfig;
}

bool MusicPlayer::isEqualizerConfigModified(int idx)
{
    if(idx >= 0 && idx < _equalizerList.size())
        return _equalizerList[idx].modified;
    return false;
}

int MusicPlayer::nbOfFrequenciesAvailable()
{
    return _vlcEqualizer->bandCount();
}

QStringList MusicPlayer::equalizerConfigListNames()
{
    QStringList names;
    for(EqualizerConfig eq: _equalizerList)
        names.append(eq.name);
    return names;
}

QString MusicPlayer::equalizerConfigName()
{
    return _equalizerList.at(_equalizerCurrentConfig).name;
}

float MusicPlayer::equalizerFrequency(int idx)
{
    return _equalizerList.at(_equalizerCurrentConfig).frequencies[idx];
}

QString MusicPlayer::equalizerFrequencyString(int idx)
{
    return Common::unitToString(equalizerFrequency(idx), "Hz", 2);
}

float MusicPlayer::equalizerAmplification(int idx)
{
    return _equalizerList.at(_equalizerCurrentConfig).amplifications[idx];
}

QString MusicPlayer::equalizerAmplificationString(int idx)
{
    return Common::unitToString(equalizerAmplification(idx), "dB", 1, false);
}

float MusicPlayer::equalizerPreamplification()
{
    return _equalizerList.at(_equalizerCurrentConfig).preamplification;
}

QString MusicPlayer::equalizerPreamplificationString()
{
    return Common::unitToString(equalizerPreamplification(), "dB", 1, false);
}

//
// History stuff
//

void MusicPlayer::addHistoryEntry(QUrl source, MediaInfo::MetadataType meta,
                                  QVariant metaValue, const QString &headerText,
                                  bool inPlaylist, const QString &playlistFile)
{
    _listViewHistory.push(ListViewHistoryEntry({source, meta, metaValue, headerText, inPlaylist, playlistFile}));
    emit hasHistoryEntryChanged();

    // Update last view type if it's a top level list view
    if(meta == MediaInfo::UNKNOWN && metaValue.type() == QVariant::Invalid && inPlaylist == false)
    {
        if(source.toString() == "qrc:/qml/music/ListViewArtist.qml")
        {
            _lastMainViewType = MainViewType::Artists;
        }
        else if(source.toString() == "qrc:/qml/music/ListViewAlbum.qml")
        {
            _lastMainViewType = MainViewType::Albums;
        }
        else if(source.toString() == "qrc:/qml/music/ListViewTrack.qml")
        {
            _lastMainViewType = MainViewType::Tracks;
        }
        else if(source.toString() == "qrc:/qml/music/ListViewPlaylist.qml")
        {
            _lastMainViewType = MainViewType::Playlists;
        }
    }
}

void MusicPlayer::removeLastHistoryEntry()
{
    if(_listViewHistory.isEmpty())
        return;

    _listViewHistory.pop();
    if(_listViewHistory.isEmpty())
        emit hasHistoryEntryChanged();
}

bool MusicPlayer::hasHistoryEntry()
{
    return !_listViewHistory.isEmpty();
}

QUrl MusicPlayer::lastHistoryEntrySource()
{
    return _listViewHistory.top().source;
}

QVariant MusicPlayer::lastHistoryEntryMetaValue()
{
    return _listViewHistory.top().metaValue;
}

MediaInfo::MetadataType MusicPlayer::lastHistoryEntryMeta()
{
    return _listViewHistory.top().meta;
}

QString MusicPlayer::lastHistoryEntryHeaderText()
{
    return _listViewHistory.top().headerText;
}

bool MusicPlayer::lastHistoryEntryInPlaylist()
{
    return _listViewHistory.top().inPlaylist;
}

QString MusicPlayer::lastHistoryEntryPlaylistFile()
{
    return _listViewHistory.top().playlistFile;
}

//
// Settings
//

void MusicPlayer::saveSettings()
{
    if(!_init)
        return;

    _settings->beginGroup(settingsGroupStr);
    _settings->setValue(settingsRandomStr, random());
    _settings->setValue(settingsRepeatModeStr, static_cast<int>(repeatMode()));
    _settings->setValue(settingsLastMainViewType, static_cast<int>(lastMainViewType()));
    _settings->setValue(settingsVolumeStr, volume());
    saveEqualizerConfigs();
    _settings->setValue(settingsEqualizerStr, _equalizerCurrentConfig);
    _settings->endGroup();
}


