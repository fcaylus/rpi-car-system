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
import ".."
import "."

Rectangle {
    id: musicQueuePopup

    x: 0
    y: 0
    width: Style.windowWidth
    height: Style.windowHeight - Style.toolbar.height

    color: Style.popupBackground

    function centerView() {
        queueListView.positionViewAtIndex(soundManager.currentIndex(), ListView.Center)
    }

    // This mouseArea disable backgound input
    MouseArea {
        anchors.fill: parent
        onClicked: {}
    }

    Rectangle {

        width: parent.width * .85
        height: parent.height * .9

        anchors.centerIn: parent

        border.color: Style.button.topBorderColor
        border.width: 1

        gradient: Gradient {
            GradientStop {
                color: Style.backgroundColorEnd
                position: 0
            }
            GradientStop {
                color: Style.backgroundColorStart
                position: .5
            }
            GradientStop {
                color: Style.backgroundColorEnd
                position: 1
            }
        }

        FlatButton {
            id: closeIcon
            iconSource: "qrc:/images/close"
            width: 80
            height: width
            anchors.right: parent.right
            anchors.top: parent.top

            onClicked: {
                musicQueuePopup.visible = false
            }
        }

        ListViewBase {
            id: queueListView

            width: parent.width - closeIcon.width - 15
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.leftMargin: 15
            anchors.topMargin: 15
            anchors.bottomMargin: 15

            property string headerText: qsTr("Music queue ...")

            model: ListModel {
                id: model
            }

            delegate: Item {
                width: queueListView.width
                height: queueRow.implicitHeight

                Row {
                    id: queueRow
                    spacing: 20

                    Image {
                        asynchronous: true
                        width: 65
                        height: width
                        source: cover
                    }

                    StyledText {
                        text: name
                        font.pixelSize: 22
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                MouseArea {
                    anchors.fill: queueRow
                    onClicked: {
                        soundManager.playFromIndex(index)
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
                model.clear()
                var titlesList = soundManager.currentMediaQueueTitles
                var coversList = soundManager.currentMediaQueueCovers
                for (var i=0; i < titlesList.length; i++) {
                    model.append({"name": titlesList[i],
                                     "cover": coversList[i],
                                     "index": i})
                }
                queueListView.positionViewAtIndex(soundManager.currentIndex(), ListView.Center)
                queueListView.currentIndex = soundManager.currentIndex();
            }

            Connections {
                target: soundManager
                onCurrentMediaQueueChanged: queueListView.fillData()
                onMediaTitleChanged: queueListView.currentIndex = soundManager.currentIndex()
            }

            Component.onCompleted: fillData()
        }

    }
}

