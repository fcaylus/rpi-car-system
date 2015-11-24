#include "soundmanager.h"

#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QProcess>

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Audio.h>

#include <algorithm>

#include "pugixml.hpp"
#include "dirutility.h"
#include "common.h"

// Public constructor
SoundManager::SoundManager()
{
    // Load the engine
    _vlcInstance = new VlcInstance(VlcCommon::args(), this);
    _vlcMediaPlayer = new VlcMediaPlayer(_vlcInstance);
    _vlcAudio = new VlcAudio(_vlcMediaPlayer);

    connect(_vlcMediaPlayer, &VlcMediaPlayer::timeChanged, this, &SoundManager::timeChanged);
    connect(_vlcMediaPlayer, &VlcMediaPlayer::timeChanged, this, &SoundManager::formatedTimeChanged);
    connect(_vlcMediaPlayer, &VlcMediaPlayer::lengthChanged, this, &SoundManager::formatedTimeChanged);

    connect(_vlcMediaPlayer, &VlcMediaPlayer::playing, this, &SoundManager::isPlayingChanged);
    connect(_vlcMediaPlayer, &VlcMediaPlayer::paused, this, &SoundManager::isPlayingChanged);
    connect(_vlcMediaPlayer, &VlcMediaPlayer::stopped, this, &SoundManager::isPlayingChanged);

    // On end handler
    connect(_vlcMediaPlayer, &VlcMediaPlayer::end, [this](){
        emit isPlayingChanged();
        emit endReached();

        if(_repeatMode == RepeatOne)
        {
            _vlcMediaPlayer->open(_currentMedia);
            _vlcMediaPlayer->play();
        }
        else if(_repeatMode == RepeatAll)
        {
            playFromIndex((_currentMediaIndex + 1) % _currentMediaList.size());
        }
    });

    // Check if map files exists
    if(QFileInfo::exists(MediaInfo::albumMapFilePath())
       && QFileInfo::exists(MediaInfo::artistMapFilePath())
       && QFileInfo::exists(MediaInfo::trackListFilePath()))
    {
        // Medias are already parsed, send the signal
        _mediaListReady = true;
        emit mediaListReadyChanged();
    }

    // Start the process for searching new devices
    QProcess *musicSearchProcess = new QProcess(this);
    connect(musicSearchProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            [this](int exitCode, QProcess::ExitStatus exitStatus) {
        if(exitCode == 0 && exitStatus == QProcess::NormalExit)
        {
            qDebug() << "test";
            _mediaListReady = true;
            emit mediaListReadyChanged();
        }
    });
    musicSearchProcess->start(QCoreApplication::applicationDirPath() + QStringLiteral("/musicindex-generator"));
}

//
// Public methods
//

