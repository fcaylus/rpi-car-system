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
import rpicarsystem.mediamanager 1.0
import ".."
import "."

Popup {
    id: musicQueuePopup

    Component {
        id: popupContent

        Rectangle {
            color: "transparent"

            ListViewBase {
                id: queueListView

                width: popupContentWidth
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                anchors.margins: 15

                property string headerText: qsTr("Music queue ...")

                model: MusicQueueListModel {
                    id: model
                    player: musicPlayer
                }

                delegate: Item {
                    width: queueListView.width
                    height: queueRow.implicitHeight

                    Row {
                        id: queueRow
                        spacing: 20

                        Image {
                            fillMode: Image.PreserveAspectFit
                            asynchronous: true
                            width: 65
                            height: 65
                            sourceSize: Qt.size(65, 65)

                            source: cover
                        }

                        StyledText {
                            text: title
                            font.pixelSize: 22
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    MouseArea {
                        anchors.fill: queueRow
                        onClicked: {
                            musicPlayer.playFromIndex(index)
                            musicQueuePopup.visible = false
                        }
                    }
                }

                highlight: Rectangle {
                    color: "transparent"

                    radius: 3
                    border.width: 2
                    border.color: Style.button.clickedOverlayColor
                }

                function fillData() {
                    model.update()
                    queueListView.positionViewAtIndex(musicPlayer.mediaIndex, ListView.Center)
                    queueListView.currentIndex = musicPlayer.mediaIndex;
                }

                Component.onCompleted: fillData()

                Connections {
                    target: musicPlayer
                    onMediaQueueChanged: queueListView.fillData()
                    onMediaTitleChanged: queueListView.currentIndex = musicPlayer.mediaIndex
                }

                // Center list on update
                onVisibleChanged: {
                    if(visible) {
                        positionViewAtIndex(musicPlayer.mediaIndex, ListView.Center)
                    }
                }
            }
        }
    }

    Component.onCompleted: setContent(popupContent)
}

