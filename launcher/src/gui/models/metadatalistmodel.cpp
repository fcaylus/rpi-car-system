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

#include "metadatalistmodel.h"
#include "../../core/mediamanager/mediamanager.h"

#include <algorithm>

MetadataListModel::MetadataListModel(QObject *parent) : QAbstractListModel(parent)
{
    connect(MediaManager::instance(), &MediaManager::availableMediasChanged, this, &MetadataListModel::update);
}

MediaInfo::MetadataType MetadataListModel::requiredMeta() const
{
    return _meta;
}

QVariant MetadataListModel::requiredMetaValue() const
{
    return _metaValue;
}

MetadataTypeList MetadataListModel::displayedMeta() const
{
    return _metaList;
}

void MetadataListModel::setRequiredMeta(MediaInfo::MetadataType meta)
{
    _meta = meta;
    emit requiredMetaChanged();
}

void MetadataListModel::setRequiredMetaValue(QVariant value)
{
    _metaValue = value;
    emit requiredMetaValueChanged();
}

void MetadataListModel::setDisplayedMeta(MetadataTypeList metaList)
{
    _metaList = metaList;
    emit displayedMetaChanged();
}

void MetadataListModel::addDisplayedMeta(MediaInfo::MetadataType meta)
{
    _metaList.append(meta);
    emit displayedMetaChanged();
}

void MetadataListModel::resetDisplayedMeta()
{
    _metaList.clear();
    emit displayedMetaChanged();
}

int MetadataListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _list.count();
}

QVariant MetadataListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= _list.count())
        return QVariant();

    MediaInfo::MetadataType type = static_cast<MediaInfo::MetadataType>(role - Qt::UserRole);
    if(!_metaList.contains(type))
        return QVariant();

    const QVariantList metas = _list.at(index.row());
    const int idx = _metaList.indexOf(type);

    if(idx >= metas.size())
        return QVariant();

    return metas.at(idx);
}

QHash<int, QByteArray> MetadataListModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    // At the initialisation, the displayedMeta are unknown
    // So, add every meta to the roles.
    // When retrieving data, if the role is not one of the displayed meta, it will return an empty QVariant.
    roles[Qt::UserRole + MediaInfo::MEDIA_URI] = "mediaUri";
    roles[Qt::UserRole + MediaInfo::ARTIST] = "artist";
    roles[Qt::UserRole + MediaInfo::ALBUM] = "album";
    roles[Qt::UserRole + MediaInfo::TITLE] = "title";
    roles[Qt::UserRole + MediaInfo::TRACK_NUMBER] = "trackNb";
    roles[Qt::UserRole + MediaInfo::COVER_URI] = "cover";

    return roles;
}

void MetadataListModel::update()
{
    if(!_list.isEmpty())
    {
        // Reset the model
        beginResetModel();
        _list.clear();
        endResetModel();
    }

    QList<QVariantList> newList = MediaManager::instance()->listMetadata(_metaList, _meta, _metaValue);
    if(newList.isEmpty())
        return;

    beginInsertRows(QModelIndex(), 0, newList.size() - 1);
    _list = newList;

    // Only use the first metadata to sort (only support string type)
    std::sort(_list.begin(), _list.end(), [](const QVariantList& v1, const QVariantList& v2){
        if(v1.size() > 0 && v1.at(0).type() == QVariant::String)
            return QString::localeAwareCompare(v1.at(0).toString(), v2.at(0).toString()) < 0;
        return true;
    });

    endInsertRows();
}



