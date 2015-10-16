#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <vlcpp/vlc.hpp>

// Singleton sound manager
class SoundManager
{
    public:
        static SoundManager& instance();

        void playFromFile(const std::string& path);

    private:

        VLC::Instance *_vlcInstance;
        VLC::MediaPlayer *_vlcMediaPlayer;
        VLC::Media *_currentMedia;

        SoundManager(); // ctor
        ~SoundManager(); // dtor

        static SoundManager _instance;
};
#endif // SOUNDMANAGER_H
