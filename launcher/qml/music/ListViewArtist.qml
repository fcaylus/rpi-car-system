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

ListViewBase {
    model: MetadataListModel {
        id: model

        requiredMeta: meta
        requiredMetaValue: metaValue

        Component.onCompleted: {
            addDisplayedMeta(MediaInfo.ARTIST)
        }
    }

    Component.onCompleted: model.update()

    delegate: Item {
        width: artistRow.implicitWidth
        height: artistRow.implicitHeight

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
                text: artist
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
                loader.headerText = qsTr("Artist: ") + artist
                loader.meta = MediaInfo.ARTIST
                loader.metaValue = artist
                loader.inPlaylist = false
                loader.playlistFile = ""
                loader.source = "qrc:/qml/music/ListViewAlbum.qml"
            }
        }
    }

    onVisibleChanged: {
        if(visible) {
            model.update()
        }
    }
}
