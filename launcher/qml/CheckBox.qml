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
    id: checkbox

    property string text
    property bool textBold: false

    checkable: true

    implicitWidth: label.implicitWidth
    implicitHeight: label.implicitHeight

    label: Row {
        spacing: 10

        Rectangle {
            id: borderRect
            border.width: 2
            border.color: checkbox.checked ? Style.button.checkedOverlayColor : Style.progressBar.borderColor
            radius: 5
            color: "#F2111111"
            implicitWidth: 40
            implicitHeight: 40

            Image {
                asynchronous: true
                visible: checkbox.checked
                anchors.margins: 2
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                fillMode: Image.PreserveAspectFit

                source: "qrc:/images/check"
            }
        }

        Text {
            height: borderRect.implicitHeight
            font.pixelSize: height * .6
            font.bold: checkbox.textBold
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: Style.fontColor

            text: checkbox.text
        }
    }
}