void SoundManager::playFromFile(const QString &path, QString xmlSourceFile, const QString &xmlSourceQuery)
{
    if(_currentMedia != nullptr)
        delete _currentMedia;

    _currentMediaListRandomized.clear();
    while(!_currentMediaList.isEmpty())
        delete _currentMediaList.takeLast();
    _currentMediaIndex = 0;

    // Add all medias in the list
    if(!xmlSourceFile.isEmpty() && !xmlSourceQuery.isEmpty()
       && xmlSourceFile != trackListFilePath().toString())
    {
        // Use pugixml since Qt doesn't provide "simple" xpath support
        pugi::xml_document xmlDoc;
        if(!xmlDoc.load_file(xmlSourceFile.remove(0, 7).toStdString().c_str())) // Remove "file://" prefix
        {
            qCritical() << "Cannot open xml file at:" << xmlSourceFile;
            return;
        }

        pugi::xpath_node_set nodes = xmlDoc.select_nodes(xmlSourceQuery.toStdString().c_str());
        for(pugi::xpath_node_set::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
        {
            pugi::xpath_node node = *it;
            const QString trackPath = node.node().child_value("path");

            // Check the place of the current track
            if(trackPath == path)
            {
                _currentMedia = new VlcMedia(path, true, _vlcInstance);
                _currentMediaList.append(new MediaInfo(_currentMedia, path));
                _currentMediaIndex = _currentMediaList.size() - 1;
            }
            else
            {
                VlcMedia *media = new VlcMedia(trackPath, true, _vlcInstance);
                _currentMediaList.append(new MediaInfo(media, trackPath));
                delete media;
            }
        }
    }
    // Only add the specified media
    else
    {
        _currentMedia = new VlcMedia(path, true, _vlcInstance);
        _currentMediaList.append(new MediaInfo(_currentMedia, path));
    }

    if(_playRandom)
        randomizeQueue();

    emitNewMediaSignals();
    emit currentMediaQueueChanged();

    //_vlcMediaPlayer->stop();
    _vlcMediaPlayer->open(_currentMedia);
    _vlcMediaPlayer->play();

    if(!_started)
    {
        _started = true;
        emit startedChanged();
    }
}

void SoundManager::playNextIndex()
{
    if(_repeatMode == RepeatOne)
        playFromIndex(_currentMediaIndex);
    else if(_repeatMode == RepeatAll)
        playFromIndex((_currentMediaIndex + 1) % _currentMediaList.size());
    else if(_currentMediaIndex + 1 < _currentMediaList.size())
        playFromIndex(_currentMediaIndex + 1);
}

void SoundManager::playPreviousIndex()
{
    if(_repeatMode == RepeatOne)
        playFromIndex(_currentMediaIndex);
    else if(_repeatMode == RepeatAll)
        playFromIndex(_currentMediaIndex - 1 >= 0 ? _currentMediaIndex - 1: _currentMediaList.size() - 1);
    else if(_currentMediaIndex - 1 >= 0)
        playFromIndex(_currentMediaIndex - 1);
}

// Private
void SoundManager::playFromIndex(const int& idx)
{
    _currentMediaIndex = idx;

    if(_currentMedia != nullptr)
        delete _currentMedia;

    if(_playRandom)
        _currentMedia = new VlcMedia(_currentMediaListRandomized[_currentMediaIndex]->filePath(), true, _vlcInstance);
    else
        _currentMedia = new VlcMedia(_currentMediaList[_currentMediaIndex]->filePath(), true, _vlcInstance);

    emitNewMediaSignals();
    _vlcMediaPlayer->open(_currentMedia);
    _vlcMediaPlayer->play();
}

void SoundManager::pauseMusic()
{
    _vlcMediaPlayer->pause();
}

void SoundManager::resumeMusic()
{
    _vlcMediaPlayer->resume();

    // Re-play last music
    if(_vlcMediaPlayer->state() == Vlc::Ended)
    {
        _vlcMediaPlayer->open(_currentMedia);
        _vlcMediaPlayer->play();
    }
}

void SoundManager::nextMusic()
{
    playNextIndex();
}

void SoundManager::previousMusic()
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

void SoundManager::forward()
{
    if(!isPlaying())
        return;

    const int newTime = _vlcMediaPlayer->time() + 30000;
    if(newTime > _vlcMediaPlayer->length())
        _vlcMediaPlayer->setTime(_vlcMediaPlayer->length());
    else
        _vlcMediaPlayer->setTime(newTime);
}

void SoundManager::rewind()
{
    if(!isPlaying())
        return;

    const int newTime = _vlcMediaPlayer->time() - 30000;
    if(newTime < 0)
        _vlcMediaPlayer->setTime(0);
    else
        _vlcMediaPlayer->setTime(newTime);
}

void SoundManager::setRandom(bool random)
{
    if(random == _playRandom)
        return;

    if(random)
        randomizeQueue();
    else
    {
        // Update index
        MediaInfo *currentInfo = _currentMediaListRandomized[_currentMediaIndex];
        if(_currentMedia != nullptr)
            _currentMediaIndex = _currentMediaList.indexOf(currentInfo);
    }

    _playRandom = random;
    emit randomChanged();
    emit currentMediaQueueChanged();
}

void SoundManager::setRepeatMode(RepeatMode mode)
{
    if(mode == _repeatMode)
        return;

    _repeatMode = mode;
    emit repeatModeChanged();
}

void SoundManager::setVolume(int volume)
{
    if(volume >= 0 && volume <= 100)
        _vlcAudio->setVolume(volume);
}

void SoundManager::setPlayerVisibility(bool visible)
{
    _playerVisibility = visible;
}

// Getters
QString SoundManager::mediaAlbum() const
{
    if(_currentMediaList.size() > _currentMediaIndex)
        return _playRandom ? _currentMediaListRandomized[_currentMediaIndex]->album()
                           : _currentMediaList[_currentMediaIndex]->album();
    return MediaInfo::defaultAlbumName();
}

QString SoundManager::mediaArtist() const
{
    if(_currentMediaList.size() > _currentMediaIndex)
        return _playRandom ? _currentMediaListRandomized[_currentMediaIndex]->artist()
                           : _currentMediaList[_currentMediaIndex]->artist();
    return MediaInfo::defaultArtistName();
}

QString SoundManager::mediaTitle() const
{
    if(_currentMediaList.size() > _currentMediaIndex)
        return _playRandom ? _currentMediaListRandomized[_currentMediaIndex]->title()
                           : _currentMediaList[_currentMediaIndex]->title();
    return QString();
}

QUrl SoundManager::mediaCover() const
{
    if(_currentMediaList.size() > _currentMediaIndex)
        return _playRandom ? _currentMediaListRandomized[_currentMediaIndex]->coverFileUrl()
                           : _currentMediaList[_currentMediaIndex]->coverFileUrl();
    return QUrl(MediaInfo::defaultCoverPath());
}

int SoundManager::time()
{
    if(_vlcMediaPlayer->time() == _vlcMediaPlayer->length())
        emit isPlayingChanged();

    return _vlcMediaPlayer->length() == 0 ? 0 : _vlcMediaPlayer->time() * 100 / _vlcMediaPlayer->length();
}

QString SoundManager::formatedTime()
{
    return timeToString(_vlcMediaPlayer->time()) + QString(" / ") + timeToString(_vlcMediaPlayer->length());
}

int SoundManager::volume() const
{
    return _vlcAudio->volume();
}

bool SoundManager::isPlaying()
{
    return _vlcMediaPlayer->state() == Vlc::Playing;
}

bool SoundManager::random() const
{
    return _playRandom;
}

SoundManager::RepeatMode SoundManager::repeatMode() const
{
    return _repeatMode;
}

bool SoundManager::mediaListReady()
{
    return _mediaListReady;
}

QUrl SoundManager::albumMapFilePath() const
{
    return QUrl::fromLocalFile(MediaInfo::albumMapFilePath());
}

QUrl SoundManager::artistMapFilePath() const
{
    return QUrl::fromLocalFile(MediaInfo::artistMapFilePath());
}

QUrl SoundManager::trackListFilePath() const
{
    return QUrl::fromLocalFile(MediaInfo::trackListFilePath());
}

bool SoundManager::started() const
{
    return _started;
}

bool SoundManager::isPlayerVisible()
{
    return _playerVisibility;
}

QStringList SoundManager::currentMediaQueueTitles()
{
    QStringList list;
    if(_playRandom)
    {
        for(MediaInfo *info: _currentMediaListRandomized)
            list << info->title();

        return list;
    }
    for(MediaInfo *info: _currentMediaList)
        list << info->title();

    return list;
}

QStringList SoundManager::currentMediaQueueCovers()
{
    QStringList list;
    if(_playRandom)
    {
        for(MediaInfo *info: _currentMediaListRandomized)
            list << info->coverFileUrl().toString();

        return list;
    }
    for(MediaInfo *info: _currentMediaList)
        list << info->coverFileUrl().toString();

    return list;
}

int SoundManager::currentIndex()
{
    return _currentMediaIndex;
}

// Private
void SoundManager::emitNewMediaSignals()
{
    emit mediaAlbumChanged();
    emit mediaArtistChanged();
    emit mediaTitleChanged();
    emit mediaCoverChanged();
}

void SoundManager::randomizeQueue()
{
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
// History related
//

void SoundManager::addHistoryEntry(QUrl source, QString sourceFile, QString sourceQuery, QString headerText)
{
    _listViewHistory.push(ListViewHistoryEntry({source, sourceFile, sourceQuery, headerText}));
    emit hasHistoryEntryChanged();
}

void SoundManager::removeLastHistoryEntry()
{
    _listViewHistory.pop();
    if(_listViewHistory.isEmpty())
        emit hasHistoryEntryChanged();
}

bool SoundManager::hasHistoryEntry()
{
    return !_listViewHistory.isEmpty();
}

QUrl SoundManager::lastHistoryEntrySource()
{
    return _listViewHistory.top().source;
}

QString SoundManager::lastHistoryEntrySourceFile()
{
    return _listViewHistory.top().sourceFile;
}

QString SoundManager::lastHistoryEntrySourceQuery()
{
    return _listViewHistory.top().sourceQuery;
}

QString SoundManager::lastHistoryEntryHeaderText()
{
    return _listViewHistory.top().headerText;
}

// Static
QString SoundManager::timeToString(unsigned int time)
{
    const unsigned int timeInSeconds = time / 1000;
    const unsigned int hours = timeInSeconds / 3600;
    const unsigned int minutes = (timeInSeconds % 3600) / 60;
    const unsigned int secondes = timeInSeconds - hours * 3600 - minutes * 60;

    QString result = "";

    if(hours != 0)
        result += QString::number(hours) + QString(":");

    if(minutes < 10 && hours != 0)
        result += QString("0") + QString::number(minutes);
    else
        result += QString::number(minutes);

    result += ":";

    if(secondes < 10)
        result += QString("0") + QString::number(secondes);
    else
        result += QString::number(secondes);

    return result;
}
