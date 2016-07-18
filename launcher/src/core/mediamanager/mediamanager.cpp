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

#include "mediamanager.h"
#include "usbsource.h"
#include "playlist.h"

#include "../common.h"
#include "../dirutility.h"

#include <QDebug>
#include <QXmlStreamWriter>

MediaManager MediaManager::_instance;

// Static
MediaManager* MediaManager::instance()
{
    return &_instance;
}

// Private
MediaManager::MediaManager()
{
    qDebug() << "MediaManager created !";
}

// Private
MediaManager::~MediaManager()
{
    _hotplugThread.requestInterruption();
    _hotplugThread.quit();
    _hotplugThread.wait();

    qDebug() << "MediaManager deleted !";
}

void MediaManager::initialScan()
{
    if(_init)
        return;

    //
    // Search for new USB sources
    //

    _sources.append(UsbSource::listAllUsbSources(this, true));
    qDebug() << "Available USB sources: ";
    for(MediaSource* source : _sources)
    {
        connect(source, &MediaSource::newMediaAvailable, this, &MediaManager::availableMediasChanged);
        connect(source, &MediaSource::disconnected, this, [this, source](){
            const int idx = _sources.indexOf((MediaSource *)source);
            if(idx != -1)
            {
                _lastList.clear();
                emit sourceAboutToBeDeleted(source);
                _sources.takeAt(idx)->deleteLater();
                emit availableMediasChanged();
                emit sourcesListChanged();
            }
        });

        qDebug() << source->property("devPath").toString() << source->property("mountPoint").toString();
    }

    emit availableMediasChanged();
    emit sourcesListChanged();

    //
    // Start the hotplug thread
    //

    UsbSourceUtil::HotplugWorker *worker = new UsbSourceUtil::HotplugWorker(this);
    worker->moveToThread(&_hotplugThread);
    connect(&_hotplugThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(&_hotplugThread, &QThread::started, worker, &UsbSourceUtil::HotplugWorker::start);
    connect(worker, &UsbSourceUtil::HotplugWorker::newUsbSourceAvailable, this, [this](UsbSource *source) {
        connect(source, &MediaSource::newMediaAvailable, this, &MediaManager::availableMediasChanged);
        connect(source, &MediaSource::disconnected, this, [this, source](){
            const int idx = _sources.indexOf((MediaSource *)source);
            if(idx != -1)
            {
                _lastList.clear();
                emit sourceAboutToBeDeleted(source);
                _sources.takeAt(idx)->deleteLater();
                emit availableMediasChanged();
                emit sourcesListChanged();
            }
        });

        _sources.append(source);
        emit availableMediasChanged();
        emit sourcesListChanged();
    });

    connect(worker, &UsbSourceUtil::HotplugWorker::usbSourceDisconnected, this, [this](const QString& sysPath) {
        for(MediaSource* source : _sources)
        {
            UsbSource *usb = qobject_cast<UsbSource *>(source);
            if(usb)
            {
                if(usb->sysPath() == sysPath)
                {
                    usb->requestDisconnection();
                    break;
                }
            }
        }
    });

    _hotplugThread.start();

    //
    // Search for playlists
    //

    _playlists = Playlist::findAllPlaylists(this);
    emit availablePlaylistsChanged();

    _init = true;
    emit scannedChanged();
}

bool MediaManager::scanned() const
{
    return _init;
}

bool MediaManager::cacheEnabled() const
{
    return _cacheEnabled;
}

void MediaManager::setCacheEnabled(bool enabled)
{
    _cacheEnabled = enabled;
}

bool MediaManager::isCachePersistent() const
{
    return _cachePersistent;
}

void MediaManager::setCachePersistent(bool persistent)
{
    _cachePersistent = persistent;
}

MediaSourceList MediaManager::sourcesList() const
{
    return _sources;
}

MediaInfoList MediaManager::availableMedias(MediaInfo::MetadataType onlyWithMeta, QVariant onlyWithMetaValue)
{
    _lastList.clear();

    if(_sources.isEmpty())
        _lastList = MediaInfoList();
    // This check avoid an unnecessary copy of the media list if there is only one source
    else if(_sources.length() == 1)
         _lastList = _sources.value(0)->availableMedias(onlyWithMeta, onlyWithMetaValue);
    else
    {
        for(MediaSource *src : _sources)
            _lastList.append(src->availableMedias(onlyWithMeta, onlyWithMetaValue));
    }

    //
    // Sort the list

    // Sort by title when retrieving all media
    if(onlyWithMeta == MediaInfo::UNKNOWN)
        std::sort(_lastList.begin(), _lastList.end(), MediaInfo::sortAlpha);
    // Sort by trackNumber when selecting an album
    else if(onlyWithMeta == MediaInfo::ALBUM)
        std::sort(_lastList.begin(), _lastList.end(), MediaInfo::sort);

    return _lastList;
}

MediaInfoList MediaManager::lastMediaList() const
{
    return _lastList;
}

QList<QVariantList> MediaManager::listMetadata(MetadataTypeList metadataToRetrieve,
                                               MediaInfo::MetadataType requiredMeta,
                                               QVariant requiredMetaValue)
{
    if(_sources.isEmpty())
        return QList<QVariantList>();

    if(_sources.length() == 1)
        return _sources.value(0)->listMetadata(metadataToRetrieve, requiredMeta, requiredMetaValue);

    // This first list is constitued of unique element
    QList<QVariantList> list = _sources.value(0)->listMetadata(metadataToRetrieve, requiredMeta, requiredMetaValue);
    for(int i=1, size=_sources.size() ; i < size ; ++i)
    {
        QList<QVariantList> srcList = _sources.value(i)->listMetadata(metadataToRetrieve, requiredMeta, requiredMetaValue);
        for(QVariantList vars : srcList)
        {
            if(!list.contains(vars))
                list.append(vars);
        }
    }

    return list;
}

MediaInfo* MediaManager::findMediaFromUri(const QString &uri, const QString &sourceId)
{
    if(!sourceId.isEmpty())
    {
        // If sourceId is available, check in this source;
        MediaSource *source = findSourceFromId(sourceId);
        if(source)
        {
            MediaInfoList list = source->availableMedias();
            for(MediaInfo *info : list)
            {
                if(info->mediaUri() == uri)
                    return info;
            }

            return nullptr;
        }
    }

    // Search the corresponding MediaInfo* in lastMediaList() first, then in all available medias
    for(MediaInfo *info : _lastList)
    {
        if(info->mediaUri() == uri)
            return info;
    }

    MediaInfoList list = availableMedias();
    for(MediaInfo *info : list)
    {
        if(info->mediaUri() == uri)
            return info;
    }

    return nullptr;
}

MediaInfo* MediaManager::findMediaFromId(const QString &id, const QString &sourceId)
{
    MediaSource *source = findSourceFromId(sourceId);
    if(source)
    {
        MediaInfoList list = source->availableMedias();
        for(MediaInfo *info : list)
        {
            if(info->identifier() == id)
                return info;
        }
    }

    return nullptr;
}

MediaSource* MediaManager::findSourceFromId(const QString &identifier)
{
    for(MediaSource *source : _sources)
    {
        if(source->identifier() == identifier)
            return source;
    }

    return nullptr;
}

//
// Playlist stuff
//

void MediaManager::createPlaylist(const QString &name)
{
    const QString fileName = Playlist::playlistDirectory() + QLatin1String("/playlist-") + DirUtility::uniqueFileName() + QLatin1String(".xml");
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

        Playlist *playlist = new Playlist(fileName, name, this);
        _playlists.append(playlist);
        emit availablePlaylistsChanged();

        return;
    }

    qWarning() << "Cannot create playlist file: " << file.errorString();
}

