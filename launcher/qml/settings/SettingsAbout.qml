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
import QtQuick.XmlListModel 2.0
import ".."
import "."

Activity {
    id: settingsLanguage
    subActivity: true

    // Toolbars button
    DarkButton {
        id: aboutButton
        inToolbar: true
        anchors.bottom: parent.bottom
        height: Style.toolbar.height
        text: qsTr("About")
        bold: true
        checkable: true
        x: licenseButton.x - 35 - width
        onClicked: {
            licenseButton.checked = false
            license3rdButton.checked = false
            control = about
            checked = true
        }
        checked: true
    }

    DarkButton {
        id: licenseButton
        inToolbar: true
        anchors.bottom: parent.bottom
        height: Style.toolbar.height
        text: qsTr("Licenses")
        bold: true
        checkable: true
        x: availableToolBarWidth / 2 - width / 2 + 35
        onClicked: {
            aboutButton.checked = false
            license3rdButton.checked = false
            control = license
            checked = true
        }
    }

    DarkButton {
        id: license3rdButton
        inToolbar: true
        anchors.bottom: parent.bottom
        height: Style.toolbar.height
        text: qsTr("3rd Licenses")
        bold: true
        checkable: true
        x: licenseButton.x + licenseButton.width + 35
        onClicked: {
            licenseButton.checked = false
            aboutButton.checked = false
            control = license3rd
            checkable = true
        }
    }

    property Component about: Rectangle {
        width: controlBounds.width
        height: controlBounds.height
        color: "transparent"

        Column {
            width: parent.width
            height: parent.height - 30
            y:25

            spacing: 21

            StyledText {
                width: parent.width
                font.pixelSize: 50
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                text: "RPI Car System"
            }

            StyledText {
                width: parent.width
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: "Copyright (c) 2015 Fabien CAYLUS"
            }

            StyledText {
                width: parent.width
                font.pixelSize: 30
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: qsTr("Full featured headunit system !");
            }

            StyledText {
                width: parent.width
                font.pixelSize: 22
                font.italic: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                text: "<b>" +  qsTr("Version:") + "</b>   " + programVersion
                      + "<br><b>" + qsTr("Hardware version:") + "</b>   " + hardwareVersion
            }

            StyledText {
                width: parent.width
                font.pixelSize: 15
                font.italic: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
                text: "<b>" +  qsTr("LibVLC version:") + "</b>   " + vlcVersion
                      + "<br><b>" + qsTr("VLC-Qt version:") + "</b>   " + vlcqtVersion
            }
        }
    }

    property Component license: Rectangle {
        width: controlBounds.width
        height: controlBounds.height
        color: "transparent"

        Flickable {
            id: flick
            anchors.fill: parent
            contentHeight: text.height

            StyledText {
                id: text
                x: 20
                width: parent.width - 40
                textFormat: Text.RichText
                wrapMode: Text.Wrap
                verticalAlignment: Text.AlignVCenter
                text: fileReader.read(":/LICENSE.html")
            }
        }
    }

    property Component license3rd: Rectangle {
        width: controlBounds.width
        height: controlBounds.height
        color: "transparent"

        ListViewBase {
            id: listView3rd

            x: 20
            y: 20
            width: parent.width - 40
            height: parent.height - 40

            spacing: 20

            // Reset header
            header: Item {
            }

            model: XmlListModel {
                id: model

                source: "qrc:/ThirdpartyLicenses.xml"
                query: "/deps/dep"

                XmlRole { name: "name"; query: "name/string()" }
                XmlRole { name: "license"; query: "license/string()" }
            }

            delegate: StyledText {
                width: listView3rd.width
                textFormat: Text.RichText
                wrapMode: Text.Wrap
                verticalAlignment: Text.AlignVCenter
                text: "<h1><b>" + name + "</b></h1>\n" + license
            }
        }
    }

    control: about
}
