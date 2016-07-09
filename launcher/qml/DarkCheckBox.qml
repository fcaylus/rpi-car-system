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

CheckBox {

    property bool textBold: false

    style: CheckBoxStyle {
        indicator: Rectangle {
            border.width: 2
            border.color: control.checked ? Style.button.checkedOverlayColor : Style.progressBar.borderColor
            radius: 5
            color: "#F2111111"
            implicitWidth: 40
            implicitHeight: 40

            Image {
                asynchronous: true
                visible: control.checked
                anchors.margins: 2
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                fillMode: Image.PreserveAspectFit

                source: "qrc:/images/check"
            }
        }

        background: Rectangle {
            color: "transparent"
            height: control.height
            width: control.width
        }

        label: Text {
            text: control.text
            font.pixelSize: control.height * .6
            font.bold: control.textBold
            color: Style.fontColor
        }

        spacing: 15
    }
}
