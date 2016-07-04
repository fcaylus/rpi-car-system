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

#ifndef USBSOURCE_H
#define USBSOURCE_H

#include "mediasource.h"

#include <QThread>
#include <QList>
#include <QPair>

/*
 * Allow access to a usb mass storage.
 * This media source maintains a set of config files that follows this pattern:
 *
 *      usbsources/
 *          idVendor-idProduct-serial/           Only devices with at least one media are listed (updated at each scan)
 *              files/                           Represent the structure of the media files on the USB source.
 *                                               But each files contains the name of the cover file, not the data.
 *              covers/
 *                  coverfile1
 *                  coverfile2
 *                  ...
 *          ...
 *
 */
class UsbSource: public MediaSource
{
        Q_OBJECT

        Q_PROPERTY(QString devPath READ devPath)
        Q_PROPERTY(QString mountPoint READ mountPoint)
        Q_PROPERTY(QString idVendor READ idVendor)
        Q_PROPERTY(QString idProduct READ idProduct)
        Q_PROPERTY(QString serial READ serial)

    public:
        UsbSource(const QString& devPath, const QString& mountPoint,
                  const QString& idVendor, const QString& idProduct,
                  const QString& serial, const QString& sysPath,
                  QObject* parent = nullptr);

        ~UsbSource();

        //
        // Re-implemented
        //

        Type type();
        QString identifier();
        bool cacheSupported();
        void deleteCache();

        QVariant accessMediaMetadata(const QString& mediaUri, MediaInfo::MetadataType type);

        MediaInfoList availableMedias(MediaInfo::MetadataType onlyWithMeta = MediaInfo::UNKNOWN,
                                      QVariant onlyWithMetaValue = QVariant());

        QList<QVariantList> listMetadata(MetadataTypeList metadataToRetrieve,
                                         MediaInfo::MetadataType requiredMeta = MediaInfo::UNKNOWN,
                                         QVariant requiredMetaValue = QVariant());

        // Getters
        QString devPath() const;
        QString mountPoint() const;
        QString idVendor() const;
        QString idProduct() const;
        QString serial() const;
        QString sysPath() const;

        QString configDir() const;

        static MediaSourceList listAllUsbSources(QObject *sourceParent = nullptr);

    public slots:

        void requestDisconnection();

    private:
        QString _devPath;
        QString _mountPoint;
        QString _idVendor;
        QString _idProduct;
        QString _serial;
        QString _sysPath;

        QThread _searchThread;

        MediaInfoList _mediaList;

        // Return the uri of the cover file if found
        // Else, return an empty string
        QString checkForCoverCache(const QString& mediaUri);
        QString saveCoverData(const QString& mediaUri, const QString& base64Data);

        // Launch the search thread
        void scan();
};


// Worker class used to search for media on the device
namespace UsbSourceUtil {
    class SearchMediaWorker: public QObject
    {
            Q_OBJECT

        public:
            SearchMediaWorker(UsbSource *source);

        public slots:
            void search();

        signals:
            void searchFinished(MediaInfoList mediaList);

        private:
            UsbSource *_source;
    };

    class HotplugWorker: public QObject
    {
            Q_OBJECT

        public:
            HotplugWorker(QObject *usbSourceParent);

        public slots:
            void start();

        signals:
            void newUsbSourceAvailable(UsbSource *source);
            void usbSourceDisconnected(const QString& sysPath);

        private:
            QObject* _usbSourceParent;
    };
}

struct udev;
struct udev_device;

namespace UdevUtil {

    struct UsbDeviceInfo {
        QString devPath;
        QString idVendor;
        QString idProduct;
        QString serial;
        QString sysPath;
    };

    struct udev_device* getChild(struct udev* udev, struct udev_device* parent, const char* subsystem, const char *devtype = nullptr);
    struct udev_device* getFirstPartition(struct udev* udev, struct udev_device* parent);
    QList<UsbDeviceInfo> enumerateUsbMassStorage();

    // Return a list of (devPath, mountPoint)
    QList<QPair<QString, QString> > availableMountPoints();
}

#endif // USBSOURCE_H
