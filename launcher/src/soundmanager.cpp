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

#include "soundmanager.h"

#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QProcess>
#include <QSettings>
#include <QDateTime>
#include <QXmlStreamWriter>

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Audio.h>
#include <VLCQtCore/Error.h>

#include <algorithm>

#include "pugixml.hpp"
#include "dirutility.h"
#include "common.h"

static const QString settingsGroupStr = "music";
static const QString settingsRandomStr = "random";
static const QString settingsRepeatModeStr = "repeat";
static const QString settingsLastMainViewType = "lastviewtype";
static const QString settingsVolumeStr = "volume";

// Public constructor
SoundManager::SoundManager(QSettings *settings)
{
    _settings = settings;

#ifdef QT_DEBUG
    QStringList args = VlcCommon::args() << "--verbose=1";
#else
    QStringList args = VlcCommon::args() << "--verbose=0";
#endif

    // Load the engine
    _vlcInstance = new VlcInstance(args, this);
    _vlcMediaPlayer = new VlcMediaPlayer(_vlcInstance);
    _vlcAudio = new VlcAudio(_vlcMediaPlayer);

    connect(_vlcMediaPlayer, &VlcMediaPlayer::timeChanged, this, &SoundManager::timeChanged);
    connect(_vlcMediaPlayer, &VlcMediaPlayer::timeChanged, this, &SoundManager::formatedTimeChanged);
    connect(_vlcMediaPlayer, &VlcMediaPlayer::lengthChanged, this, &SoundManager::formatedTimeChanged);

    connect(_vlcMediaPlayer, &VlcMediaPlayer::playing, this, &SoundManager::isPlayingChanged);
    connect(_vlcMediaPlayer, &VlcMediaPlayer::paused, this, &SoundManager::isPlayingChanged);
    connect(_vlcMediaPlayer, &VlcMediaPlayer::stopped, this, &SoundManager::isPlayingChanged);

    // On end handler
    connect(_vlcMediaPlayer, &VlcMediaPlayer::end, this, &SoundManager::onEndReachedHandler, Qt::QueuedConnection);

    // On error handler
    connect(_vlcMediaPlayer, &VlcMediaPlayer::error, [this](){
        qWarning() << VlcError::errmsg();
    });

    updatePlaylistsData();

    //
    // Load settings
    //

    _settings->beginGroup(settingsGroupStr);
    setRandom(_settings->value(settingsRandomStr, false).toBool());
    setRepeatMode(static_cast<RepeatMode>(_settings->value(settingsRepeatModeStr, 0).toInt()));
    _lastMainViewType = static_cast<MainViewType>(_settings->value(settingsLastMainViewType, 0).toInt());
    setVolume(_settings->value(settingsVolumeStr, -1).toInt());
    _settings->endGroup();

    //
    // Update media indexes
    //

    // Check if map files exists
    if(QFileInfo::exists(MediaInfo::albumMapFilePath())
       && QFileInfo::exists(MediaInfo::artistMapFilePath())
       && QFileInfo::exists(MediaInfo::trackListFilePath()))
    {
        // Medias are already parsed, send the signal
        _mediaListReady = true;
        emit mediaListReadyChanged();
    }

    // Start the process for searching new media
    QProcess *musicSearchProcess = new QProcess(this);
    connect(musicSearchProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            [this](int exitCode, QProcess::ExitStatus exitStatus) {
        if(exitCode == 0 && exitStatus == QProcess::NormalExit)
        {
            _mediaListReady = true;
            emit mediaListReadyChanged();
        }
    });
    musicSearchProcess->start(QCoreApplication::applicationDirPath() + QStringLiteral("/musicindex-generator"));
}

SoundManager::~SoundManager()
{
    delete _vlcAudio;
    delete _currentMedia;
    delete _vlcMediaPlayer;
    delete _vlcInstance;
}

//
// Private slot
//

void SoundManager::onEndReachedHandler()
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

void SoundManager::playFromFile(QString path, QString xmlSourceFile, const QString &xmlSourceQuery)
{
    if(_currentMedia != nullptr)
    {
        delete _currentMedia;
        _currentMedia = nullptr;
    }

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

            // Set the path to the first entry if the path is empty
            if(path.isEmpty() && it == nodes.begin())
                path = trackPath;

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
        if(path.isEmpty())
            return;

        _currentMedia = new VlcMedia(path, true, _vlcInstance);
        _currentMediaList.append(new MediaInfo(_currentMedia, path));
    }

    // Only play if a media was found
    if(_currentMedia != nullptr)
    {
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

        // Must be after startedChanged() signal
        emit newMediaPlayedFromFile();
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
    else if(isPlaying())// Restart the media
        _vlcMediaPlayer->setTime(0);
}

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
    else if(!_currentMediaList.isEmpty())
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

