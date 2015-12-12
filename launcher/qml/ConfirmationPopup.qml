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
import ".."
import "."

Rectangle {
    id: popup

    x: 0
    y: 0
    width: Style.windowWidth
    height: Style.windowHeight - Style.toolbar.height

    color: Style.popupBackground

    property int fontSize: 28
    property string title
    property bool bold: false

    property string userData

    property bool highlightYesButton: true
    property bool highlightNoButton: false

    signal exitSuccess()

    // This mouseArea disable backgound input
    MouseArea {
        anchors.fill: parent
        onClicked: {}
    }

    Rectangle {
        id: popupRect

        width: parent.width * .85
        height: parent.height * .9

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

        StyledText {
            id: titleText

            anchors.left: parent.left
            anchors.top: parent.top
            width: parent.width - 60

            anchors.leftMargin: 30
            anchors.topMargin: 30

            font.pixelSize: fontSize
            font.bold: bold

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            textFormat: Text.RichText
            wrapMode: Text.Wrap

            text: title
        }

        // Yes and no buttons

        PopupButton {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 50
            anchors.left: parent.left
            anchors.leftMargin: 120
            height: 80
            width: 150
            text: qsTr("Yes")
            yes: highlightYesButton

            onClicked: {
                exitSuccess()
                popup.visible = false
            }
        }

        PopupButton {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 50
            anchors.right: parent.right
            anchors.rightMargin: 120
            height: 80
            width: 150
            text: qsTr("No")
            yes: highlightNoButton

            onClicked: {
                popup.visible = false
            }
        }
    }
}

