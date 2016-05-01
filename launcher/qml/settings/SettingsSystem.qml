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
                text: "<b>" + qsTr("Kernel:") + "</b> " + sysinfoManager.kernelType()
            }

            StyledText {
                width: parent.width
                font.pixelSize: 17
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + qsTr("Kernel version:") + "</b> " + sysinfoManager.kernelVersion()
            }

            StyledText {
                width: parent.width
                font.pixelSize: 17
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + qsTr("CPU architecture:") + "</b> " + sysinfoManager.cpuArch()
            }

            StyledText {
                width: parent.width
                font.pixelSize: 17
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + qsTr("Word size:") + "</b> " + sysinfoManager.wordSize()
            }

            StyledText {
                width: parent.width
                font.pixelSize: 17
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + qsTr("Build ABI:") + "</b> " + sysinfoManager.buildAbi()
            }

            StyledText {
                width: parent.width
                font.pixelSize: 17
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + qsTr("Build date:") + "</b> " + buildDate
            }
        }

        // Disk Usage
        Rectangle {
            id: diskSpaceUsage
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            anchors.margins: 20
            height: 50 + border.width * 2

            color: Style.diskSpace.free
            border.color: Style.diskSpace.border
            border.width: 3

            property int lineWidth: Style.windowWidth - anchors.margins * 2

            Row {
                anchors.fill: parent
                anchors.margins: parent.border.width
                spacing: 0

                // System used
                Rectangle {
                    height: parent.height
                    color: Style.diskSpace.systemUsed

                    Component.onCompleted: {
                        width = ((devicesManager.rootStorageSize()
                                  - devicesManager.rootStorageAvailableSize()
                                  - devicesManager.musicDirSize()) / devicesManager.rootStorageSize()) * diskSpaceUsage.lineWidth
                    }
                }

                // Music used
                Rectangle {
                    height: parent.height
                    color: Style.diskSpace.musicUsed

                    Component.onCompleted: {
                        width = (devicesManager.musicDirSize() / devicesManager.rootStorageSize()) * diskSpaceUsage.lineWidth
                    }
                }
            }
        }

        StyledText {
            anchors.bottom: diskSpaceUsage.top
            anchors.left: parent.left
            anchors.right: parent.right

            anchors.margins: 20

            font.pixelSize: 20
            verticalAlignment: Text.AlignVCenter
            text: "<b>" + qsTr("Free space:") + "</b> " + devicesManager.rootStorageAvailableSizeStr()
        }
    }
}
