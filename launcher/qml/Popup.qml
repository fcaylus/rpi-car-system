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

Rectangle {
    id: popup

    x: 0
    y: 0
    width: Style.windowWidth
    height: Style.windowHeight - Style.toolbar.height

    color: Style.popupBackground

    function setContent(component) {
        popupLoader.sourceComponent = component;
    }

    property bool closeIcon: true

    property int popupWidth: popup.width * .85
    property int popupHeight: popup.height * .9

    property int popupContentWidth: popupWidth - (closeIcon ? (75 + 15) : 0)
    property int popupContentHeight: popupHeight

    // This mouseArea disable background input
    MouseArea {
        anchors.fill: parent
        onClicked: {}
    }

    Rectangle {
        id: popupRect

        width: popupWidth
        height: popupHeight

        anchors.centerIn: parent

        border.color: Style.popupBorder
        border.width: 1

        color: Style.backgroundColor

        FlatButton {
            iconSource: "qrc:/images/close"
            width: 75
            height: 75
            anchors.right: parent.right
            anchors.top: parent.top

            onClicked: popup.visible = false

            visible: closeIcon
        }

        Loader {
            id: popupLoader
            anchors.fill: parent

            asynchronous: true
            visible: status == Loader.Ready
        }
    }
}

