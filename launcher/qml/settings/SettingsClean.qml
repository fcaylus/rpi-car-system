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
    id: settingsClean
    subActivity: true

    property bool allChecked: false
    property bool usbChecked: false
    property bool playlistChecked: false
    property bool equalizerChecked: false
    property bool settingsChecked: false

    ToolbarTextButton {
        id: cleanButton
        anchors.bottom: parent.bottom
        text: qsTr("Clean system")

        x: availableToolBarWidth / 2 - width / 2 + Style.toolbar.height
        onClicked: {
            var args = ""
            if(allChecked) {
                args = "--clear-all-cache"
            } else {
                if(usbChecked)
                    args += "--clear-usb-cache"
                if(settingsChecked)
                    args += " --clear-settings"
                if(playlistChecked)
                    args += " --clear-playlists"
                if(equalizerChecked)
                    args += " --clear-equalizer-confs"
            }

            confirmClean.userData = args
            confirmClean.visible = true
        }
    }

    control: Rectangle {
        color: "transparent"

        Column {
            anchors.fill: parent
            anchors.margins: 10
            anchors.leftMargin: 50

            spacing: 10

            StyledText {
                font.bold: true
                font.pixelSize: 30

                text: qsTr("Remove:")
            }

            DarkCheckBox {
                id: checkAll
                text: qsTr("All caches/configs")
                textBold: true
                onCheckedChanged: settingsClean.allChecked = checked
            }

            DarkCheckBox {
                id: checkUsbCover
                text: qsTr("USB devices cache")
                onCheckedChanged: settingsClean.usbChecked = checked
            }

            DarkCheckBox {
                id: checkPlaylist
                text: qsTr("Playlists")
                onCheckedChanged: settingsClean.playlistChecked = checked
            }

            DarkCheckBox {
                id: checkEqualizer
                text: qsTr("Equalizer configs")
                onCheckedChanged: settingsClean.equalizerChecked = checked
            }

            DarkCheckBox {
                id: checkSettings
                text: qsTr("Settings")
                onCheckedChanged: settingsClean.settingsChecked = checked
            }
        }
    }

    // userData -> args
    ConfirmationPopup {
        id: confirmClean
        visible: false

        title: qsTr("Are you sure to remove all these files ?")
        highlightNoButton: true
        highlightYesButton: false

        onExitSuccess: {
            confirmClean2.userData = userData
            confirmClean2.visible = true
        }
    }

    // userData -> args
    ConfirmationPopup {
        id: confirmClean2
        visible: false

        title: qsTr("Really ?")
        highlightNoButton: true
        highlightYesButton: false

        onExitSuccess: {
            sysinfoManager.rebootAndApplyArgs(userData)
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
