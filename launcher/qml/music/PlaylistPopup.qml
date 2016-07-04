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
import ".."
import "."

Popup {
    id: playlistPopup

    property string newTitle
    property string newMusicFile

    Component {
        id: popupContent

        Rectangle {
            color: "transparent"

            ListViewPlaylist {
                id: listView
                width: popupContentWidth
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.leftMargin: 15
                anchors.topMargin: 15
                anchors.bottomMargin: 15
                addMode: true

                property string headerText: qsTr("Add %1 to ...").arg(newTitle)
            }
        }
    }

    function requestMusicAddition(musicFile, title, cover)
    {
        newMusicFile = musicFile
        newTitle = title
        visible = true
    }

    Component.onCompleted: setContent(popupContent)
}

