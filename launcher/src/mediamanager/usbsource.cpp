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

#include "usbsource.h"
#include "mediamanager.h"
#include "../common.h"
#include "../dirutility.h"

#include <libudev.h>
#include <mntent.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include <QDebug>
#include <QDirIterator>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QByteArray>
#include <QStringList>

#include <MediaInfo/MediaInfo.h>

UsbSource::UsbSource(const QString &devPath, const QString &mountPoint,
                     const QString &idVendor, const QString &idProduct,
                     const QString &serial, const QString &sysPath,
                     QObject *parent): MediaSource(parent)
{
    _devPath = devPath;
    _mountPoint = mountPoint;
    _idVendor = idVendor;
    _idProduct = idProduct;
    _serial = serial;
    _sysPath = sysPath;

    if(!MediaManager::instance()->isCachePersistent())
        deleteCache();

    scan();
}

UsbSource::~UsbSource()
{
    while(!_mediaList.isEmpty())
        delete _mediaList.takeFirst();
}

MediaSource::Type UsbSource::type()
{
    return MediaSource::LocalFileSystem;
}

bool UsbSource::cacheSupported()
{
    return true;
}

QString UsbSource::identifier()
{
    return QLatin1String("usb-") + _idVendor + QLatin1Char('-') + _idProduct + QLatin1Char('-') + _serial;
}

void UsbSource::deleteCache()
{
    QDir cacheDir(configDir());
    if(cacheDir.exists())
        cacheDir.removeRecursively();
}

