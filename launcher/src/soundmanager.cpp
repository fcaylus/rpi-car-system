#include "soundmanager.h"

#include "easylogging++.h"

#include <lib/FileSystem.h>

#include <fstream>
#include <algorithm>
#include <set>
#include <stdio.h>


SoundManager SoundManager::_instance = SoundManager();

// Instance getter
SoundManager& SoundManager::instance()
{
    return _instance;
}

// Public methods

void SoundManager::playFromFile(const std::string &path)
{
    if(_currentMedia != nullptr)
        delete _currentMedia;

    _endReached = false;

    _currentMedia = new VLC::Media(*_vlcInstance, path, VLC::Media::FromPath);
    _vlcMediaPlayer->stop();
    _vlcMediaPlayer->setMedia(*_currentMedia);
    _vlcMediaPlayer->play();

    LOG(INFO) << "Play media from file: " << path;

    _onNewMediaHandler(MediaInfo::fromMedia(_currentMedia, path), true);
}

void SoundManager::pauseMusic()
{
    _vlcMediaPlayer->setPause(true);
}

void SoundManager::resumeMusic()
{
    _vlcMediaPlayer->setPause(false);

    if(_endReached)
    {
        _endReached = false;
        _vlcMediaPlayer->setMedia(*_currentMedia);
        _vlcMediaPlayer->play();
    }
}

bool SoundManager::isPlaying()
{
    return _vlcMediaPlayer->isPlaying();
}

void SoundManager::nextMusic()
{
    // TODO: implement
}

void SoundManager::previousMusic()
{
    // TODO: finish impelement

    // Re-start the media if we reach 3 seconds
    if(_vlcMediaPlayer->isPlaying() && _vlcMediaPlayer->time() > 3000)
        _vlcMediaPlayer->setTime(0);

    // Re-start if the media reach the end
    if(_endReached)
    {
        _endReached = false;
        _vlcMediaPlayer->setMedia(*_currentMedia);
        _vlcMediaPlayer->play();
    }
}

void SoundManager::forward()
{
    const libvlc_time_t newTime = _vlcMediaPlayer->time() + 30000;
    if(newTime > _vlcMediaPlayer->length())
        _vlcMediaPlayer->setTime(_vlcMediaPlayer->length());
    else
        _vlcMediaPlayer->setTime(newTime);
}

void SoundManager::rewind()
{
    const libvlc_time_t newTime = _vlcMediaPlayer->time() - 30000;
    if(newTime < 0)
        _vlcMediaPlayer->setTime(0);
    else
        _vlcMediaPlayer->setTime(newTime);
}

void SoundManager::setRandom(bool random)
{
    _playRandom = random;
}

void SoundManager::setRepeatMode(RepeatMode mode)
{
    _repeatMode = mode;
}

int SoundManager::volume() const
{
    return _vlcMediaPlayer->volume();
}

void SoundManager::setVolume(int volume)
{
    if(volume >= 0 && volume <= 100)
        _vlcMediaPlayer->setVolume(volume);
}

void SoundManager::setOnNewMediaHandler(std::function<void (MediaInfo, bool)> handler)
{
    _onNewMediaHandler = handler;
}

void SoundManager::setOnNewTimeHandler(std::function<void (libvlc_time_t)> handler)
{
    _vlcMediaPlayer->eventManager().onTimeChanged(handler);
}

void SoundManager::setOnEndReachedHandler(std::function<void ()> handler)
{
    _onEndReachedHandler = handler;
}

void SoundManager::setOnMediaListUpdatedHandler(std::function<void (std::vector<MediaInfo>)> handler)
{
    _onMediaListUpdatedHandler = handler;
}

