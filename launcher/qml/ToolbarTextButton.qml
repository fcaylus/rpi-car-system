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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import "."

Button {
    property bool alignCenter: true

    property bool bold: true
    property real fontRatio: .35

    height: Style.toolbar.height

    style: ButtonStyle {
        background: Rectangle {
            color: control.pressed ? Style.button.colorPressed : Style.button.color
        }

        label: StyledText {
            id: textLabel
            text: control.text
            font.pixelSize: control.height * fontRatio
            horizontalAlignment: alignCenter ? Text.AlignHCenter : Text.AlignLeft
            verticalAlignment: Text.AlignVCenter

            font.bold: bold

            // Handle clicked and checked states
            Connections {
                target: control
                onPressedChanged: {
                    textLabel.color = pressed ? Style.button.clickedOverlayColor : Style.fontColor
                }
                onCheckedChanged: {
                    textLabel.color = checked ? Style.button.checkedOverlayColor : Style.fontColor
                }
            }
        }
    }
}
