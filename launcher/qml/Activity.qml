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
import "."

Item {
    id: view

    property Component control

    // Substract the "home" button width
    property int availableToolBarWidth: toolbar.width - back.width

    // Change "home" icon to "back" icon
    property bool subActivity: false

    Loader {
        id: activityLoader
        sourceComponent: control

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: Style.windowHeight - Style.toolbar.height

        asynchronous: true
        visible: status == Loader.Ready
    }

    // Set the toolbar
    Rectangle {
        id: toolbar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: Style.toolbar.height

        color: Style.toolbar.color

        // Top Border
        Rectangle {
            height: 1
            width: parent.width
            anchors.top: parent.top
            color: Style.toolbar.topBorderColor
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
            iconSource: subActivity ? "qrc:/images/back" : "qrc:/images/home"
            onClicked: mainStackView.pop()
        }
    }
}
