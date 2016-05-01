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

import QtQuick 2.3
import QtQuick.Controls 1.2
import "."
import ".."

Activity {
    id: settingsLanguage
    subActivity: true

    control: Rectangle {
        color: "transparent"

        ListView {
            x: 30
            y: 15
            width: parent.width - 60
            height: parent.height - 30
            spacing: 15

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
                        text: qsTr("Languages list:")
                        font.pixelSize: 32
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

            model: ListModel {
                id: model

                // Use native language for each item
                ListElement {
                    name: "English"
                    code: "en"
                    icon: "qrc:/flags/uk"
                }
                ListElement {
                    name: "Français"
                    code: "fr"
                    icon: "qrc:/flags/france"
                }
            }

            delegate: Item {
                width: trackRow.implicitWidth
                height: trackRow.implicitHeight
                //clip: true

                Row {
                    id: trackRow
                    spacing: 30
                    Image {
                        asynchronous: true
                        width: 95
                        height: width
                        sourceSize.width: width
                        sourceSize.height: height
                        source: icon
                    }

                    StyledText {
                        text: name
                        font.pixelSize: 30
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                MouseArea {
                    anchors.fill: trackRow
                    onClicked: {
                        if(languageManager.currentLanguage() !== code) {
                            languageManager.changeLanguage(code);
                        }
                    }
                }
            }
        }
    }
}
