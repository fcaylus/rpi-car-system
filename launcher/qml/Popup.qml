/*
 * This file is part of RPI Car System.
 * Copyright (c) 2016 Fabien Caylus <toutjuste13@gmail.com>
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
import ".."
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

    // This mouseArea disable backgound input
    MouseArea {
        anchors.fill: parent
        onClicked: {}
    }

    Rectangle {
        id: popupRect

        width: popupWidth
        height: popupHeight

        anchors.centerIn: parent

        border.color: Style.button.topBorderColor
        border.width: 1

        gradient: Gradient {
            GradientStop {
                color: Style.backgroundColorEnd
                position: 0
            }
            GradientStop {
                color: Style.backgroundColorStart
                position: .5
            }
            GradientStop {
                color: Style.backgroundColorEnd
                position: 1
            }
        }

        FlatButton {
            iconSource: "qrc:/images/close"
            width: 75
            height: width
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

