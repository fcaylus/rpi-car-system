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
import ".."

BasicButton {
    id: but
    property string text

    height: 60

    background: Rectangle {
        color: "transparent"
        border.color: Style.separatorColor
        border.width: 3
        radius: 15
    }

    label: StyledText {
        id: hText
        text: but.text
        font.pixelSize: but.height * .45
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        font.bold: true

        // Handle clicked and checked states
        Connections {
            target: but
            onPressedChanged: {
                hText.color = pressed ? Style.button.clickedOverlayColor : Style.fontColor
            }
            onCheckedChanged: {
                hText.color = checked ? Style.button.checkedOverlayColor : Style.fontColor
            }
        }
    }
}
