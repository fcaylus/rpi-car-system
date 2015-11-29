/*
 * This file is part of RPI Car System.
 * Copyright (c) 2015 Fabien Caylus <toutjuste13@gmail.com>
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

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import "."

Rectangle {
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    height: Style.toolbar.height
    width: Style.toolbar.width

    gradient: Gradient {
        GradientStop {
            color: Style.toolbar.gradientStart
            position: 0
        }
        GradientStop {
            color: Style.toolbar.gradientEnd
            position: 1
        }
    }

    // Top Border
    Rectangle {
        height: 1
        width: parent.width
        anchors.top: parent.top
        color: Style.toolbar.topBorderColor
    }

    // Bottom border
    Rectangle {
        height: 1
        width: parent.width
        anchors.bottom: parent.bottom
        color: Style.toolbar.bottomBorderColor
    }

    // Home button
    // Back to the main menu on click
    DarkButton {
        id: back
        inToolbar: true
        width: parent.height
        height: parent.height
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        iconSource: "qrc:/images/home"
        onClicked: stackView.pop()
    }
}
