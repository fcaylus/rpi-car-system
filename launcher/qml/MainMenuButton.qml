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

BasicButton {
    id: but

    property url iconSource
    property string text

    background: Rectangle {
        radius: 20

        color: but.pressed ? Style.button.colorPressed : Style.button.color

        border.color: Style.button.borderColor
        border.width: 1
    }

    label: Column {
        spacing: 5

        Image {
            asynchronous: true
            height: but.height * .7
            width: but.width
            sourceSize: Qt.size(height, height)
            fillMode: Image.PreserveAspectFit
            horizontalAlignment: Image.AlignHCenter

            source: but.iconSource
        }

        StyledText {
            width: but.width
            text: but.text
            font.pixelSize: but.height * .13
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            color: but.enabled ? Style.fontColor : Style.fontColorDisabled
        }
    }
}