QVariant UsbSource::accessMediaMetadata(const QString &mediaUri, MediaInfo::MetadataType type)
{
    if(type == MediaInfo::MEDIA_URI)
        return QVariant(mediaUri);

    // MEDIA_URI cannot be an identifier since it contains the mount point that can change after reboot
    // So return the source identifier followed by the the mediaUri without the mount point
    // This identifier is used in playlist for example.
    if(type == MediaInfo::IDENTIFIER)
        return identifier() + QLatin1Char('-') + QString(mediaUri).remove(mountPoint() + "/");

    // Handle COVER_URI request
    if(type == MediaInfo::COVER_URI)
    {
        const QString cover = checkForCoverCache(mediaUri);
        if(!cover.isEmpty())
            return cover;
    }

    // Cache is enabled only if type != ALL and type != COVER_URI because COVER_URI has it's own cache
    if(MediaManager::instance()->cacheEnabled() && type != MediaInfo::ALL && type != MediaInfo::COVER_URI)
    {
        // Use the cache to get the data
        const QString mediaPath = QString(mediaUri).remove(mountPoint() + "/");
        QFile cacheFile(configDir() + QLatin1String("/cache/") + mediaPath + QLatin1String(".meta"));
        if(cacheFile.exists())
        {
            if(cacheFile.open(QFile::ReadOnly | QFile::Text))
            {
                QTextStream in(&cacheFile);
                QString line = in.readLine();

                if(type == MediaInfo::ARTIST)
                    return line.isEmpty() ? MediaInfo::defaultArtistName() : line;

                line = in.readLine();
                if(type == MediaInfo::ALBUM)
                    return line.isEmpty() ? MediaInfo::defaultAlbumName() : line;

                line = in.readLine();
                if(type == MediaInfo::TITLE)
                    return line.isEmpty() ? QFileInfo(mediaPath).baseName() : line;

                line = in.readLine();
                if(type == MediaInfo::TRACK_NUMBER)
                    return QVariant(line).toInt();

                cacheFile.close();
            }
        }
    }

    //
    // Here, get the data directly from the media files

    QString metaName = "General;";
    // Used for MediaInfo::ALL
    QString coverPathAll;
    switch(type)
    {
        // Return all metadata in the following order:
        /*
         * MEDIA_URI,
         * IDENTIFIER,
         * ARTIST,
         * ALBUM,
         * TITLE,
         * TRACK_NUMBER,
         * COVER_URI
         */
        case MediaInfo::ALL:
        {
            coverPathAll = checkForCoverCache(mediaUri);
            metaName += "%Performer%|%Album%|%Title%|%Track/Position%";

            if(coverPathAll.isEmpty())
                metaName += "|%Cover_Data%";

            break;
        }
        case MediaInfo::ALBUM:
            metaName += "%Album%";
            break;
        case MediaInfo::ARTIST:
            metaName += "%Performer%";
            break;
        case MediaInfo::TITLE:
            metaName += "%Title%";
            break;
        case MediaInfo::TRACK_NUMBER:
            metaName += "%Track/Position%";
            break;
        case MediaInfo::COVER_URI:
            metaName += "%Cover_Data%";
            break;
        default:
            return QVariant();
            break;
    }

    MediaInfoLib::MediaInfo mi;
    mi.Open(mediaUri.toStdWString());
    mi.Option(__T("Inform"), metaName.toStdWString());

    QString result = QString::fromStdWString(mi.Inform());
    mi.Close();

    if(type == MediaInfo::COVER_URI)
        return saveCoverData(mediaUri, result);

    if(type == MediaInfo::ALL)
    {
        QStringList list = result.split("|", QString::KeepEmptyParts);
        if(list.size() != 4 && list.size() != 5)
            return QVariantList() << mediaUri << accessMediaMetadata(mediaUri, MediaInfo::IDENTIFIER) << "" << "" << "" << "" << "";

        QVariantList returnList;
        returnList.reserve(7);
        returnList.append(mediaUri);
        returnList.append(accessMediaMetadata(mediaUri, MediaInfo::IDENTIFIER));

        // Artist
        returnList.append((list.at(0).isEmpty()) ? MediaInfo::defaultArtistName() : list.at(0));
        // Album
        returnList.append((list.at(1).isEmpty()) ? MediaInfo::defaultAlbumName() : list.at(1));
        // Title
        returnList.append((list.at(2).isEmpty()) ? QFileInfo(mediaUri).baseName() : list.at(2));
        // Track number
        returnList.append((list.at(3).isEmpty()) ? QVariant(0) : list.at(3));
        // Cover Uri
        if(coverPathAll.isEmpty())
        {
            // Create the cover
            returnList.append(saveCoverData(mediaUri, list.at(4)));
        }
        else
        {
            returnList.append(coverPathAll);
        }

        // If the cache is enabled and the media is not already cached, create it !
        if(MediaManager::instance()->cacheEnabled())
        {
            const QString mediaPath = QString(mediaUri).remove(mountPoint() + "/");
            QFile cacheFile(configDir() + QLatin1String("/cache/") + mediaPath + QLatin1String(".meta"));
            if(!cacheFile.exists())
            {
                QDir fileDir = QFileInfo(cacheFile).dir();
                if(!fileDir.exists())
                    fileDir.mkpath(".");
                if(cacheFile.open(QFile::WriteOnly | QFile::Text))
                {
                    QTextStream out(&cacheFile);
                    // Artist
                    out << list.at(0) << "\n";
                    // Album
                    out << list.at(1) << "\n";
                    // Title
                    out << list.at(2) << "\n";
                    // Track number
                    out << returnList.at(5).toInt() << "\n";

                    out.flush();
                    cacheFile.close();
                }
            }
        }

        return returnList;
    }

    // Handle empty values
    if(result.isEmpty())
    {
        if(type == MediaInfo::ALBUM)
            return MediaInfo::defaultAlbumName();
        else if(type == MediaInfo::ARTIST)
            return MediaInfo::defaultArtistName();
        else if(type == MediaInfo::TITLE)
            return QFileInfo(mediaUri).baseName();
        else if(type == MediaInfo::TRACK_NUMBER)
            return QVariant(0);
    }

    return QVariant(result);
}

// Private
QString UsbSource::checkForCoverCache(const QString &mediaUri)
{
    // Check if the media is in the /files/ dir
    const QString path = QString(mediaUri).remove(mountPoint() + "/");
    QFile file(configDir() + QLatin1String("/files/") + path + QLatin1String(".cover"));
    if(file.exists())
    {
        if(file.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream in(&file);
            const QString coverName = in.readLine();
            return coverName.isEmpty() ? MediaInfo::defaultCoverPath()
                                       : QLatin1String("file://") + configDir() + QLatin1String("/covers/") + coverName;
        }
    }

    return QString();
}

