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
import QtQuick.XmlListModel 2.0
import ".."
import "."

ListViewBase {
    id: playlistListView
    property bool newPlaylistRequested: false

    property bool addMode: false

    model: ListModel {
        id: model
    }

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
                if(file == "%%new%%") {
                    addPlaylistPrompt.visible = true
                    playlistListView.newPlaylistRequested = true
                }
                else if(addMode === true) {
                    soundManager.addSongToPlaylist(file, newTitle, newMusicFile, newCover);
                    playlistPopup.visible = false
                }
                else {
                    loader.appendLastEntry()
                    loader.headerText = qsTr("Playlist: ") + name
                    loader.sourceFile = soundManager.playlistUrl(file)
                    loader.sourceQuery = "/playlist/track"
                    loader.source = "qrc:/qml/music/ListViewTrack.qml"
                }
            }

            onPressAndHold: {
                if(!addMode) {
                    removePlaylistPopup.userData = name
                    removePlaylistPopup.userData2 = file
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
                soundManager.playFromPlaylist(file)
            }
        }
    }

    function fillData() {
        model.clear()
        var namesList = soundManager.playlistNames
        var filesList = soundManager.playlistFiles
        for (var i=0; i < namesList.length; i++) {
            model.append({"name": namesList[i],
                             "file": filesList[i],
                             "isRealPlaylist": true})
        }

        model.append({"name": qsTr("New playlist"),
                         "file": "%%new%%",
                         "isRealPlaylist": false})
    }

    // Update playlist when a new one was created
    Connections {
        target: addPlaylistPrompt
        onPromptFinish: {
            if(newPlaylistRequested) {
                newPlaylistRequested = false
                soundManager.createNewPlaylist(text)
                playlistListView.fillData()
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
            playlistListView.fillData()
        }
    }

    onVisibleChanged: {
        if(visible) {
            fillData()
        }
    }

    Component.onCompleted: fillData()

    // Update playlists when the index change
    Connections {
        target: soundManager
        onMediaListReadyChanged: playlistListView.fillData()
    }


}
