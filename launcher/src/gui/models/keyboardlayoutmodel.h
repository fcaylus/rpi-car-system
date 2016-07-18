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

#ifndef KEYBOARDLAYOUTMODEL_H
#define KEYBOARDLAYOUTMODEL_H

#include <QAbstractListModel>
#include <QList>

class KeyboardRowModel;
class KeyboardFirstRowModel;
class KeyboardSecondRowModel;
class KeyboardThirdRowModel;

// This is not a real model but it allows access to models for each keyboard row.
// Layouts are loaded at object creation depending on the current locale
class KeyboardLayoutModel : public QObject
{
        Q_OBJECT

        Q_PROPERTY(KeyboardFirstRowModel* firstRowModel READ firstRowModel CONSTANT)
        Q_PROPERTY(KeyboardSecondRowModel* secondRowModel READ secondRowModel CONSTANT)
        Q_PROPERTY(KeyboardThirdRowModel* thirdRowModel READ thirdRowModel CONSTANT)

    public:
        explicit KeyboardLayoutModel(QObject *parent = 0);

        KeyboardFirstRowModel* firstRowModel() const;
        KeyboardSecondRowModel* secondRowModel() const;
        KeyboardThirdRowModel* thirdRowModel() const;

    private:

        KeyboardFirstRowModel *_firstRow;
        KeyboardSecondRowModel *_secondRow;
        KeyboardThirdRowModel *_thirdRow;
};

// Base class for each row model
class KeyboardRowModel: public QAbstractListModel
{
        Q_OBJECT

    public:

        // keyboardRow and locale must be specified must be specified (between 1 and 3)
        KeyboardRowModel(int keyboardRow, const QString& locale, QObject *parent = nullptr);

        int rowCount(const QModelIndex &parent) const override;
        QVariant data(const QModelIndex &index, int role) const override;
        QHash<int, QByteArray> roleNames() const override;

    private:

        struct Key
        {
            QChar letter;
            QChar symbol;

            constexpr Key(QChar let, QChar symb): letter(let), symbol(symb) {}
        };

        QList<Key> _data;
};

class KeyboardFirstRowModel: public KeyboardRowModel
{
        Q_OBJECT
    public:
        KeyboardFirstRowModel(const QString& locale, QObject *parent = nullptr): KeyboardRowModel(1, locale, parent) {}
};

class KeyboardSecondRowModel: public KeyboardRowModel
{
        Q_OBJECT
    public:
        KeyboardSecondRowModel(const QString& locale, QObject *parent = nullptr): KeyboardRowModel(2, locale, parent) {}
};

class KeyboardThirdRowModel: public KeyboardRowModel
{
        Q_OBJECT
    public:
        KeyboardThirdRowModel(const QString& locale, QObject *parent = nullptr): KeyboardRowModel(3, locale, parent) {}
};

#endif // KEYBOARDLAYOUTMODEL_H
