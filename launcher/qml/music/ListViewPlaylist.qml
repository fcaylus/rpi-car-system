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
    id: playlistListView

    property bool newPlaylistRequested: false
    property bool addMode: false

    model: PlaylistListModel {
        id: model
    }

    Component.onCompleted: model.update()

    delegate: Item {
        width: playlistListView.width
        height: playlistRow.implicitHeight 

        Row {
            id: playlistRow
            spacing: 20

            Image {
                asynchronous: true
                width: 65
                height: width
                sourceSize.width: width
                sourceSize.height: height
                source: isRealPlaylist ? "qrc:/images/playlist" : "qrc:/images/playlist_add"
            }

            StyledText {
                text: name
                font.pixelSize: 22
                font.bold: isRealPlaylist ? true : false
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            anchors.fill: playlistRow
            onClicked: {
                // Create new playlist
                if(fileName == "%%new%%") {
                    addPlaylistPrompt.visible = true
                    playlistListView.newPlaylistRequested = true
                }
                else if(addMode === true) {
                    mediaManager.addMediaToPlaylist(fileName, newMusicFile);
                    playlistPopup.visible = false
                }
                else {
                    loader.appendLastEntry()

                    loader.headerText = qsTr("Playlist: ") + name
                    loader.meta = MediaInfo.UNKNOWN
                    loader.metaValue = undefined
                    loader.inPlaylist = true
                    loader.playlistFile = fileName
                    loader.source = "qrc:/qml/music/ListViewTrack.qml"
                }
            }

            onPressAndHold: {
                if(!addMode) {
                    removePlaylistPopup.userData = name
                    removePlaylistPopup.userData2 = fileName
                    removePlaylistPopup.visible = true
                }
            }
        }

        Image {
            id: playImage

            visible: isRealPlaylist && !addMode

            asynchronous: true
            width: 60
            height: width
            sourceSize.width: width
            sourceSize.height: height
            source: "qrc:/images/play_arrow"

            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
        }

        MouseArea {
            visible: isRealPlaylist && !addMode
            anchors.fill: playImage
            onClicked: {
                musicPlayer.playFromPlaylist(fileName)
            }
        }
    }

    // Update playlist when a new one was created
    Connections {
        target: addPlaylistPrompt
        onPromptFinish: {
            if(newPlaylistRequested) {
                newPlaylistRequested = false
                mediaManager.createPlaylist(text)
                //model.update()
            }
        }
        onBackClicked: {
            if(newPlaylistRequested) {
                newPlaylistRequested = false
            }
        }
    }

    Connections {
        target: removePlaylistPopup
        onExitSuccess: {
            model.update()
        }
    }
}
