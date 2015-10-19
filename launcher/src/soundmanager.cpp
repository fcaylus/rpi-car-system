#include "soundmanager.h"

#include "easylogging++.h"
#include "utility.h"

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

    _currentMedia->parse();

    std::string coverPath = DirUtility::filePathForURI(_currentMedia->meta(libvlc_meta_ArtworkURL));
    // Replace with the default artwork if no cover
    if(coverPath.empty())
        coverPath = DirUtility::executableDir() + "/theme/default_artwork.png";

    _onNewMediaHandler(MediaInfo({_currentMedia->meta(libvlc_meta_Title),
                                 _currentMedia->meta(libvlc_meta_Artist),
                                 _currentMedia->meta(libvlc_meta_Album),
                                 coverPath,
                                 _vlcMediaPlayer->length()}),
                                 true);
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

