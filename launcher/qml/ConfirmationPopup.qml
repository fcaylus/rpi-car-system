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
import ".."
import "."

Popup {
    id: confirmPopup

    closeIcon: false

    property int fontSize: 28
    property string title
    property bool bold: false

    property string userData
    property string userData2
    property string userData3

    property bool highlightYesButton: true
    property bool highlightNoButton: false

    signal exitSuccess()

    Component {
        id: popupContent

        Rectangle {
            color: "transparent"

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
                textFormat: Text.StyledText
                wrapMode: Text.Wrap

                text: title
            }

            //
            // Yes and no buttons

            PopupButton {
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 50
                anchors.left: parent.left
                anchors.leftMargin: 120

                height: 80
                width: 150
                text: qsTr("Yes")
                highlight: highlightYesButton

                onClicked: {
                    exitSuccess()
                    confirmPopup.visible = false
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
                highlight: highlightNoButton

                onClicked: confirmPopup.visible = false
            }
        }
    }

    Component.onCompleted: setContent(popupContent)
}

