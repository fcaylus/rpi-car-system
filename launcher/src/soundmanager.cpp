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
#include <QUrl>
#include <QStringList>

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
static const QString settingsEqualizerStr = "eqconfig";

// Public constructor
SoundManager::SoundManager(QSettings *settings, QObject *parent): QObject(parent)
{
    _settings = settings;
}

void SoundManager::init()
{
#ifdef QT_DEBUG
    QStringList args = VlcCommon::args() << "--verbose=1";
#else
    QStringList args = VlcCommon::args() << "--verbose=0";
#endif

    // Load the engine
    _vlcInstance = new VlcInstance(args, this);
    _vlcMediaPlayer = new VlcMediaPlayer(_vlcInstance);
    _vlcEqualizer = new VlcEqualizer(_vlcMediaPlayer);
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
    _equalizerCurrentConfig = _settings->value(settingsEqualizerStr, 0).toInt();
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

    checkForDefaultEqualizerConfigs();
    initEqualizerConfigList();
    // Check if the equalizer idx is in bound, else set to 0
    if(_equalizerCurrentConfig < 0 || _equalizerCurrentConfig >= _equalizerList.size())
        _equalizerCurrentConfig = 0;
    setEqualizerConfig(_equalizerCurrentConfig);

    _init = true;

    qDebug() << "SoundManager initialized !";
}

bool SoundManager::initialized() const
{
    return _init;
}

SoundManager::~SoundManager()
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
    return Common::timeToString(_vlcMediaPlayer->time()) + QString(" / ") + Common::timeToString(_vlcMediaPlayer->length());
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
// Equalizer stuff
//

// Private
void SoundManager::checkForDefaultEqualizerConfigs()
{
    const QString defaultDirPath = MediaInfo::equalizerDefaultConfigDir();
    const QString customDirPath = MediaInfo::equalizerCustomConfigDir();
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
void SoundManager::initEqualizerConfigList()
{
    _equalizerList.clear();

    const QString path = MediaInfo::equalizerCustomConfigDir();
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
EqualizerConfig SoundManager::parseEqualizerConfigFile(const QString filePath, const QString fileName)
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
void SoundManager::saveEqualizerConfigs()
{
    for(EqualizerConfig eq: _equalizerList)
    {
        if(eq.modified)
        {
            const QString path = MediaInfo::equalizerCustomConfigDir() + "/" + eq.fileName;
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

void SoundManager::resetEqualizerConfig(int idx)
{
    if(idx < 0 || idx >= _equalizerList.size())
        return;

    EqualizerConfig eq = _equalizerList[idx];
    const QString customPath = MediaInfo::equalizerCustomConfigDir() + "/" + eq.fileName;
    const QString defaultPath = MediaInfo::equalizerDefaultConfigDir() + "/" + eq.fileName;
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

void SoundManager::setEqualizerConfig(int idx)
{
    if(idx >= 0 && idx < _equalizerList.size())
    {
        _equalizerCurrentConfig = idx;
        updateEqualizerConfig();
        emit newEqualizerConfigLoaded();
    }
}

void SoundManager::updateEqualizerConfig()
{
    EqualizerConfig eq = _equalizerList.at(_equalizerCurrentConfig);
    _vlcEqualizer->setPreamplification(eq.preamplification);

    for(int i=0; i < eq.frequencies.size(); ++i)
        _vlcEqualizer->setAmplificationForBandAt(eq.amplifications[i], i);

    _vlcEqualizer->setEnabled(true);
    emit equalizerConfigChanged();
}

// Setters
void SoundManager::setEqualizerPreamp(float preamp)
{
    _equalizerList[_equalizerCurrentConfig].preamplification = preamp;
    _equalizerList[_equalizerCurrentConfig].modified = true;
    _vlcEqualizer->setPreamplification(preamp);
    _vlcEqualizer->setEnabled(true);
    emit equalizerConfigChanged();
}

void SoundManager::setEqualizerAmp(int freqId, float amp)
{
    _equalizerList[_equalizerCurrentConfig].amplifications[freqId] = amp;
    _equalizerList[_equalizerCurrentConfig].modified = true;
    _vlcEqualizer->setAmplificationForBandAt(freqId, amp);
    _vlcEqualizer->setEnabled(true);
    emit equalizerConfigChanged();
}

void SoundManager::increaseEqualizerPreamp(float inc)
{
    float v = equalizerPreamplification() + inc;
    if(v > 20.0f)
        v = 20.0f;
    else if (v < -20.0f)
        v = -20.0f;

    setEqualizerPreamp(v);
}

void SoundManager::increaseEqualizerAmp(int freqId, float inc)
{
    float v = equalizerAmplification(freqId) + inc;
    if(v > 20.0f)
        v = 20.0f;
    else if (v < -20.0f)
        v = -20.0f;

    setEqualizerAmp(freqId, v);
}

int SoundManager::currentEqualizerConfigId()
{
    return _equalizerCurrentConfig;
}

bool SoundManager::isEqualizerConfigModified(int idx)
{
    if(idx >= 0 && idx < _equalizerList.size())
        return _equalizerList[idx].modified;
    return false;
}

int SoundManager::nbOfFrequenciesAvailable()
{
    return _vlcEqualizer->bandCount();
}

QStringList SoundManager::equalizerConfigListNames()
{
    QStringList names;
    for(EqualizerConfig eq: _equalizerList)
        names.append(eq.name);
    return names;
}

QString SoundManager::equalizerConfigName()
{
    return _equalizerList.at(_equalizerCurrentConfig).name;
}

float SoundManager::equalizerFrequency(int idx)
{
    return _equalizerList.at(_equalizerCurrentConfig).frequencies[idx];
}

QString SoundManager::equalizerFrequencyString(int idx)
{
    return Common::unitToString(equalizerFrequency(idx), "Hz", 2);
}

float SoundManager::equalizerAmplification(int idx)
{
    return _equalizerList.at(_equalizerCurrentConfig).amplifications[idx];
}

QString SoundManager::equalizerAmplificationString(int idx)
{
    return Common::unitToString(equalizerAmplification(idx), "dB", 1, false);
}

float SoundManager::equalizerPreamplification()
{
    return _equalizerList.at(_equalizerCurrentConfig).preamplification;
}

QString SoundManager::equalizerPreamplificationString()
{
    return Common::unitToString(equalizerPreamplification(), "dB", 1, false);
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

    if(!QDir(playlistDirPath).exists())
        return;

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