void SoundManager::checkForNewMusicFiles()
{
    // If the index file doesn't exist, create it
    if(!ilixi::FileSystem::fileExists(_indexFilePath))
    {
        recreateMusicIndex();
        return;
    }

    // Send the media list before update
    updateMediaListObject();

    // Here, the index file exist, but maybe it's not up to date
    // Refresh it !

    std::ifstream indexFile;
    indexFile.open(_indexFilePath);

    std::ofstream indexFileNew;
    const std::string indexFileNewPath = _indexFilePath + ".new";
    indexFileNew.open(indexFileNewPath);

    std::string line;

    // Check first line
    std::getline(indexFile, line);
    if(line != "v1")
    {
        // The file is not in the correct version, recreate it !
        indexFile.close();
        indexFileNew.close();
        recreateMusicIndex();
        return;
    }

    indexFileNew << line << std::endl;

    //
    // First step, parse each line in the index to find files to remove and files to update
    //

    bool changeOccur = false; // Used to know if an update, delete or add operation has occured

    std::string currentFilePath = "";
    std::set<std::string> filesList;

    bool needToRemove = false;
    bool needToUpdate = false;

    int varCount = 0; // Used to check if the media have the correct number of argument
    for(; std::getline(indexFile, line);)
    {
        if(line != "")
        {
            varCount += 1;
            if(varCount == 1)
            {
                currentFilePath = line;
                if(!ilixi::FileSystem::fileExists(currentFilePath))
                {
                    needToRemove = true;
                    changeOccur = true;
                }
                else
                    filesList.insert(currentFilePath);
            }
            else if(varCount == 2 && !needToRemove)
            {
                const long size = std::stol(line);
                const long realSize = DirUtility::fileSize(currentFilePath);
                if(size != realSize)
                {
                    // Need to update all values
                    needToUpdate = true;
                    changeOccur = true;
                    VLC::Media *media = new VLC::Media(*_vlcInstance, currentFilePath, VLC::Media::FromPath);
                    MediaInfo info = MediaInfo::fromMedia(media, currentFilePath);
                    indexFileNew << realSize << std::endl
                                 << info.title << std::endl
                                 << info.artist << std::endl
                                 << info.album << std::endl
                                 << info.coverFile << std::endl;
                    delete media;
                }

            }

            if(!needToRemove && !needToUpdate)
                indexFileNew << line << std::endl;
        }
        else
        {
            // We reach the end of the media
            if(!needToRemove)
                indexFileNew << std::endl;

            needToRemove = false;
            needToUpdate = false;
            varCount = 0;
        }
    }

    //
    // Second step: list all files and check if some are missing from the index
    //

    indexFile.close();

    DirUtility::listAllFilesInDir(musicDirectory(), [this, &filesList, &indexFileNew, &changeOccur](const std::string& path) {
        if(isMusicFileFormat(path))
        {
            // If not in the index file
            if(filesList.find(path) == filesList.end())
            {
                // Append the file to the index
                VLC::Media *media = new VLC::Media(*_vlcInstance, path, VLC::Media::FromPath);
                MediaInfo info = MediaInfo::fromMedia(media, path);
                indexFileNew << path << std::endl
                             << info.fileSize << std::endl
                             << info.title << std::endl
                             << info.artist << std::endl
                             << info.album << std::endl
                             << info.coverFile << std::endl << std::endl;
                delete media;

                changeOccur = true;
            }
        }
    });

    indexFileNew.close();

    // Remove the old file and copy the new one
    remove(_indexFilePath.c_str());
    std::rename(indexFileNewPath.c_str(), _indexFilePath.c_str());

    // Send the media list after update (only if there are changes)
    if(changeOccur)
        updateMediaListObject();
}

void SoundManager::recreateMusicIndex()
{   
    std::ofstream indexFileStream;
    indexFileStream.open(_indexFilePath);

    // Output the version
    indexFileStream << "v1" << std::endl;

    _mediaList.clear();

    // Output all files name to the file
    DirUtility::listAllFilesInDir(musicDirectory(), [this, &indexFileStream](const std::string& path) {
        if(isMusicFileFormat(path))
        {
            //
            // Structure of each media in the index file
            // (blank lines separate the media):
            //
            //    <filePath>
            //    <fileSize>  (used to check if the file has changed)
            //    <musicTitle>
            //    <musicArtist>
            //    <musicAlbum>
            //    <coverFilePath>
            //

            VLC::Media *media = new VLC::Media(*_vlcInstance, path, VLC::Media::FromPath);
            MediaInfo info = MediaInfo::fromMedia(media, path);
            _mediaList.push_back(info);

            indexFileStream << path << std::endl
                            << DirUtility::fileSize(path) << std::endl
                            << info.title << std::endl
                            << info.artist << std::endl
                            << info.album << std::endl
                            << info.coverFile << std::endl << std::endl;

            delete media;
        }
    });

    indexFileStream.close();

    _onMediaListUpdatedHandler(_mediaList);
}

