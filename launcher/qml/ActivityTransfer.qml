/*
 * This file is part of RPI Car System.
 * Copyright (c) 2016 Fabien Caylus <toutjuste13@gmail.com>
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
import QtGraphicalEffects 1.0
import "."

Activity {
    id: activityTransfer

    //
    // Add toolbars buttons

    DarkButton {
        id: refreshButton
        inToolbar: true
        anchors.bottom: parent.bottom
        height: Style.toolbar.height

        text: qsTr("Refresh")
        bold: true

        x: availableToolBarWidth / 2 - width / 2 + Style.toolbar.height
        onClicked: {
            devicesManager.refreshDevicesList()
        }
    }

    DarkButton {
        id: deleteButton
        inToolbar: true
        anchors.bottom: parent.bottom
        height: Style.toolbar.height

        iconSource: "qrc:/images/delete"
        iconScale: 0.6

        x: Style.windowWidth - Style.toolbar.height * iconScale - 20
        onClicked: {
            confirmDelete.visible = true
        }
    }

    control: Rectangle {
        color: "transparent"

        property string headerText: qsTr("USB sticks list :")

        StyledText {
            id: noDevices

            anchors.centerIn: parent
            width: parent.width
            font.pixelSize: 33
            font.bold: true

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: qsTr("Please insert a USB stick !");

            visible: false
        }

        ListViewBase {
            id: listView

            x: 30
            y: 15
            width: parent.width - 60
            height: parent.height - 30
            spacing: 10

            model: ListModel {
                id: model
            }

            delegate: Item {
                width: row.implicitWidth
                height: row.implicitHeight

                Row {
                    id: row
                    spacing: 20
                    Image {
                        asynchronous: true
                        width: 65
                        height: width
                        sourceSize.width: width
                        sourceSize.height: height
                        source: "qrc:/images/usb"
                    }

                    StyledText {
                        text: title
                        font.pixelSize: 24
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                MouseArea {
                    anchors.fill: row
                    onClicked: {
                        confirmTransfer.visible = true
                        confirmTransfer.userData = path
                    }
                }
            }

            function fillData() {
                model.clear()
                var pathsList = devicesManager.availableDevicesPath()
                var namesList = devicesManager.availableDevicesName()

                if (pathsList.length === 0) {
                    noDevices.visible = true
                } else {
                    noDevices.visible = false
                }

                for (var i=0; i < pathsList.length; i++) {
                    model.append({"title": namesList[i], "path": pathsList[i]})
                }
            }

            Connections {
                target: devicesManager
                onDevicesListRefreshed: listView.fillData()
            }

            Component.onCompleted: fillData()
        }

        ConfirmationPopup {
            id: confirmTransfer
            title: qsTr("Are you sure to transfer all music files from this device ?")
            visible: false

            onExitSuccess: {
                devicesManager.copyDeviceFiles(userData)
            }
        }

        WaitPopup {
            id: waitTransfer
            title: qsTr("Copy all files from device ...");
            fontSize: 35
            visible: false
        }

        Connections {
            target: devicesManager
            onNewTaskStarted: {
                if(taskName === "copy") {
                    waitTransfer.setupProgressBar(taskLength);
                    waitTransfer.visible = true
                    confirmTransfer.visible = false
                }
            }

            onTaskUpdate: {
                if(taskName === "copy") {
                    waitTransfer.updateProgressBar(value);
                }
            }

            onTaskFinished: {
                if(taskName === "copy") {
                    waitTransfer.visible = false
                }
            }
        }
    }

    ConfirmationPopup {
        id: confirmDelete
        title: qsTr("Are you sure to delete all media stored in the radio ?")
        visible: false

        highlightNoButton: true
        highlightYesButton: false

        onExitSuccess: {
            devicesManager.deleteLocalFiles();
        }
    }

    WaitPopup {
        id: waitDelete
        title: qsTr("Deleting local media files ...");
        fontSize: 35
        visible: false
    }

    Connections {
        target: devicesManager
        onNewTaskStarted: {
            if(taskName === "delete") {
                waitDelete.setupProgressBar(taskLength);
                waitDelete.visible = true
                confirmDelete.visible = false
            }
        }

        onTaskUpdate: {
            if(taskName === "delete") {
                waitDelete.updateProgressBar(value);
            }
        }

        onTaskFinished: {
            if(taskName === "delete") {
                waitDelete.visible = false
            }
        }
    }
}
