#include "soundmanager.h"

#include "easylogging++.h"

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

    _currentMedia = new VLC::Media(*_vlcInstance, path, VLC::Media::FromPath);
    _vlcMediaPlayer->stop();
    _vlcMediaPlayer->setMedia(*_currentMedia);
    _vlcMediaPlayer->play();

    LOG(INFO) << "Play media from file: " << path;
}

// Private constructor
SoundManager::SoundManager()
{
    // Load the engine
    _vlcInstance = new VLC::Instance(0, nullptr);
    _vlcMediaPlayer = new VLC::MediaPlayer(*_vlcInstance);
}

// Private destructor
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
}

