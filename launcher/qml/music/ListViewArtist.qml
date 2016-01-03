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
import QtQuick.XmlListModel 2.0
import ".."
import "."

ListViewBase {
    id: listViewTrack

    model: XmlListModel {
        id: model

        source: sourceFile
        query: sourceQuery

        XmlRole { name: "name"; query: "@name/string()" }
    }

    delegate: Item {
        width: artistRow.implicitWidth
        height: artistRow.implicitHeight
        //clip: true

        Row {
            id: artistRow
            spacing: 20
            Image {
                asynchronous: true
                width: 65
                height: width
                sourceSize.width: width
                sourceSize.height: height
                source: "qrc:/images/person"
            }

            StyledText {
                text: name
                font.pixelSize: 22
                //font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            anchors.fill: artistRow
            onClicked: {
                loader.appendLastEntry()
                // show the selected artist
                loader.headerText = qsTr("Artist: ") + name
                loader.sourceQuery = loader.sourceQuery + "[@name='" + name + "']/album"
                loader.source = "qrc:/qml/music/ListViewAlbum.qml"
            }
        }
    }
}
