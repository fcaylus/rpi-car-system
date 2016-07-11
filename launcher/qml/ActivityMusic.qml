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
import "music"
import "."

Activity {
    id: activityMusic

    property Component player: MusicPlayer {}
    property Component lister: MusicListView {}

    property bool playerVisible: false

    control: playerVisible ? player : lister

    //
    // Toolbar buttons
    //

    ToolbarIconButton {
        id: playButton
        anchors.bottom: parent.bottom
        x: availableToolBarWidth / 2 + Style.toolbar.height + 30

        iconScale: .7
        iconSource: "qrc:/images/play"
        hasSecondState: true
        secondIconSource: "qrc:/images/pause"

        onCurrentStateChanged: currentState == 2 ? musicPlayer.resumeMusic() : musicPlayer.pauseMusic()
        secondStateEnabled: musicPlayer.isPlaying
    }

    ToolbarIconButton {
        id: previousButton
        anchors.bottom: parent.bottom
        x: playButton.x - 95

        iconScale: .7
        iconSource: "qrc:/images/skip_previous"

        onClicked: musicPlayer.previousMusic()
    }

    ToolbarIconButton {
        id: nextButton
        anchors.bottom: parent.bottom
        x: playButton.x + 95

        iconScale: .7
        iconSource: "qrc:/images/skip_next"

        onClicked: musicPlayer.nextMusic()
    }

    ToolbarIconButton {
        id: randomButton
        anchors.bottom: parent.bottom
        x: playButton.x + 2 * 95

        iconScale: .7
        iconSource: "qrc:/images/shuffle"

        checkable: true
        onCheckedChanged: {
            musicPlayer.setRandom(checked)
        }

        Component.onCompleted: {
            if(musicPlayer.random) {
                checked = true
            }
        }
    }

    ToolbarIconButton {
        id: repeatButton
        anchors.bottom: parent.bottom
        x: playButton.x - 2 * 95

        iconScale: .7
        iconSource: "qrc:/images/repeat"

        checkable: true
        hasSecondState: true
        secondIconSource: "qrc:/images/repeat_one"

        onCurrentStateChanged: musicPlayer.setRepeatMode(currentState)

        Component.onCompleted: {
            var mode = musicPlayer.repeatMode
            if(mode !== 0) {
                checked = true
            }
            if(mode === 2) {
                secondStateEnabled = true
            }
        }
    }

    ToolbarIconButton {
        id: chooseButton
        anchors.bottom: parent.bottom
        x: playButton.x - 3.5 * 95

        iconScale: .7
        iconSource: "qrc:/images/music"

        checkable: true
        enabled: musicPlayer.started

        onCheckedChanged: playerVisible = !checked

        Component.onCompleted: {
            playerVisible = musicPlayer.isPlayerVisible()
            checked = !playerVisible
        }

        Connections {
            target: musicPlayer
            onNewMediaListPlayed: {
                if(musicPlayer.started) {
                    chooseButton.checked = false
                }
            }
        }
    }

    // Handle when the queue become empty and there is no more media available
    // (ie: when a source is disconnected)
    Connections {
        target: musicPlayer
        onStartedChanged: {
            if(!musicPlayer.started) {
                chooseButton.checked = true
            }
        }
    }

    // Used to add playlists
    KeyboardPrompt {
        id: addPlaylistPrompt
        visible: false
        standardInput: true
        titleText: qsTr("New playlist's name:")
        showBackButton: true

        onPromptFinish: {
            visible = false
        }
        onBackClicked: {
            visible = false
        }

        onVisibleChanged: {
            resetText()
        }
    }

    Component.onDestruction: {
        musicPlayer.setPlayerVisibility(playerVisible)
    }
}