void MediaManager::removePlaylist(const QString &playlistFileName)
{
    if(QFile::exists(playlistFileName) && QFile::remove(playlistFileName))
    {
        Playlist *p = findPlaylistFromFileName(playlistFileName);
        _playlists.removeOne(p);
        delete p;

        emit availablePlaylistsChanged();
    }
}

void MediaManager::addMediaToPlaylist(const QString &playlistFileName, const QString &mediaUri)
{
    Playlist* playlist = findPlaylistFromFileName(playlistFileName);

    if(!playlist)
        return;

    MediaInfo *info = findMediaFromUri(mediaUri);
    if(info)
        playlist->addMedia(info);
}

void MediaManager::removeMediaFromPlaylist(const QString &playlistFileName, const QString &mediaUri)
{
    Playlist* playlist = findPlaylistFromFileName(playlistFileName);

    if(!playlist)
        return;

    MediaInfo *info = findMediaFromUri(mediaUri);
    if(info)
        playlist->removeMedia(info);
}

Playlist* MediaManager::findPlaylistFromFileName(const QString &playlistFileName)
{
    for(Playlist *p : _playlists)
    {
        if(p->fileName() == playlistFileName)
            return p;
    }

    return nullptr;
}

QList<Playlist *> MediaManager::availablePlaylists()
{
    return _playlists;
}

