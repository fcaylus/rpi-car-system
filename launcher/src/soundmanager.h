#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <vlcpp/vlc.hpp>
#include <functional>
#include "dirutility.h"

struct MediaInfo
{
    std::string title;
    std::string artist;
    std::string album;
    std::string coverFile;
    libvlc_time_t length;

    std::string filePath;
    long fileSize;

    static MediaInfo fromMedia(VLC::Media *media, std::string path = "")
    {
        if(path.empty())
            path = DirUtility::filePathForURI(media->mrl());

        if(!media->isParsed())
            media->parse();

        std::string coverPath = DirUtility::filePathForURI(media->meta(libvlc_meta_ArtworkURL));
        // Replace with the default artwork if no cover
        if(coverPath.empty())
            coverPath = DirUtility::executableDir() + "/theme/default_artwork.png";

        return MediaInfo({media->meta(libvlc_meta_Title),
                          media->meta(libvlc_meta_Artist),
                          media->meta(libvlc_meta_Album),
                          coverPath,
                          media->duration(),
                          path,
                          DirUtility::fileSize(path)});
    }
};

// Singleton sound manager
class SoundManager
{
    public:
        static SoundManager& instance();
        ~SoundManager(); // dtor

        enum RepeatMode {
            NoRepeat = 0,
            RepeatAll = 1,
            RepeatOne = 2
        };

        void playFromFile(const std::string& path);

        void pauseMusic();
        void resumeMusic();

        bool isPlaying();

        void nextMusic();
        void previousMusic();

        void forward();
        void rewind();

        void setRepeatMode(RepeatMode mode);
        void setRandom(bool random);

        int volume() const;
        void setVolume(int volume);

        void setOnNewMediaHandler(std::function<void(MediaInfo /*info*/, bool /*autoPlay*/)> handler);
        void setOnNewTimeHandler(std::function<void(libvlc_time_t /*new_time_in_ms*/)> handler);
        void setOnEndReachedHandler(std::function<void(void)> handler);

        // Check if there are new files in the music dir
        void checkForNewMusicFiles();

        // Re-create the entire index file
        void recreateMusicIndex();

        // Convert time in ms to "hh:mm:ss"
        static std::string timeToString(libvlc_time_t time);

        static bool isMusicFileFormat(const std::string& file);
        static std::string musicDirectory();

    private:

        VLC::Instance *_vlcInstance;
        VLC::MediaPlayer *_vlcMediaPlayer;
        VLC::Media *_currentMedia;

        // Play parameters
        bool _playRandom = false;
        RepeatMode _repeatMode = NoRepeat;

        bool _endReached = false;

        std::string _indexFilePath;

        std::function<void(MediaInfo, bool)> _onNewMediaHandler = [](MediaInfo, bool){};
        std::function<void(void)> _onEndReachedHandler = [](){};

        SoundManager(); // ctor

        static SoundManager _instance;
};
#endif // SOUNDMANAGER_H
