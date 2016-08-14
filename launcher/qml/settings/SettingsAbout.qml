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
import rpicarsystem.licenses 1.0
import ".."
import "."

Activity {
    id: settingsLanguage
    subActivity: true

    // Toolbars button
    ToolbarTextButton {
        id: aboutButton
        anchors.bottom: parent.bottom
        text: qsTr("About")

        checkable: true
        x: licenseButton.x - 35 - width
        onClicked: {
            licenseButton.checked = false
            license3rdButton.checked = false
            control = about
        }
        checked: true
    }

    ToolbarTextButton {
        id: licenseButton
        anchors.bottom: parent.bottom
        text: qsTr("License")

        checkable: true
        x: availableToolBarWidth / 2 - width / 2 + 35
        onClicked: {
            aboutButton.checked = false
            license3rdButton.checked = false
            control = license
        }
    }

    ToolbarTextButton {
        id: license3rdButton
        anchors.bottom: parent.bottom
        text: qsTr("3rd Licenses")

        checkable: true
        x: licenseButton.x + licenseButton.width + 35
        onClicked: {
            licenseButton.checked = false
            aboutButton.checked = false

            // Reload the component if already loader
            if(control === license3rd) {
                control = null
                checked = false
            }

            control = license3rd
        }
    }

    property Component about: StyledText {
        anchors.fill: parent

        font.pixelSize: 20
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        textFormat: Text.StyledText
        text: "<h1>RPI Car System</h1><h6>Copyright (c) 2016 Fabien CAYLUS</h6><br><h3>"
              + qsTr("Full featured headunit system !") + "</h3><br>"
              + "<b>" +  qsTr("Version:") + "</b>   " + sysinfoManager.programVersion
              + "<br><b>" + qsTr("Hardware version:") + "</b>   " + sysinfoManager.hardwareVersion
              + "<br><b>" +  qsTr("LibVLC version:") + "</b>   " + sysinfoManager.vlcVersion
              + "<br><b>" + qsTr("VLC-Qt version:") + "</b>   " + sysinfoManager.vlcqtVersion
    }

    property Component license: Flickable {
        anchors.fill: parent
        anchors.topMargin: 25
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        contentHeight: text.height

        StyledText {
            id: text
            width: parent.width
            textFormat: Text.RichText
            wrapMode: Text.Wrap
            verticalAlignment: Text.AlignVCenter
            text: sysinfoManager.license
        }
    }

    property Component license3rd: Item {
        id: license3rdView
        anchors.fill: parent
        anchors.margins: 20
        anchors.bottomMargin: 0

        property string licenseText

        Flickable {
            id: licenseViewer
            anchors.fill: parent
            contentHeight: licenseTextView.height

            visible: false

            StyledText {
                id: licenseTextView
                width: parent.width
                //textFormat: Text.StyledText
                wrapMode: Text.Wrap
                verticalAlignment: Text.AlignVCenter
                text: licenseText
            }
        }

        ListViewBase {
            id: listView3rd

            anchors.fill: parent
            spacing: 5

            // Reset header
            header: Item {
            }

            model: LicensesListModel {
            }

            delegate: StyledText {
                width: listView3rd.width
                textFormat: Text.StyledText
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.Wrap
                font.pixelSize: 18
                text: "<b>" + packageName + " " + version + " : </b>" + licenseName

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        license3rdView.licenseText = licenseText
                        licenseViewer.visible = true
                        listView3rd.visible = false
                    }
                }
            }
        }
    }

    control: about
}
