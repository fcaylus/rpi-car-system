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
import "."

Activity {
    id: activityTransfer

    control: Rectangle {
        width: controlBounds.width
        height: controlBounds.height
        color: "transparent"

        ListView {
            x: 30
            y: 15
            width: parent.width - 60
            height: parent.height - 30
            spacing: 10

            clip: true

            header: Item {
                id: header
                width: parent.width
                height: headerRow.implicitHeight + 10
                //clip: true

                Column {
                    id: headerRow
                    spacing: 5

                    StyledText {
                        text: qsTr("USB sticks list :")
                        font.pixelSize: 22
                        font.bold: true
                        font.italic: true
                    }

                    Rectangle {
                        height: 2
                        width: header.width
                        color: Style.separatorColor
                    }
                }
            }
        }
    }
}
