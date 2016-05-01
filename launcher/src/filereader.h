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

#ifndef FILEREADER_H
#define FILEREADER_H

#include <QObject>
#include <QFile>
#include <QTextStream>

class FileReader : public QObject
{
    Q_OBJECT

    public:

        FileReader(QObject *parent = nullptr): QObject(parent) {}

        static QString readFile(const QString &fileName)
        {
            QFile file(fileName);
            if(!file.open(QFile::ReadOnly | QFile::Text))
                return QString();
            QTextStream in(&file);
            return in.readAll();
        }

        static void writeFile(const QString& fileName, const QString& content)
        {
            QFile file(fileName);
            if(file.open(QFile::WriteOnly | QFile::Text))
            {
                QTextStream out(&file);
                out << content;
            }
        }

        // For QML
        Q_INVOKABLE QString read(const QString &filename)
        {
            return readFile(filename);
        }
};

#endif // FILEREADER_H
