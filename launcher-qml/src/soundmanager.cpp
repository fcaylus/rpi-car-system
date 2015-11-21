#include "soundmanager.h"

#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDir>
#include <QDirIterator>
#include <QSet>
#include <QStringList>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Audio.h>

#include <algorithm>

#include "pugixml.hpp"

#include "dirutility.h"

// Public constructor
SoundManager::SoundManager()
{
    _indexFilePath = QCoreApplication::applicationDirPath() + QStringLiteral("/music-index.txt");
    _artistMapFilePath = QCoreApplication::applicationDirPath() + QStringLiteral("/artist-map.xml");
    _albumMapFilePath = QCoreApplication::applicationDirPath() + QStringLiteral("/album-map.xml");
    _trackListFilePath = QCoreApplication::applicationDirPath() + QStringLiteral("/track-list.xml");

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
    connect(_vlcMediaPlayer, &VlcMediaPlayer::end, this, [this](){
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

    _musicFileFormats << QStringLiteral("*.mp3") << QStringLiteral("*.m4a") << QStringLiteral("*.m4p") << QStringLiteral("*.ogg")
                         << QStringLiteral("*.wav") << QStringLiteral("*.wma") << QStringLiteral("*.dct") << QStringLiteral("*.dss")
                         << QStringLiteral("*.act") << QStringLiteral("*.ivs") << QStringLiteral("*.gsm") << QStringLiteral("*.dvf")
                         << QStringLiteral("*.amr") << QStringLiteral("*.mmf") << QStringLiteral("*.3gp") << QStringLiteral("*.mpc")
                         << QStringLiteral("*.msv") << QStringLiteral("*.aac") << QStringLiteral("*.oga") << QStringLiteral("*.raw")
                         << QStringLiteral("*.sln") << QStringLiteral("*.tta") << QStringLiteral("*.vox") << QStringLiteral("*.ape")
                         << QStringLiteral("*.awb") << QStringLiteral("*.aiff") << QStringLiteral("*.flac") << QStringLiteral("*.opus")
                         << QStringLiteral("*.webm") << QStringLiteral("*.au") << QStringLiteral("*.ra") << QStringLiteral("*.rm")
                         << QStringLiteral("*.wv");
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
    _mediaListReadyMutex.lock();
    const bool ready = _mediaListReady;
    _mediaListReadyMutex.unlock();
    return ready;
}

QUrl SoundManager::albumMapFilePath() const
{
    return QUrl::fromLocalFile(_albumMapFilePath);
}

QUrl SoundManager::artistMapFilePath() const
{
    return QUrl::fromLocalFile(_artistMapFilePath);
}

QUrl SoundManager::trackListFilePath() const
{
    return QUrl::fromLocalFile(_trackListFilePath);
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

void SoundManager::checkForNewMusicFiles()
{
    MediaList mediaList;

    // If the index file doesn't exist, create it
    if(!QFile::exists(_indexFilePath))
    {
        recreateMusicIndex(mediaList);

        while(!mediaList.isEmpty())
            delete mediaList.takeLast();

        return;
    }

    // Send the media list before update
    updateMediaList(mediaList);

    // Here, the index file exist, but maybe it's not up to date
    // Refresh it !

    QFile indexFile(_indexFilePath);
    if(!indexFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "Cannot open the index file at:" << _indexFilePath;
        return;
    }
    QTextStream in(&indexFile);

    const QString indexFileNewPath = _indexFilePath + QStringLiteral(".new");
    QFile indexFileNew(indexFileNewPath);
    if(!indexFileNew.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qCritical() << "Cannot open a temp index file at:" << indexFileNewPath;
        return;
    }
    QTextStream out(&indexFileNew);


    // Check first line
    QString line = in.readLine();
    if(line != QStringLiteral("v2"))
    {
        // The file is not in the correct version, delete it !
        indexFile.close();
        indexFileNew.close();
        indexFile.remove();
        indexFileNew.remove();
        return;
    }
    out << line << "\n";

    //
    // First step, parse each line in the index to find files to remove and files to update
    //

    bool changeOccur = false; // Used to know if an update, delete or add operation has occured

    QString currentFilePath;
    QSet<QString> filesList;

    bool needToRemove = false;
    bool needToUpdate = false;

    int varCount = 0; // Used to check if the media have the correct number of argument
    while(!in.atEnd())
    {
        line = in.readLine();

        if(!line.isEmpty())
        {
            varCount += 1;
            if(varCount == 1)
            {
                currentFilePath = line;
                if(!QFile::exists(currentFilePath))
                {
                    needToRemove = true;
                    changeOccur = true;
                }
                else
                    filesList.insert(currentFilePath);
            }
            else if(varCount == 2 && !needToRemove)
            {
                const long size = line.toLong();
                const long realSize = QFileInfo(currentFilePath).size();
                if(size != realSize)
                {
                    // Need to update all values
                    needToUpdate = true;
                    changeOccur = true;

                    VlcMedia *media = new VlcMedia(currentFilePath, true, _vlcInstance);
                    MediaInfo *info = new MediaInfo(media, currentFilePath);

                    out << realSize << "\n"
                        << info->title() << "\n"
                        << info->artist() << "\n"
                        << info->album() << "\n"
                        << info->trackNumber() << "\n"
                        << info->coverFile() << "\n";

                    delete info;
                    delete media;
                }
            }

            if(!needToRemove && !needToUpdate)
                out << line << "\n";
        }
        else
        {
            // We reach the end of the media
            if(!needToRemove)
                out << "\n";

            needToRemove = false;
            needToUpdate = false;
            varCount = 0;
        }
    }
    indexFile.close();

    //
    // Second step: list all files and check if some are missing from the index
    //

    QDirIterator it(musicDirectory(), _musicFileFormats, QDir::Files | QDir::Readable | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        QString path = it.next();
        // If not in the index file
        if(!filesList.contains(path))
        {
            // Append the file to the index
            VlcMedia *media = new VlcMedia(path, true, _vlcInstance);
            MediaInfo *info = new MediaInfo(media, path);

            out << path << "\n"
                << info->fileSize() << "\n"
                << info->title() << "\n"
                << info->artist() << "\n"
                << info->album() << "\n"
                << info->trackNumber() << "\n"
                << info->coverFile() << "\n\n";

            delete info;
            delete media;

            changeOccur = true;
        }
    }

    indexFileNew.close();

    // Remove the old file and copy the new one
    indexFile.remove();
    indexFileNew.rename(_indexFilePath);

    // Send the media list after update (only if there are changes)
    if(changeOccur)
    {
        updateMediaList(mediaList);
        createMediaMapsFromList(mediaList);
    }

    // Now delete the media list
    while(!mediaList.isEmpty())
        delete mediaList.takeLast();

    _mediaListReadyMutex.lock();
    _mediaListReady = true;
    _mediaListReadyMutex.unlock();
    emit mediaListReadyChanged();
}

void SoundManager::recreateMusicIndex(MediaList &mediaList)
{   
    QFile indexFile(_indexFilePath);
    if(!indexFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qCritical() << "Cannot open the index file at:" << _indexFilePath;
        return;
    }
    QTextStream out(&indexFile);

    // Output the version
    out << "v2\n";

    QDirIterator it(musicDirectory(), _musicFileFormats, QDir::Files | QDir::Readable | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        QString path = it.next();

        //
        // Structure of each media in the index file
        // (blank lines separate the media):
        //
        //    <filePath>
        //    <fileSize>  (used to check if the file has changed)
        //    <musicTitle>
        //    <musicArtist>
        //    <musicAlbum>
        //    <trackNumber>
        //    <coverFilePath>
        //

        VlcMedia *media = new VlcMedia(path, true, _vlcInstance);
        MediaInfo *info = new MediaInfo(media, path);
        delete media;
        mediaList.append(info);

        out << path << "\n"
            << info->fileSize() << "\n"
            << info->title() << "\n"
            << info->artist() << "\n"
            << info->album() << "\n"
            << info->trackNumber() << "\n"
            << info->coverFile() << "\n\n";
    }

    indexFile.close();

    createMediaMapsFromList(mediaList);
}

// Private
void SoundManager::updateMediaList(MediaList &mediaList)
{
    QFile indexFile(_indexFilePath);
    if(!indexFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "Cannot open the index file at:" << _indexFilePath;
        return;
    }
    QTextStream in(&indexFile);

    // Check first line
    QString line = in.readLine();
    if(line != QStringLiteral("v2"))
    {
        indexFile.close();
        return;
    }

    QString fileName;
    QString artist;
    QString album;
    QString title;
    QString coverPath;
    int trackNumber = 0;
    long fileSize = -1;

    int varCount = 0;
    while(!in.atEnd())
    {
        line = in.readLine();
        if(!line.isEmpty())
        {
            varCount += 1;
            if(varCount == 1)
                fileName = line;
            else if(varCount == 2)
                fileSize =line.toLong();
            else if(varCount == 3)
                title = line;
            else if(varCount == 4)
                artist = line;
            else if(varCount == 5)
                album = line;
            else if(varCount == 6)
                trackNumber = line.toInt();
            else if(varCount == 7)
                coverPath = line;
        }
        else if(!fileName.isEmpty() && !artist.isEmpty()
                && !album.isEmpty() && !title.isEmpty()
                && !coverPath.isEmpty() && fileSize != -1
                && varCount == 7)
        {
            varCount = 0;

            MediaInfo *info = new MediaInfo();
            info->setAlbum(album);
            info->setArtist(artist);
            info->setCoverFile(coverPath);
            info->setFilePath(fileName);
            info->setTrackNumber(trackNumber);
            info->setFileSize(fileSize);
            info->setTitle(title);

            mediaList.append(info);

            // Reset vars
            fileName = "";
            fileSize = -1;
            title = "";
            artist = "";
            album = "";
            coverPath = "";
            trackNumber = 0;
        }
    }

    indexFile.close();
}

void SoundManager::createMediaMapsFromList(MediaList &mediaList)
{
    SortedMediaMap artistMediaMap;
    MediaMap albumMediaMap;

    MediaMap artistMapTemp;

    //
    // First step, list all artist and all albums

    for(MediaInfo *info : mediaList)
    {
        // Not yet in the list
        if(!albumMediaMap.contains(info->album()))
        {
            MediaList trackList;
            trackList.append(info);
            albumMediaMap[info->album()] = trackList;
        }
        else
            albumMediaMap[info->album()].append(info);

        if(!artistMapTemp.contains(info->artist()))
        {
            MediaList trackList;
            trackList.append(info);
            artistMapTemp[info->artist()] = trackList;
        }
        else
            artistMapTemp[info->artist()].append(info);
    }

    //
    // Second step, sort albums in artist map

    for(MediaMap::iterator it = artistMapTemp.begin(); it != artistMapTemp.end(); ++it)
    {
        MediaMap tempAlbumMap;

        for(MediaInfo *info : it.value())
        {
            if(!tempAlbumMap.contains(info->album()))
            {
                MediaList trackList;
                trackList.append(info);
                tempAlbumMap[info->album()] = trackList;
            }
            else
                tempAlbumMap[info->album()].append(info);
        }

        // Sort the map
        for(MediaMap::iterator it2 = tempAlbumMap.begin(); it2 != tempAlbumMap.end(); ++it2)
            std::sort(it2.value().begin(), it2.value().end(), MediaInfo::sort);

        // Copy
        artistMediaMap[it.key()] = tempAlbumMap;
    }

    //
    // Third step, order tracks in album list
    for(MediaMap::iterator it = albumMediaMap.begin(); it != albumMediaMap.end(); ++it)
        std::sort(it.value().begin(), it.value().end(), MediaInfo::sort);

    //
    // Last step, order media list
    std::sort(mediaList.begin(), mediaList.end(), MediaInfo::sortAlpha);


    //
    // Here, all maps are created, we just have to write them to files
    //

    DirUtility::removeIfExists(_albumMapFilePath);
    DirUtility::removeIfExists(_artistMapFilePath);
    DirUtility::removeIfExists(_trackListFilePath);

    QFile albumMapFile(_albumMapFilePath);
    if(!albumMapFile.open(QIODevice::ReadWrite | QIODevice::Text))
        return;

    QFile artistMapFile(_artistMapFilePath);
    if(!artistMapFile.open(QIODevice::ReadWrite | QIODevice::Text))
        return;

    QFile trackListFile(_trackListFilePath);
    if(!trackListFile.open(QIODevice::ReadWrite | QIODevice::Text))
        return;

    QXmlStreamWriter streamAlbum(&albumMapFile);
    QXmlStreamWriter streamArtist(&artistMapFile);
    QXmlStreamWriter streamTrack(&trackListFile);

#ifdef QT_DEBUG
    streamAlbum.setAutoFormatting(true);
    streamArtist.setAutoFormatting(true);
    streamTrack.setAutoFormatting(true);
#endif

    //
    // Write album

    streamAlbum.writeStartDocument();
    streamAlbum.writeStartElement(QStringLiteral("albums"));

    for(MediaMap::iterator it = albumMediaMap.begin(); it != albumMediaMap.end(); ++it)
    {
        streamAlbum.writeStartElement(QStringLiteral("album"));
        streamAlbum.writeAttribute(QStringLiteral("name"), it.key());
        // Use the first cover file as album cover
        streamAlbum.writeAttribute(QStringLiteral("cover"), QStringLiteral("file://") + it.value().first()->coverFile());

        for(MediaInfo *info: it.value())
            info->toXml(&streamAlbum);

        streamAlbum.writeEndElement();
    }
    streamAlbum.writeEndElement();
    streamAlbum.writeEndDocument();
    albumMapFile.close();

    //
    // Write artist

    streamArtist.writeStartDocument();
    streamArtist.writeStartElement(QStringLiteral("artists"));

    for(SortedMediaMap::iterator it = artistMediaMap.begin(); it != artistMediaMap.end(); ++it)
    {
        streamArtist.writeStartElement(QStringLiteral("artist"));
        streamArtist.writeAttribute(QStringLiteral("name"), it.key());

        for(MediaMap::iterator it2 = it.value().begin(); it2 != it.value().end(); ++it2)
        {
            streamArtist.writeStartElement(QStringLiteral("album"));
            streamArtist.writeAttribute(QStringLiteral("name"), it2.key());
            streamArtist.writeAttribute(QStringLiteral("cover"), QStringLiteral("file://") + it2.value().first()->coverFile());

            for(MediaInfo *info: it2.value())
                info->toXml(&streamArtist);

            streamArtist.writeEndElement();
        }
        streamArtist.writeEndElement();
    }
    streamArtist.writeEndElement();
    streamArtist.writeEndDocument();
    artistMapFile.close();

    //
    // Write tracks

    streamTrack.writeStartDocument();
    streamTrack.writeStartElement(QStringLiteral("tracks"));

    for(MediaInfo *info: mediaList)
        info->toXml(&streamTrack);

    streamTrack.writeEndElement();
    streamTrack.writeEndDocument();
    trackListFile.close();
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
QString SoundManager::musicDirectory()
{
#ifdef READY_FOR_CARSYSTEM
    return QStringLiteral("/home/pi/music");
#else
    //return QStringLiteral("/media/fabien/Disque Multimedia/Music/Ma musique");
    return QStringLiteral("/home/fabien/Musique");
#endif
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
