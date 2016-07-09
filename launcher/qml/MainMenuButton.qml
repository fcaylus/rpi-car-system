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
    style: ButtonStyle {
        background: Rectangle {
            radius: 20

            color: control.pressed ? Style.button.colorPressed : Style.button.color

            border.color: Style.button.borderColor
            border.width: 1
        }

        label: Item {
            implicitWidth: col.implicitWidth
            implicitHeight: col.implicitHeight

            Column {
                id: col
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 5

                Image {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    asynchronous: true
                    source: control.iconSource
                    height: control.height * .7
                    width: Math.min(sourceSize.width, height)
                    fillMode: Image.PreserveAspectFit
                    horizontalAlignment: Image.AlignHCenter
                }

                StyledText {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: control.text
                    font.pixelSize: control.height * .13
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    color: control.enabled ? Style.fontColor : Style.fontColorDisabled
                }
            }
        }
    }
}
