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

Rectangle {
    color: "transparent"

    property int coverWidth: 260
    property int rightWidth: Style.windowWidth - coverWidth - 30*3

    Column {
        spacing: 20
        x: 30

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Row {
            spacing: 30
            anchors.horizontalCenter: parent.horizontalCenter

            // Cover image
            Image {
                fillMode: Image.PreserveAspectFit
                asynchronous: true
                height: coverWidth
                width: height
                sourceSize.width: width
                sourceSize.height: height
                source: musicPlayer.mediaCover
            }

            Column {
                spacing: 6
                height: coverWidth

                Rectangle {
                    height: 1
                    width: rightWidth
                    color: Style.separatorColor
                }

                //
                // Title row
                //

                StyledText {
                    width: rightWidth
                    text: musicPlayer.mediaTitle
                    elide: Text.ElideRight
                    font.bold: true
                    font.pixelSize: 30
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                StyledText {
                    width: rightWidth
                    text: musicPlayer.mediaArtist
                    font.italic: true
                    font.pixelSize: 20
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                Rectangle {
                    height: 1
                    width: rightWidth
                    color: Style.separatorColor
                }
                Rectangle {
                    height: 1
                    width: rightWidth
                    color: Style.separatorColor
                }

                //
                // Volume row
                //

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 20

                    FlatButton {
                        width: 40
                        height: width
                        iconSource: "qrc:/images/minus"
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: {
                            volumeBar.value = volumeBar.value - .05
                        }
                    }

                    DarkProgressBar {
                        id: volumeBar
                        width: 150
                        anchors.verticalCenter: parent.verticalCenter
                        onValueChanged: musicPlayer.setVolume(value*100)
                        Component.onCompleted: {
                            value = musicPlayer.volume / 100
                        }
                    }

                    FlatButton {
                        width: 40
                        height: width
                        iconSource: "qrc:/images/plus"
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: {
                            volumeBar.value = volumeBar.value + .05
                        }
                    }
                }

                Rectangle {
                    height: 1
                    width: rightWidth
                    color: Style.separatorColor
                }

                //
                // Queue row
                //

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 40

                    FlatButton {
                        width: 50
                        height: width
                        iconSource: "qrc:/images/fast_rewind"
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: musicPlayer.rewind()
                    }

                    FlatButton {
                        id: queueButton
                        width: 70
                        height: width
                        iconSource: "qrc:/images/playlist"
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: musicQueuePopup.visible = true
                    }

                    FlatButton {
                        width: 50
                        height: width
                        iconSource: "qrc:/images/fast_forward"
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: musicPlayer.forward()
                    }
                }
            }
        }

        // Timeline
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 18

            DarkProgressBar {
                id: timeBar
                width: Style.windowWidth - 160
                anchors.verticalCenter: parent.verticalCenter
                value: 0
            }

            StyledText {
                anchors.verticalCenter: parent.verticalCenter
                text: musicPlayer.formatedTime
                font.italic: true
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Connections {
                target: musicPlayer
                onTimeChanged: timeBar.value = musicPlayer.time / 100
                onEndReached: timeBar.value = 1
            }
        }
    }

    MusicQueuePopup {
        id: musicQueuePopup
        visible: false
    }

}

