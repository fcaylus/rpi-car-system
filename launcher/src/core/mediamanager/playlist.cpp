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

#include "playlist.h"

#include "mediamanager.h"
#include "../common.h"

#include <QFile>
#include <QDirIterator>
#include <QDir>
#include "pugixml.hpp"


Playlist::Playlist(const QString &fileName, const QString &name, QObject *parent): MediaSource(parent)
{
    _name = name;
    _fileName = fileName;
}

QString Playlist::name() const
{
    return _name;
}

QString Playlist::fileName() const
{
    return _fileName;
}

QString Playlist::identifier()
{
    return _fileName;
}

MediaSource::Type Playlist::type()
{
    return LocalFileSystem;
}

// Unused
bool Playlist::cacheSupported()
{
    return false;
}

QVariant Playlist::accessMediaMetadata(const QString&, MediaInfo::MetadataType)
{
    return QVariant();
}

QList<QVariantList> Playlist::listMetadata(MetadataTypeList, MediaInfo::MetadataType, QVariant)
{
    return QList<QVariantList>();
}

MediaInfoList Playlist::availableMedias(MediaInfo::MetadataType onlyWithMeta, QVariant onlyWithMetaValue)
{
    // Selectors are not supported, return all available media
    Q_UNUSED(onlyWithMeta)
    Q_UNUSED(onlyWithMetaValue)

    // Read the playlist file
    pugi::xml_document xmlDoc;
    if(!xmlDoc.load_file(_fileName.toStdString().c_str()))
        return MediaInfoList();

    MediaInfoList list;

    pugi::xpath_node_set nodes = xmlDoc.select_nodes("/playlist/media");
    for(pugi::xpath_node_set::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        pugi::xml_node node = (*it).node();
        const QString id = node.attribute("id").value();
        const QString sourceId = node.attribute("source").value();

        if(!id.isEmpty() && !sourceId.isEmpty())
        {
            MediaInfo *info = MediaManager::instance()->findMediaFromId(id, sourceId);
            if(info)
                list.append(info);
        }
    }

    return list;
}

//
// Slots
//

void Playlist::setName(const QString &newName)
{
    _name = newName;
}

void Playlist::addMedia(MediaInfo *info)
{
    if(!QFile::exists(_fileName))
        return;

    pugi::xml_document xmlDoc;
    if(!xmlDoc.load_file(_fileName.toStdString().c_str()))
        return;

    pugi::xml_node newMedia = xmlDoc.child("playlist").append_child("media");
    newMedia.append_attribute("source").set_value(info->sourceId().toStdString().c_str());
    newMedia.append_attribute("id").set_value(info->identifier().toStdString().c_str());

    if(!xmlDoc.save_file(_fileName.toStdString().c_str(), "\t", PUGI_SAVE_FORMAT, pugi::xml_encoding::encoding_utf8))
        return;
}

void Playlist::removeMedia(MediaInfo *info)
{
    pugi::xml_document xmlDoc;
    if(!xmlDoc.load_file(_fileName.toStdString().c_str()))
        return;

    pugi::xml_node toRemove;
    bool found = false;

    pugi::xpath_node_set nodes = xmlDoc.select_nodes("/playlist/media");
    for(pugi::xpath_node_set::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        pugi::xml_node node = (*it).node();
        const QString id = node.attribute("id").value();
        if(id == info->identifier())
        {
            found = true;
            toRemove = node;
            break;
        }
    }

    if(found)
    {
        xmlDoc.child("playlist").remove_child(toRemove);
        xmlDoc.save_file(_fileName.toStdString().c_str(), "\t", PUGI_SAVE_FORMAT, pugi::xml_encoding::encoding_utf8);
    }
}

// Static
QString Playlist::playlistDirectory()
{
    const QString path = Common::configDir() + QLatin1String("/playlists");
    QDir dir(path);
    if(!dir.exists())
        dir.mkpath(".");

    return path;
}

// Static
QList<Playlist*> Playlist::findAllPlaylists(QObject *parent)
{
    QList<Playlist *> list;

    QDirIterator it(Playlist::playlistDirectory(),
                    QStringList({"playlist-*.xml"}),
                    QDir::Files | QDir::Readable | QDir::Writable | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    while(it.hasNext())
    {
        const QString path = it.next();
        pugi::xml_document xmlDoc;
        if(!xmlDoc.load_file(path.toStdString().c_str()))
            continue;

        QString name = xmlDoc.child("playlist").attribute("name").value();

        if(!name.isEmpty())
            list.append(new Playlist(path, name, parent));
    }

    return list;
}
