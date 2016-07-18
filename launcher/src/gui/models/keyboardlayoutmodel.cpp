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

#include "keyboardlayoutmodel.h"
#include <QSettings>

KeyboardLayoutModel::KeyboardLayoutModel(QObject *parent) : QObject(parent)
{
    QSettings settings;
    const QString locale = settings.value("locale").toString();

    _firstRow = new KeyboardFirstRowModel(locale, this);
    _secondRow = new KeyboardSecondRowModel(locale, this);
    _thirdRow = new KeyboardThirdRowModel(locale, this);
}

KeyboardFirstRowModel* KeyboardLayoutModel::firstRowModel() const
{
    return _firstRow;
}

KeyboardSecondRowModel* KeyboardLayoutModel::secondRowModel() const
{
    return _secondRow;
}

KeyboardThirdRowModel* KeyboardLayoutModel::thirdRowModel() const
{
    return _thirdRow;
}

// *******************************************
// *             KeyboardRowModel            *
// *******************************************

KeyboardRowModel::KeyboardRowModel(int keyboardRow, const QString& locale, QObject *parent): QAbstractListModel(parent)
{
    if(locale == "fr")
    {
        if(keyboardRow == 1)
        {
            _data.append({Key('a', '1'),
                         Key('z', '2'),
                         Key('e', '3'),
                         Key('r', '4'),
                         Key('t', '5'),
                         Key('y', '6'),
                         Key('u', '7'),
                         Key('i', '8'),
                         Key('o', '9'),
                         Key('p', '0')});
        }
        else if(keyboardRow == 2)
        {
            _data.append({Key('q', '@'),
                         Key('s', '#'),
                         Key('d', 8364), // Unicode code for "€"
                         Key('f', '%'),
                         Key('g', '&'),
                         Key('h', '-'),
                         Key('j', '+'),
                         Key('k', '*'),
                         Key('l', '('),
                         Key('m', ')')});
        }
        else if(keyboardRow == 3)
        {
            _data.append({Key('w', '_'),
                         Key('x', '"'),
                         Key('c', '\''),
                         Key('v', '?'),
                         Key('b', '!'),
                         Key('n', '/'),
                         Key('\'', '\\')});
        }
    }
    else
    {
        if(keyboardRow == 1)
        {
            _data.append({Key('q', '1'),
                         Key('w', '2'),
                         Key('e', '3'),
                         Key('r', '4'),
                         Key('t', '5'),
                         Key('y', '6'),
                         Key('u', '7'),
                         Key('i', '8'),
                         Key('o', '9'),
                         Key('p', '0')});
        }
        else if(keyboardRow == 2)
        {
            _data.append({Key('a', '!'),
                         Key('s', '@'),
                         Key('d', '#'),
                         Key('f', '$'),
                         Key('g', '%'),
                         Key('h', '&'),
                         Key('j', '*'),
                         Key('k', '?'),
                         Key('l', '/')});
        }
        else if(keyboardRow == 3)
        {
            _data.append({Key('z', '_'),
                         Key('x', '"'),
                         Key('c', '\''),
                         Key('v', '('),
                         Key('b', ')'),
                         Key('n', '-'),
                         Key('m', '+')});
        }
    }
}

//
// Re-implemented
//

int KeyboardRowModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _data.size();
}

QVariant KeyboardRowModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= _data.size())
        return QVariant();

    role = role - Qt::UserRole;
    if(role == 1)
        return _data.at(index.row()).letter;
    else if(role == 2)
        return _data.at(index.row()).symbol;

    return QVariant();
}

QHash<int, QByteArray> KeyboardRowModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole + 1] = "letter";
    roles[Qt::UserRole + 2] = "symbol";
    return roles;
}