// Private
QString UsbSource::saveCoverData(const QString &mediaUri, const QString &base64Data)
{
    // Create a file containing the cover and append it the the covers list
    QByteArray coverData = QByteArray::fromBase64(base64Data.toUtf8());
    const QString mediaPath = QString(mediaUri).remove(mountPoint() + "/");

    if(coverData.isEmpty())
    {
        // Set to default album
        QFile file(configDir() + QLatin1String("/files/") + mediaPath + QLatin1String(".cover"));
        file.open(QFile::WriteOnly | QFile::Text);
        file.close();

        return MediaInfo::defaultCoverPath();
    }

    //
    // Check if the cover already exist
    QString path;
    bool alreadyExist = false;
    QDirIterator it(configDir() + QLatin1String("/covers/"),
                    QStringList(),
                    QDir::Files | QDir::Readable | QDir::NoDotAndDotDot);

    while(it.hasNext())
    {
        path = it.next();
        if(DirUtility::checkContent(path, coverData))
        {
            alreadyExist = true;
            break;
        }
    }

    if(!alreadyExist)
    {
        path = configDir() + QLatin1String("/covers/") + DirUtility::uniqueFileName();
        QFile coverFile(path);
        if(coverFile.open(QFile::WriteOnly))
        {
            coverFile.write(coverData);
            coverFile.close();
        }
    }

    QFile file(configDir() + QLatin1String("/files/") + mediaPath + QLatin1String(".cover"));
    QDir fileDir = QFileInfo(file).dir();
    if(!fileDir.exists())
        fileDir.mkpath(".");
    if(file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&file);
        out << QFileInfo(path).baseName();
        out.flush();
        file.close();

        return QLatin1String("file://") + path;
    }

    return MediaInfo::defaultCoverPath();
}

MediaInfoList UsbSource::availableMedias(MediaInfo::MetadataType onlyWithMeta, QVariant onlyWithMetaValue)
{
    if(onlyWithMeta == MediaInfo::UNKNOWN || onlyWithMetaValue == QVariant())
        return _mediaList;

    // Only select media with the corresponding metadata
    MediaInfoList newList;
    for(MediaInfo *info: _mediaList)
    {
        if(accessMediaMetadata(info->mediaUri(), onlyWithMeta) == onlyWithMetaValue)
            newList.append(info);
    }

    return newList;
}

QList<QVariantList> UsbSource::listMetadata(MetadataTypeList metadataToRetrieve,
                                            MediaInfo::MetadataType requiredMeta,
                                            QVariant requiredMetaValue)
{
    QList<QVariantList> list;
    for(MediaInfo *info : _mediaList)
    {
        if(requiredMeta == MediaInfo::UNKNOWN
           || requiredMetaValue == QVariant()
           || accessMediaMetadata(info->mediaUri(), requiredMeta) == requiredMetaValue)
        {
            QVariantList vars;
            for(MediaInfo::MetadataType meta : metadataToRetrieve)
                vars.append(accessMediaMetadata(info->mediaUri(), meta));

            if(!list.contains(vars))
                list.append(vars);
        }
    }

    return list;
}

QString UsbSource::devPath() const
{
    return _devPath;
}

QString UsbSource::mountPoint() const
{
    return _mountPoint;
}

QString UsbSource::idVendor() const
{
    return _idVendor;
}

QString UsbSource::idProduct() const
{
    return _idProduct;
}

QString UsbSource::serial() const
{
    return _serial;
}

QString UsbSource::sysPath() const
{
    return _sysPath;
}

QString UsbSource::configDir() const
{
    return Common::configDir() + QString("/usbsources/%1-%2-%3").arg(idVendor(), idProduct(), serial());
}

void UsbSource::requestDisconnection()
{
    emit disconnected();
}

// Static
MediaSourceList UsbSource::listAllUsbSources(QObject *sourceParent)
{
    MediaSourceList sourcesList;

    //
    // Get a list of mountpoints
    QList<QPair<QString, QString> > mountPoints = UdevUtil::availableMountPoints();

    if(mountPoints.isEmpty())
        return sourcesList;

    //
    // Get a list of USB mass storage devices
    QList<UdevUtil::UsbDeviceInfo> usbList = UdevUtil::enumerateUsbMassStorage();

    //
    // Now, create sources object if a usb devices is mounted
    for(UdevUtil::UsbDeviceInfo device: usbList)
    {
        for(QPair<QString, QString> mountPt : mountPoints)
        {
            if(mountPt.first == device.devPath)
            {
                MediaSource *source = new UsbSource(device.devPath, mountPt.second, device.idVendor,
                                                    device.idProduct, device.serial, device.sysPath, sourceParent);
                sourcesList.append(source);
                break;
            }
        }
    }

    return sourcesList;
}

