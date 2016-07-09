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
    id: settingsLanguage
    subActivity: true

    control: Rectangle {
        color: "transparent"

        Column {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 20

            StyledText {
                width: parent.width
                font.pixelSize: 17
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + qsTr("Kernel:") + "</b> " + sysinfoManager.kernelType
            }

            StyledText {
                width: parent.width
                font.pixelSize: 17
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + qsTr("Kernel version:") + "</b> " + sysinfoManager.kernelVersion
            }

            StyledText {
                width: parent.width
                font.pixelSize: 17
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + qsTr("CPU architecture:") + "</b> " + sysinfoManager.cpuArch
            }

            StyledText {
                width: parent.width
                font.pixelSize: 17
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + qsTr("Word size:") + "</b> " + sysinfoManager.wordSize
            }

            StyledText {
                width: parent.width
                font.pixelSize: 17
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + qsTr("Build ABI:") + "</b> " + sysinfoManager.buildAbi
            }

            StyledText {
                width: parent.width
                font.pixelSize: 17
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + qsTr("Build date:") + "</b> " + sysinfoManager.buildDate
            }

            StyledText {
                width: parent.width
                font.pixelSize: 17
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + qsTr("Bytes total:") + "</b> " + sysinfoManager.bytesTotalString
            }

            StyledText {
                width: parent.width
                font.pixelSize: 17
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + qsTr("Bytes available:") + "</b> " + sysinfoManager.bytesAvailableString
            }

            StyledText {
                width: parent.width
                font.pixelSize: 17
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + qsTr("Bytes used by system:") + "</b> " + sysinfoManager.bytesUsedSystemString
            }

            StyledText {
                width: parent.width
                font.pixelSize: 17
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + qsTr("Bytes used by config/cache:") + "</b> " + sysinfoManager.bytesUsedConfigString
            }
        }
    }
}
