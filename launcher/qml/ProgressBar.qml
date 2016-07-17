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

import QtQuick 2.5
import rpicarsystem.controls 1.0
import "."

AbstractProgressBar {
    id: bar
    property bool selected: false

    implicitHeight: background.implicitHeight
    implicitWidth: background.implicitWidth

    background: Rectangle {
        radius: 3
        color: Style.progressBar.backgroundColor
        border.color: Style.progressBar.borderColor
        border.width: 2
        implicitHeight: bar.orientation === Qt.Horizontal ? 8 : 0
        implicitWidth: bar.orientation === Qt.Vertical ? 8 : 0
    }

    indicator: Rectangle {
        color: selected ? Style.progressBar.selectedColor : Style.progressBar.progressColor
        border.color: Style.progressBar.borderColor

        width: bar.orientation === Qt.Horizontal ? bar.width * bar.position : bar.width
        height: bar.orientation === Qt.Vertical ? bar.height * bar.position : bar.height

        anchors.bottom: bar.bottom
    }
}