// Private
void UsbSource::scan()
{
    UsbSourceUtil::SearchMediaWorker *worker = new UsbSourceUtil::SearchMediaWorker(this);
    worker->moveToThread(&_searchThread);
    connect(&_searchThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(&_searchThread, &QThread::started, worker, &UsbSourceUtil::SearchMediaWorker::search);
    connect(worker, &UsbSourceUtil::SearchMediaWorker::searchFinished, this, [this](MediaInfoList list) {
        while(!_mediaList.empty())
            delete _mediaList.takeFirst();

        _mediaList = list;

        _searchThread.quit();
        _searchThread.wait();

        emit newMediaAvailable();
    });

    _searchThread.start();
}

//
// SearchMediaWorker
//

UsbSourceUtil::SearchMediaWorker::SearchMediaWorker(UsbSource *source)
{
    _source = source;
}

void UsbSourceUtil::SearchMediaWorker::search()
{
    MediaInfoList mediaList;
    // Search all music files in the usb stick
    QStringList filesFormats = MediaInfo::musicFilesFormats();

    QDir configDir = QDir(_source->configDir());
    // Create the config dir if not exist
    if(!configDir.exists())
        configDir.mkpath(".");

    configDir.mkpath("./covers");
    configDir.mkpath("./files");

    QDirIterator it(_source->mountPoint(), filesFormats, QDir::Files | QDir::Readable | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        QString path = it.next();
        MediaInfo *info = new MediaInfo(_source, path);
        info->moveToThread(_source->thread());
        mediaList.append(info);
    }


    if(mediaList.isEmpty())
    {
        // Remove previously config if exists
        if(configDir.exists())
            configDir.removeRecursively();
    }

    emit searchFinished(mediaList);
}

//
// HotplugWorker
//

UsbSourceUtil::HotplugWorker::HotplugWorker(QObject *usbSourceParent)
{
    _usbSourceParent = usbSourceParent;
}

void UsbSourceUtil::HotplugWorker::start()
{
    struct udev* udev = udev_new();
    struct udev_monitor* monitor = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(monitor, "usb", "usb_device");
    udev_monitor_enable_receiving(monitor);

    // Get a file descriptor for the udev instance
    int fd = udev_monitor_get_fd(monitor);

    while(!thread()->isInterruptionRequested())
    {
        // Set up the call to select()
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        const int ret = select(fd+1, &fds, NULL, NULL, &tv);

        // Check if our file descriptor has received data.
        if(ret > 0 && FD_ISSET(fd, &fds))
        {
            struct udev_device *dev = udev_monitor_receive_device(monitor);
            if(dev)
            {
                const QLatin1String action(udev_device_get_action(dev));

                if(action == QLatin1String("remove"))
                {
                    const QString sysPath = udev_device_get_syspath(dev);
                    qDebug() << "Device disconnected:" << sysPath;
                    emit usbSourceDisconnected(sysPath);
                }
                else if(action == QLatin1String("add"))
                {
                    qDebug() << "Prepare addition:" << udev_device_get_syspath(dev);

                    // Wait 2 seconds before retrieving udev information
                    // This allow the system to mount the device
                    usleep(2*1000*1000);

                    struct udev_device* scsi = UdevUtil::getChild(udev, dev, "scsi", "scsi_device");
                    struct udev_device* block = UdevUtil::getChild(udev, scsi, "block");
                    struct udev_device* scsi_disk = UdevUtil::getChild(udev, scsi, "scsi_disk");

                    if(scsi && block && scsi_disk)
                    {
                        struct udev_device* part = UdevUtil::getFirstPartition(udev, block);
                        if(part)
                        {
                            UdevUtil::UsbDeviceInfo info;
                            info.devPath = udev_device_get_devnode(part);
                            info.idVendor = udev_device_get_sysattr_value(dev, "idVendor");
                            info.idProduct = udev_device_get_sysattr_value(dev, "idProduct");
                            info.serial = udev_device_get_sysattr_value(dev, "serial");
                            info.sysPath = udev_device_get_syspath(dev);
                            udev_device_unref(part);

                            // Find the mountpoint
                            QList<QPair<QString, QString> > mountPoints = UdevUtil::availableMountPoints();
                            for(QPair<QString, QString> mountPt : mountPoints)
                            {
                                if(mountPt.first == info.devPath)
                                {
                                    qDebug() << "Device connected:" << info.devPath << "|" << mountPt.second;
                                    UsbSource *source = new UsbSource(info.devPath,
                                                                      mountPt.second,
                                                                      info.idVendor,
                                                                      info.idProduct,
                                                                      info.serial,
                                                                      info.sysPath);

                                    source->moveToThread(QCoreApplication::instance()->thread());
                                    source->setParent(_usbSourceParent);

                                    emit newUsbSourceAvailable(source);
                                    break;
                                }
                            }
                        }
                    }

                    if(block)
                        udev_device_unref(block);

                    if(scsi_disk)
                        udev_device_unref(scsi_disk);

                    if(scsi)
                        udev_device_unref(scsi);
                }

                udev_device_unref(dev);
            }
        }

        usleep(250*1000);
    }

    udev_unref(udev);
}

//
// UdevUtil
//

struct udev_device* UdevUtil::getChild(struct udev* udev, struct udev_device* parent, const char* subsystem, const char* devtype)
{
    struct udev_device* child = nullptr;
    struct udev_enumerate *enumerate = udev_enumerate_new(udev);

    udev_enumerate_add_match_parent(enumerate, parent);
    udev_enumerate_add_match_subsystem(enumerate, subsystem);
    if(devtype)
        udev_enumerate_add_match_property(enumerate, "DEVTYPE", devtype);

    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    udev_list_entry_foreach(entry, devices)
    {
        const char *path = udev_list_entry_get_name(entry);
        child = udev_device_new_from_syspath(udev, path);
        break;
    }

    udev_enumerate_unref(enumerate);
    return child;
}

struct udev_device* UdevUtil::getFirstPartition(struct udev* udev, struct udev_device* parent)
{
    struct udev_device* part = nullptr;
    struct udev_enumerate *enumerate = udev_enumerate_new(udev);

    udev_enumerate_add_match_parent(enumerate, parent);
    //udev_enumerate_add_match_subsystem(enumerate, "block");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    udev_list_entry_foreach(entry, devices)
    {
        const char *path = udev_list_entry_get_name(entry);
        struct udev_device* child = udev_device_new_from_syspath(udev, path);

        // Check if it's a partition
        if(udev_device_get_sysattr_value(child, "partition") != nullptr)
            part = child;
        else
            udev_device_unref(child);
    }

    udev_enumerate_unref(enumerate);
    return part;
}

QList<UdevUtil::UsbDeviceInfo> UdevUtil::enumerateUsbMassStorage()
{
    QList<UsbDeviceInfo> devicesList;

    struct udev* udev = udev_new();
    struct udev_enumerate* enumerate = udev_enumerate_new(udev);

    udev_enumerate_add_match_subsystem(enumerate, "scsi");
    udev_enumerate_add_match_property(enumerate, "DEVTYPE", "scsi_device");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    udev_list_entry_foreach(entry, devices)
    {
        const char* path = udev_list_entry_get_name(entry);
        struct udev_device* scsi = udev_device_new_from_syspath(udev, path);

        struct udev_device* block = getChild(udev, scsi, "block");
        struct udev_device* scsi_disk = getChild(udev, scsi, "scsi_disk");

        struct udev_device* usb = udev_device_get_parent_with_subsystem_devtype(scsi, "usb", "usb_device");

        if(block && scsi_disk && usb)
        {
            // Get the first partition path
            // So, it will only list mounted devices (they need to be automatically mounted by the system)
            struct udev_device* part = getFirstPartition(udev, block);
            if(part)
            {
                UsbDeviceInfo info;
                info.devPath = udev_device_get_devnode(part);
                info.idVendor = udev_device_get_sysattr_value(usb, "idVendor");
                info.idProduct = udev_device_get_sysattr_value(usb, "idProduct");
                info.serial = udev_device_get_sysattr_value(usb, "serial");
                info.sysPath = udev_device_get_syspath(usb);
                devicesList.append(info);

                udev_device_unref(part);
            }
        }

        if(block)
            udev_device_unref(block);

        if(scsi_disk)
            udev_device_unref(scsi_disk);

        if(usb)
            udev_device_unref(usb);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return devicesList;
}

QList<QPair<QString, QString> > UdevUtil::availableMountPoints()
{
    QList<QPair<QString, QString> > mountPoints;
    struct mntent *ent;
    FILE *file;
    file = setmntent("/proc/mounts", "r");
    if(file == nullptr)
    {
        qWarning() << "Cannot read /proc/mounts";
        return mountPoints;
    }

    while(nullptr != (ent = getmntent(file)))
    {
        // Only add /dev devices and exclude the root file system
        if(QLatin1String(ent->mnt_dir) != "/" && QString(ent->mnt_fsname).startsWith("/dev/"))
            mountPoints.append(QPair<QString, QString>(QString(ent->mnt_fsname), QString(ent->mnt_dir)));
    }
    endmntent(file);

    return mountPoints;
}
