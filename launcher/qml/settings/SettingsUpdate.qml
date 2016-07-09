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

Activity {
    id: settingsUpdate
    subActivity: true

    control: Rectangle {
        color: "transparent"

        property string headerText: qsTr("Available update packages :")

        StyledText {
            id: searchText

            anchors.centerIn: parent
            width: parent.width
            font.pixelSize: 35
            font.bold: true

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: qsTr("Searching ...");

            visible: false
        }

        StyledText {
            id: nothingText

            anchors.centerIn: parent
            width: parent.width
            font.pixelSize: 35
            font.bold: true

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: qsTr("No updates found !");

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
                        height: 65
                        sourceSize: Qt.size(65, 65)

                        source: "qrc:/images/unarchive"
                    }

                    Column {
                        anchors.verticalCenter: parent.verticalCenter

                        StyledText {
                            font.pixelSize: 24
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter

                            // For status code, see "updater/src/updatercodes.h"
                            color: status != 0 ? Style.dangerousColor : Style.safeColor
                            text: status == 0 ? qsTr("Update to version: %1").arg(version) :
                                                (status == 200 ? qsTr("Older package (version %1)").arg(version) :
                                                (status == 100 ? qsTr("Different hardware package (manual update): %1").arg(version) :
                                                                 qsTr("Malformed package !")))
                        }

                        StyledText {
                            font.pixelSize: 18
                            verticalAlignment: Text.AlignVCenter
                            color: status != 0 ? Style.dangerousColor : Style.safeColor
                            text: file
                        }
                    }
                }

                MouseArea {
                    anchors.fill: row
                    onClicked: {
                        if(status == 0) {
                            confirmUpdate.userData = version
                            confirmUpdate.userData2 = file
                            confirmUpdate.visible = true
                        }
                    }
                }
            }

            function sendRefreshRequest() {
                searchText.visible = true

                if(!updateManager.isRunning()) {
                    updateManager.refreshUpdateList()
                }
            }

            // Must be called when refresh is finished
            function fillData() {
                model.clear()

                var filesList = updateManager.updateFiles()
                var versionsList = updateManager.updateVersions()
                var statusList = updateManager.updateVersionStatus()

                if (filesList.length === 0) {
                    nothingText.visible = true
                } else {
                    nothingText.visible = false
                }

                for (var i=0; i < filesList.length; i++) {
                    model.append({"file": filesList[i], "version": versionsList[i], "status": statusList[i]})
                }

                searchText.visible = false
            }

            Connections {
                target: updateManager
                onRefreshFinished: listView.fillData()
            }

            Component.onCompleted: sendRefreshRequest()
        }

        // userData  -> version
        // userData2 -> file
        ConfirmationPopup {
            id: confirmUpdate
            visible: false

            title: qsTr("Are you sure to update your system to version %1 ?").arg(userData)
            highlightNoButton: true
            highlightYesButton: false

            onExitSuccess: {
                confirmUpdate2.userData = userData2
                confirmUpdate2.visible = true
            }
        }

        // userData -> file
        ConfirmationPopup {
            id: confirmUpdate2
            visible: false

            title: qsTr("The update can turn your system into a brick. Are you sure ?")
            highlightNoButton: true
            highlightYesButton: false

            onExitSuccess: {
                confirmUpdate3.userData = userData
                confirmUpdate3.visible = true
            }
        }

        // userData -> file
        ConfirmationPopup {
            id: confirmUpdate3
            visible: false

            title: qsTr("Really ?")
            highlightNoButton: true
            highlightYesButton: false

            onExitSuccess: {
                updateManager.launchUpdate(userData)
            }
        }
    }

    KeyboardPrompt {
        id: passwordConfirm
        visible: true
        titleText: qsTr("Enter root password to continue:")
        showBackButton: true

        onPromptFinish: {
            if(passwordManager.checkPassword(text)) {
                visible = false
            }
        }

        onBackClicked: {
            mainStackView.pop()
        }
    }
}
