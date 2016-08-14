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

#include "licenseslistmodel.h"
#include "../../core/dirutility.h"

#include <QFile>
#include <QCoreApplication>
#include <QTextStream>
#include <QStringList>
#include <QString>

LicensesListModel::LicensesListModel(QObject *parent) : QAbstractListModel(parent)
{
    // Add some licenses of packages bundled directly in the app
    _list.append(License({"pugixml", "1.7", "MIT", QStringList() << "LICENSE.txt"}));
    _list.append(License({"MiniFlatFlags", "", "CC0 1.0 Universal", QStringList() << "LICENSE.txt"}));
    _list.append(License({"OpenSansFont", "", "Apache 2.0", QStringList() << "LICENSE-OpenSans.txt"}));
    _list.append(License({"MaterialIcons", "", "CC-BY 4.0", QStringList() << "LICENSE-MaterialIcons.txt"}));

    QFile manifestFile(QCoreApplication::applicationDirPath() + "/licenses/manifest.csv");
    if(manifestFile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&manifestFile);
        // Skip the first line because it's the header of the csv file
        in.readLine();
        while(!in.atEnd())
        {
            QString line = in.readLine();
            // Remove first "
            line.remove(0, 1);
            QStringList elements = line.split("\",\"", QString::KeepEmptyParts);

            if(elements.size() >= 4)
            {
                License lic;
                lic.package = elements.at(0);
                lic.version = elements.at(1);
                lic.name = elements.at(2);
                lic.files = elements.at(3).split(" ", QString::SkipEmptyParts);

                _list.append(lic);
            }
        }
    }
}

int LicensesListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _list.count();
}

QVariant LicensesListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= _list.count())
        return QVariant();

    if(role == Qt::UserRole + 1)
        return _list.at(index.row()).package;
    else if(role == Qt::UserRole + 2)
        return _list.at(index.row()).version;
    else if(role == Qt::UserRole + 3)
        return _list.at(index.row()).name;
    else if(role == Qt::UserRole + 4)
    {
        License lic = _list.at(index.row());
        const QString filePath = QCoreApplication::applicationDirPath() + QLatin1String("/licenses/files/")
                + lic.package + (lic.version.isEmpty() ? QString() : (QLatin1Char('-') + lic.version)) + QLatin1Char('/');

        QString text;
        for(QString file : lic.files)
        {
            text += QString(file.size() + 4, '*');
            text += "\n* ";
            text += file;
            text += "\n";
            text += QString(file.size() + 4, '*');
            text += "\n\n";
            text += DirUtility::readFile(filePath + file);
            text += "\n\n";
        }

        return text;
    }

    return QVariant();
}

QHash<int, QByteArray> LicensesListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole + 1] = "packageName";
    roles[Qt::UserRole + 2] = "version";
    roles[Qt::UserRole + 3] = "licenseName";
    roles[Qt::UserRole + 4] = "licenseText";
    return roles;
}
