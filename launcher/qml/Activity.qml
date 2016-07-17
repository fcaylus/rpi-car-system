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

Rectangle {
    id: view
    color: Style.backgroundColor

    property AbstractStackView mainStackView
    // Depth in the stack view
    // Usually 2 for top activities and 3 for subActivities
    property int activityDepth: 2
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

        // Home button
        // Back to the main menu on click
        ToolbarIconButton {
            id: back
            width: parent.height

            anchors.left: parent.left
            anchors.bottom: parent.bottom
            iconSource: subActivity ? "qrc:/images/back" : "qrc:/images/home"
            onClicked: mainStackView.pop()
        }
    }

    PropertyAnimation {
        id: pushAnim
        target: view
        property: "x"
        from: Style.windowWidth
        to: 0

        onStopped: mainStackView.finishPush()
    }

    // Started when the activity is poped
    PropertyAnimation {
        id: popAnim
        target: view
        property: "x"
        from: 0
        to: Style.windowWidth

        onStopped: mainStackView.finishPop()
    }

    Connections {
        target: mainStackView
        onItemPoped: {
            if(mainStackView.depth === activityDepth - 1) {
                z = 1
                popAnim.start()
            }
        }
    }

    Component.onCompleted: pushAnim.start()
}