SoundManager::MainViewType SoundManager::lastMainViewType() const
{
    return _lastMainViewType;
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
// Playlist stuff
//

void SoundManager::updatePlaylistsData()
{
    _playlistNames.clear();
    _playlistFiles.clear();

    const QString playlistDirPath = MediaInfo::playlistsDirectory();

    if(!QDir(playlistDirPath).exists())
    {
        QDir playlistDir(playlistDirPath);
        playlistDir.mkpath(".");
    }

    QDirIterator it(playlistDirPath,
                    QStringList({"*.xml"}),
                    QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Writable | QDir::Readable);

    while(it.hasNext())
    {
        QString path = it.next();
        pugi::xml_document xmlDoc;
        if(!xmlDoc.load_file(path.toStdString().c_str()))
        {
            qWarning() << "Cannot open file: " << path << "for parsing !";
            continue;
        }

        QString name = xmlDoc.child("playlist").attribute("name").value();

        if(!name.isEmpty())
        {
            _playlistFiles.append(path);
            _playlistNames.append(name);
        }
    }
}

QString SoundManager::createNewPlaylist(QString name) // returns file name
{
    QString currentTime = QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss-zzz");
    QString fileName = MediaInfo::playlistsDirectory() + QStringLiteral("/playlist-") + currentTime + QStringLiteral(".xml");

    // Check if the file exist
    while(QFile::exists(fileName))
    {
        // Append "*" at the end
        fileName.insert(fileName.size() - 4, "*"); // 4 is the size of ".xml"
    }

    QFile file(fileName);
    if(file.open(QFile::ReadWrite | QFile::Text))
    {
        QXmlStreamWriter writer(&file);
#ifdef QT_DEBUG
        writer.setAutoFormatting(true);
#endif
        writer.writeStartDocument();
        writer.writeStartElement("playlist");
        writer.writeAttribute("name", name);
        writer.writeEndElement();
        writer.writeEndDocument();

        file.close();

        updatePlaylistsData();

        return fileName;
    }

    qWarning() << "Cannot create playlist file: " << file.errorString();
    return "";
}

void SoundManager::addSongToPlaylist(QString playlistFileName, QString musicTitle, QString musicPath, QString musicCover)
{
    if(!QFile::exists(playlistFileName))
        return;

    pugi::xml_document xmlDoc;
    if(!xmlDoc.load_file(playlistFileName.toStdString().c_str()))
        return;

    pugi::xml_node newTrack = xmlDoc.child("playlist").append_child("track");
    newTrack.append_child("title").text().set(musicTitle.toStdString().c_str());
    newTrack.append_child("path").text().set(musicPath.toStdString().c_str());
    newTrack.append_child("cover").text().set(musicCover.toStdString().c_str());

    xmlDoc.save_file(playlistFileName.toStdString().c_str(), "\t", PUGI_SAVE_FORMAT, pugi::xml_encoding::encoding_utf8);
}

void SoundManager::playFromPlaylist(const QString &playlistFile)
{
    playFromFile("", QString("file://") + playlistFile, "/playlist/track");
}

void SoundManager::removePlaylistFile(const QString &fileName)
{
    if(QFile::exists(fileName) && QFile::remove(fileName))
    {
        updatePlaylistsData();
    }
}

void SoundManager::removeFromPlaylist(const QString &playlistFilePath, const QString &musicFile)
{
    QString filePath = playlistFilePath;
    if(filePath.startsWith("file://"))
        filePath.remove("file://");

    pugi::xml_document xmlDoc;
    if(!xmlDoc.load_file(filePath.toStdString().c_str()))
        return;

    pugi::xml_node toBeRemoved;
    bool found = false;

    pugi::xpath_node_set nodes = xmlDoc.select_nodes("/playlist/track");
    for(pugi::xpath_node_set::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        pugi::xml_node node = (*it).node();
        const QString trackPath = node.child_value("path");
        if(trackPath == musicFile)
        {
            found = true;
            toBeRemoved = node;
            break;
        }
    }

    if(found)
    {
        xmlDoc.child("playlist").remove_child(toBeRemoved);
        xmlDoc.save_file(filePath.toStdString().c_str(), "\t", PUGI_SAVE_FORMAT, pugi::xml_encoding::encoding_utf8);
    }
}

QStringList SoundManager::playlistNames()
{
    return _playlistNames;
}

QStringList SoundManager::playlistFiles()
{
    return _playlistFiles;
}

QUrl SoundManager::playlistUrl(QString file)
{
    return QUrl::fromLocalFile(file);
}

//
// History stuff
//

void SoundManager::addHistoryEntry(QUrl source, QString sourceFile, QString sourceQuery, QString headerText)
{
    _listViewHistory.push(ListViewHistoryEntry({source, sourceFile, sourceQuery, headerText}));
    emit hasHistoryEntryChanged();

    // Upadate last view type
    if(sourceQuery == "/artists/artist")
    {
        _lastMainViewType = MainViewType::Artists;
    }
    else if(sourceQuery == "/albums/album")
    {
        _lastMainViewType = MainViewType::Albums;
    }
    else if(sourceQuery == "/tracks/track")
    {
        _lastMainViewType = MainViewType::Tracks;
    }
    else if(sourceQuery == "/playlist")
    {
        _lastMainViewType = MainViewType::Playlists;
    }
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

//
// Settings
//

void SoundManager::saveSettings()
{
    _settings->beginGroup(settingsGroupStr);
    _settings->setValue(settingsRandomStr, random());
    _settings->setValue(settingsRepeatModeStr, static_cast<int>(repeatMode()));
    _settings->setValue(settingsLastMainViewType, static_cast<int>(lastMainViewType()));
    _settings->setValue(settingsVolumeStr, volume());
    _settings->endGroup();
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