// Private
void SoundManager::updateMediaListObject()
{
    std::ifstream indexFile;
    indexFile.open(_indexFilePath);
    std::string line;
    std::getline(indexFile, line);
    if(line != "v1")
    {
        indexFile.close();
        return;
    }

    _mediaList.clear();

    std::string fileName = "";
    std::string artist = "";
    std::string album = "";
    std::string title = "";
    std::string coverPath = "";
    long fileSize = -1;

    int varCount = 0;
    while(std::getline(indexFile, line))
    {
        if(line != "")
        {
            varCount += 1;
            if(varCount == 1)
                fileName = line;
            else if(varCount == 2)
                fileSize = std::stol(line);
            else if(varCount == 3)
                title = line;
            else if(varCount == 4)
                artist = line;
            else if(varCount == 5)
                album = line;
            else if(varCount == 6)
                coverPath = line;
        }
        else if(!fileName.empty() && !artist.empty()
                && !album.empty() && !title.empty()
                && !coverPath.empty() && fileSize != -1
                && varCount == 6)
        {
            varCount = 0;

            // We can't use MediaInfo::fromMedia() because we just read the index file
            MediaInfo info;
            info.album = album;
            info.artist = artist;
            info.coverFile = coverPath;
            info.filePath = fileName;
            info.fileSize = fileSize;
            info.title = title;
            VLC::Media *media = new VLC::Media(*_vlcInstance, fileName, VLC::Media::FromPath);
            info.length = media->duration();
            delete media;

            _mediaList.push_back(info);

            fileName = "";
            fileSize = -1;
            title = "";
            artist = "";
            album = "";
            coverPath = "";
        }
    }

    indexFile.close();
    _onMediaListUpdatedHandler(_mediaList);
}

// Static
bool SoundManager::isMusicFileFormat(const std::string &file)
{
    // Based on the list at : https://en.wikipedia.org/wiki/Audio_file_format

    std::string last4 = file.substr(file.size() - 4);
    std::transform(last4.begin(), last4.end(), last4.begin(), ::tolower);
    if(last4 == ".mp3" || last4 == ".m4a" || last4 == ".m4p"  || last4 == ".ogg"
       || last4 == ".wav" || last4 == ".wma" || last4 == ".dct"  || last4 == ".dss"
       || last4 == ".act" || last4 == ".ivs" || last4 == ".gsm"  || last4 == ".dvf"
       || last4 == ".amr" || last4 == ".mmf" || last4 == ".3gp"  || last4 == ".mpc"
       || last4 == ".msv" || last4 == ".aac" || last4 == ".oga"  || last4 == ".raw"
       || last4 == ".sln" || last4 == ".tta" || last4 == ".vox"  || last4 == ".ape"
       || last4 == ".awb")
        return true;

    std::string last5 = file.substr(file.size() - 5);
    std::transform(last5.begin(), last5.end(), last5.begin(), ::tolower);
    if(last5 == ".aiff" || last5 == ".flac" || last5 == ".opus" || last5 == ".webm")
        return true;

    std::string last3 = file.substr(file.size() - 3);
    std::transform(last3.begin(), last3.end(), last3.begin(), ::tolower);

    return last3 == ".au" || last3 == ".ra" || last3 == ".rm" || last3 == ".wv";
}

// Static
std::string SoundManager::musicDirectory()
{
#ifdef READY_FOR_CARSYSTEM
    return "/home/pi/music";
#else
    //return "/media/fabien/Disque Multimedia/Music/Ma musique";
    return "/home/fabien/Musique";
#endif
}

// Static
std::string SoundManager::timeToString(libvlc_time_t time)
{
    const unsigned int timeInSeconds = time / 1000;
    const unsigned int hours = timeInSeconds / 3600;
    const unsigned int minutes = (timeInSeconds % 3600) / 60;
    const unsigned int secondes = timeInSeconds - hours * 3600 - minutes * 60;

    std::string result = "";

    if(hours != 0)
        result += std::to_string(hours) + std::string(":");

    if(minutes < 10 && hours != 0)
        result += std::string("0") + std::to_string(minutes);
    else
        result += std::to_string(minutes);

    result += ":";

    if(secondes < 10)
        result += std::string("0") + std::to_string(secondes);
    else
        result += std::to_string(secondes);

    return result;
}

// Private constructor
SoundManager::SoundManager()
{

    _indexFilePath = DirUtility::executableDir() + "/music-index.txt";

    // Load the engine
    _vlcInstance = new VLC::Instance(0, nullptr);
    _vlcMediaPlayer = new VLC::MediaPlayer(*_vlcInstance);

    // handle end of a media
    _vlcMediaPlayer->eventManager().onEndReached([this](){

        _onEndReachedHandler();

        _endReached = true;

       // Check if we are in repeat-one mode
        if(_repeatMode == RepeatOne)
        {
            _endReached = false;
            _vlcMediaPlayer->setMedia(*_currentMedia);
            _vlcMediaPlayer->play();
        }
    });

    // The music activity need to start the check process
    //checkForNewMusicFiles();
}

// Public destructor
SoundManager::~SoundManager()
{
    delete _vlcInstance;
    _vlcInstance = nullptr;

    if(_currentMedia != nullptr)
    {
        delete _currentMedia;
        _currentMedia = nullptr;
    }

    _vlcMediaPlayer->stop();

    delete _vlcMediaPlayer;
    _vlcMediaPlayer = nullptr;

    LOG(INFO) << "SoundManager deleted !";
}

