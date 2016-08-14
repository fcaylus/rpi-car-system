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

#ifndef LICENSESLISTMODEL_H
#define LICENSESLISTMODEL_H

#include <QAbstractListModel>
#include <QList>

// List all thirdparties licenses used by the system
class LicensesListModel : public QAbstractListModel
{
        Q_OBJECT

    public:

        LicensesListModel(QObject *parent = nullptr);

        int rowCount(const QModelIndex &parent) const override;
        QVariant data(const QModelIndex &index, int role) const override;

        // Available roleNames are:
        // - "packageName"
        // - "version"
        // - "licenseName"
        // - "licenseText" (if they are several licenses, everything is concatenated in one text)
        QHash<int, QByteArray> roleNames() const override;

    private:

        struct License
        {
            QString package;
            QString version;
            QString name;

            QStringList files;
        };

        QList<License> _list;
};

#endif // LICENSESLISTMODEL_H
