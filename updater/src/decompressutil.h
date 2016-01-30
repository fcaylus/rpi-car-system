/*
 * This file is part of RPI Car System.
 * Copyright (c) 2016 Fabien Caylus <toutjuste13@gmail.com>
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

#ifndef DECOMPRESSUTIL
#define DECOMPRESSUTIL

#include <archive.h>
#include <archive_entry.h>

#include <QString>
#include <QDebug>

#include "dirutility.h"
#include "updatercodes.h"
#include "filereader.h"

namespace DecompressUtilInternal
{
    static int copyData(struct archive *ar, struct archive *aw)
    {
        const void *buffer;
        size_t size;
        off_t offset;

        while(true)
        {
            int ret = archive_read_data_block(ar, &buffer, &size, &offset);

            if(ret == ARCHIVE_EOF)
                return ARCHIVE_OK;

            else if(ret < ARCHIVE_OK)
                return ret;

            ret = archive_write_data_block(aw, buffer, size, offset);

            if(ret < ARCHIVE_OK)
                return ret;
        }
    }
}

namespace DecompressUtil
{
    static bool checkFileExtension(const QString& path, QString *errorText)
    {
        if(!path.endsWith(QLatin1String(".tar.xz")))
        {
            if(errorText)
                *errorText = "The file is not an archive.";
            return false;
        }
        return true;
    }

    static int checkVersion(const QString &path, QString *errorText)
    {
        if(path.isEmpty() || !checkFileExtension(path, errorText))
            return UPDATER_CODE_COMMON_ERROR;

        struct archive_entry *entry;
        struct archive *ar;
        struct archive *out;

        bool found = false;

        ar = archive_read_new();
        archive_read_support_filter_xz(ar);
        archive_read_support_format_tar(ar);

        out = archive_write_disk_new();
        archive_write_disk_set_standard_lookup(out);

        int ret = archive_read_open_filename(ar, path.toStdString().c_str(), 10240);

        if(ret != ARCHIVE_OK)
        {
            *errorText =  "Cannot read archive !";
            return UPDATER_CODE_COMMON_ERROR;
        }

        while(archive_read_next_header(ar, &entry) == ARCHIVE_OK)
        {
            if(std::strcmp(archive_entry_pathname(entry), "opt/rpi-car-system/VERSION") == 0)
            {
                archive_entry_set_pathname(entry, "/tmp/rpi-car-system-VERSION");
                if(archive_write_header(out, entry) == ARCHIVE_OK)
                {
                    ret = DecompressUtilInternal::copyData(ar, out);
                    if(ret < ARCHIVE_OK)
                        return UPDATER_CODE_COMMON_ERROR;

                    ret = archive_write_finish_entry(out);
                    if(ret < ARCHIVE_OK)
                        return UPDATER_CODE_COMMON_ERROR;

                    found = true;
                    break;
                }
            }
            else
            {
                archive_read_data_skip(ar);
            }
        }

        archive_read_close(ar);
        archive_read_free(ar);
        archive_write_close(out);
        archive_write_free(out);

        // Check VERSION file
        if(found)
        {
            QString ver = FileReader::readFile("/tmp/rpi-car-system-VERSION").trimmed();
            QStringList nums = ver.split(".", QString::SkipEmptyParts);

            QFile::remove("/tmp/rpi-car-system-VERSION");

            // Check hardware
            if(nums.at(0).toInt() != HARDWARE_VERSION)
            {
                *errorText = QStringLiteral("Hardware versions not match. Current:")
                        + QString::number(HARDWARE_VERSION)
                        + QStringLiteral("Update:") + nums.at(0);
                return UPDATER_CODE_CHECKVER_BADHARDWARE;
            }

            // Check other numbers
            if(nums.at(1).toInt() >= MINOR_VERSION)
            {
                if(nums.at(2).toInt() > PATCH_VERSION)
                {
                    *errorText = QStringLiteral("Newer version found:") + ver;
                    return UPDATER_CODE_CHECKVER_OK;
                }
            }

            // Here, version is older
            *errorText = QStringLiteral("Older version. Current:")
                    + QString(APPLICATION_VERSION)
                    + QStringLiteral("Update:") + ver;
            return UPDATER_CODE_CHECKVER_OLDER;
        }

        *errorText = "VERSION file not found !";
        return UPDATER_CODE_CHECKVER_NOFILE;
    }

    static bool decompressedSize(const QString &path, qint64* sizeSum, QString *errorText)
    {
        if(path.isEmpty() || !checkFileExtension(path, errorText) || sizeSum == nullptr)
            return false;

        struct archive_entry *entry;
        struct archive *ar;

        *sizeSum = 0;

        ar = archive_read_new();
        archive_read_support_filter_xz(ar);
        archive_read_support_format_tar(ar);

        int ret = archive_read_open_filename(ar, path.toStdString().c_str(), 10240);

        if(ret != ARCHIVE_OK)
        {
            *errorText =  "Cannot read archive !";
            return false;
        }

        while(archive_read_next_header(ar, &entry) == ARCHIVE_OK)
        {
            *sizeSum += archive_entry_size(entry);
            archive_read_data_skip(ar);
        }

        archive_read_close(ar);
        archive_read_free(ar);
        return true;
    }


    static bool decompress(const QString& inputPath, const QString& outputPath, QString *errorText)
    {
        if(!checkFileExtension(inputPath, errorText))
            return false;

        struct archive *ar;
        struct archive *ext;
        struct archive_entry *entry;
        int ret;

        ar = archive_read_new();

        // Activate .tar.xz support
        archive_read_support_format_tar(ar);
        archive_read_support_filter_xz(ar);

        ext = archive_write_disk_new();
        archive_write_disk_set_options(ext, ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS);
        archive_write_disk_set_standard_lookup(ext);

        if((ret = archive_read_open_filename(ar, inputPath.toStdString().c_str(), 10240)))
        {
            *errorText = "Can't read archive file.";
            return false;
        }

        while(true)
        {
            //
            // Read Header
            ret = archive_read_next_header(ar, &entry);

            if(ret == ARCHIVE_EOF)
                break;

            // Check errors
            if(ret < ARCHIVE_OK)
            {
                *errorText = archive_error_string(ar);
                qWarning() << *errorText;
            }

            if(ret < ARCHIVE_WARN)
                return false;

            //
            // Write Header


            // Prepend dir prefix
            archive_entry_set_pathname(entry, QString(outputPath + QStringLiteral("/") + QString(archive_entry_pathname(entry))).toStdString().c_str());
            ret = archive_write_header(ext, entry);

            if(ret < ARCHIVE_OK)
            {
                *errorText = archive_error_string(ext);
                qWarning() << *errorText;
            }

            else if(archive_entry_size(entry) > 0)
            {
                ret = DecompressUtilInternal::copyData(ar, ext);

                // Check errors
                if(ret < ARCHIVE_OK)
                {
                    *errorText = archive_error_string(ext);
                    qWarning() << *errorText;
                }

                if(ret < ARCHIVE_WARN)
                    return false;
            }

            ret = archive_write_finish_entry(ext);

            // Check errors
            if(ret < ARCHIVE_OK)
            {
                *errorText = archive_error_string(ext);
                qWarning() << *errorText;
            }

            if(ret < ARCHIVE_WARN)
                return false;
        }

        archive_read_close(ar);
        archive_read_free(ar);
        archive_write_close(ext);
        archive_write_free(ext);

        return true;
    }
}

#endif // DECOMPRESSUTIL

