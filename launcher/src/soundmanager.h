#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <vlcpp/vlc.hpp>
#include <functional>
#include <map>
#include <cstdio>

#include "dirutility.h"

struct MediaInfo
{
    std::string title;
    std::string artist;
    std::string album;
    int trackNumber;
    std::string coverFile;

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

        MediaInfo info({media->meta(libvlc_meta_Title),
                        media->meta(libvlc_meta_Artist),
                        media->meta(libvlc_meta_Album),
                        0,
                        coverPath,
                        path,
                        DirUtility::fileSize(path)});

        const std::string trackNbStr = media->meta(libvlc_meta_TrackNumber);
        if(!trackNbStr.empty())
            info.trackNumber = std::stoi(trackNbStr);

        if(info.title.empty())
            info.title = path;
        if(info.artist.empty())
            info.artist = "Artiste Inconnu";
        if(info.album.empty())
            info.album = "Album inconnu";

        return info;
    }

    // Sort by trackNumber, and then, by fileName
    static bool sort(const MediaInfo& m1, const MediaInfo& m2)
    {
        if(m1.trackNumber == m2.trackNumber)
            return DirUtility::fileNameFromFullPathWithoutDot(m1.filePath).compare(DirUtility::fileNameFromFullPathWithoutDot(m2.filePath)) < 0;
        else
            return m1.trackNumber < m2.trackNumber;
    }

    // Sort by title (used by the tracks view)
    static bool sortAlpha(const MediaInfo &m1, const MediaInfo &m2)
    {
        return m1.title.compare(m2.title) < 0;
    }
};

typedef std::vector<MediaInfo> MediaList;
typedef std::map<std::string, MediaList> MediaMap;
// For examples:
//      artists -> albums -> trakcs
//      albums -> artists -> tracks
typedef std::map<std::string, MediaMap> SortedMediaMap;

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

        // TODO: delete this method
        void playFromFile(const std::string& path);
        void playFromMedia(const MediaInfo& info);

        void onNewStreamingData(unsigned char* newData, ssize_t length);

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

        void setOnNewMediaHandler(std::function<void(MediaInfo /*info*/, libvlc_time_t /*duration*/, bool /*autoPlay*/)> handler);
        void setOnNewTimeHandler(std::function<void(libvlc_time_t /*new_time_in_ms*/)> handler);
        void setOnEndReachedHandler(std::function<void(void)> handler);

        void setOnMediaListUpdatedHandler(std::function<void(MediaList /*mediaList*/,
                                                             MediaMap /*albumMap*/,
                                                             SortedMediaMap /*artistMap*/)> handler);


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
        MediaList _currentMediaQueue;

        // Play parameters
        bool _playRandom = false;
        RepeatMode _repeatMode = NoRepeat;

        bool _endReached = false;

        bool _inStream = false;
        FILE* _stream;
        char* _streamBuffer;
        size_t _streamLength;


        std::string _indexFilePath;
        MediaList _mediaList;

        SortedMediaMap _artistMediaMap;
        MediaMap _albumMediaMap;

        std::function<void(MediaList, MediaMap, SortedMediaMap)> _onMediaListUpdatedHandler = [](MediaList, MediaMap, SortedMediaMap){};
        std::function<void(MediaInfo, libvlc_time_t, bool)> _onNewMediaHandler = [](MediaInfo, libvlc_time_t, bool){};
        std::function<void(void)> _onEndReachedHandler = [](){};

        void updateMediaListObject();
        void createMediaMapsFromList();

        SoundManager(); // ctor

        static SoundManager _instance;
};


// Custom Streaming media
class StreamingMedia : VLC::Media
{
    private:
        FILE *_stream;
        long _streamPos = 0;

    public:

        FILE* stream()
        {
            return _stream;
        }

        long streamPos()
        {
            return _streamPos;
        }

        void setStreamPos(long pos)
        {
            _streamPos = pos;
        }

        // Read callback
        static ssize_t readCallback(void *opaque, unsigned char *buf, size_t len)
        {
            size_t result = 0;
            StreamingMedia *media = static_cast<StreamingMedia*>(opaque);

            fseek(media->stream(), media->streamPos(), SEEK_SET);
            while(result != len)
                result = fread(buf, 1, len, media->stream());

            media->setStreamPos(media->streamPos() + len);
        }

        // Ctor
        StreamingMedia(VLC::Instance &instance, FILE* stream)/*: VLC::Media(instance, nullptr,
                                                                          StreamingMedia::readCallback, nullptr, nullptr)*/
        {

        }
};




#endif // SOUNDMANAGER_H
