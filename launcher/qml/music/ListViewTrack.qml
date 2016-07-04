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
import rpicarsystem.mediamanager 1.0
import ".."
import "."

ListViewBase {
    model: MusicListModel {
        id: model

        requiredMeta: meta
        requiredMetaValue: metaValue

        fromPlaylist: inPlaylist
        playlistFileName: playlistFile
    }

    Component.onCompleted: model.update()

    delegate: Item {
        width: trackRow.implicitWidth
        height: trackRow.implicitHeight

        Row {
            id: trackRow
            spacing: 20
            Image {
                asynchronous: true
                width: 65
                height: width
                sourceSize.width: width
                sourceSize.height: height
                source: cover
            }

            StyledText {
                text: title
                font.pixelSize: 22
                //font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            anchors.fill: trackRow
            onClicked: {
                if(fromPlaylist) {
                    musicPlayer.playFromPlaylist(playlistFile, mediaUri)
                } else {
                    musicPlayer.play(mediaUri)
                }
            }

            onPressAndHold: {
                if(fromPlaylist) {
                    removeFromPlaylistPopup.userData = playlistFile
                    removeFromPlaylistPopup.userData2 = mediaUri
                    removeFromPlaylistPopup.userData3 = title
                    removeFromPlaylistPopup.visible = true
                } else {
                    playlistPopup.requestMusicAddition(mediaUri, title, cover)
                }
            }
        }
    }
}
