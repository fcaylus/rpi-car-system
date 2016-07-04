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
import "."
import "music"

Activity {
    id: activityMusic

    property Component player: MusicPlayer {}
    property Component lister: MusicListView {}

    property bool playerVisible: false

    control: playerVisible ? player : lister

    //
    // Toolbar buttons
    //

    DarkButton {
        id: playButton
        inToolbar: true
        anchors.bottom: parent.bottom
        height: Style.toolbar.height
        iconScale: .7
        iconSource: "qrc:/images/play"
        hasSecondIcon: true
        secondIcon: "qrc:/images/pause"

        x: availableToolBarWidth / 2 + Style.toolbar.height + 30

        disableAutoMgr: true
        onClicked: {
            if(currentState == 0)
                musicPlayer.resumeMusic()
            else
                musicPlayer.pauseMusic()
        }

        currentState: musicPlayer.isPlaying ? 1 : 0
    }

    DarkButton {
        id: previousButton
        inToolbar: true
        anchors.bottom: parent.bottom
        height: Style.toolbar.height
        iconScale: .7
        iconSource: "qrc:/images/skip_previous"

        x: playButton.x - 95

        onClicked: musicPlayer.previousMusic()
    }

    DarkButton {
        id: nextButton
        inToolbar: true
        anchors.bottom: parent.bottom
        height: Style.toolbar.height
        iconScale: .7
        iconSource: "qrc:/images/skip_next"

        x: playButton.x + 95

        onClicked: musicPlayer.nextMusic()
    }

    DarkButton {
        id: randomButton
        inToolbar: true
        anchors.bottom: parent.bottom
        height: Style.toolbar.height
        iconScale: .7
        iconSource: "qrc:/images/shuffle"

        checkable: true
        disableAutoMgr: true

        x: playButton.x + 2 * 95

        onClicked: musicPlayer.setRandom(checked)
        currentState: musicPlayer.random ? 0 : 1

        Component.onCompleted: {
            if(musicPlayer.random) {
                checked = true
            }
        }
    }

    DarkButton {
        id: repeatButton
        inToolbar: true
        anchors.bottom: parent.bottom
        height: Style.toolbar.height
        iconScale: .7
        iconSource: "qrc:/images/repeat"

        checkable: true
        hasSecondIcon: true
        secondIcon: "qrc:/images/repeat_one"

        x: playButton.x - 2 * 95

        disableAutoMgr: true
        onClicked: musicPlayer.setRepeatMode((currentState + 1) % 3)

        Connections {
            target: musicPlayer
            onRepeatModeChanged: {
                repeatButton.currentState = musicPlayer.repeatMode
                repeatButton.updateCheckedState()
            }
        }

        Component.onCompleted: {
            repeatButton.currentState = musicPlayer.repeatMode
            repeatButton.updateCheckedState()
        }
    }

    DarkButton {
        id: chooseButton
        inToolbar: true
        anchors.bottom: parent.bottom
        height: Style.toolbar.height
        iconScale: .7
        iconSource: "qrc:/images/music"

        checkable: true
        x: playButton.x - 3.5 * 95

        enabled: musicPlayer.started

        disableAutoMgr: true
        Component.onCompleted: {
            if(musicPlayer.isPlayerVisible()) {
                currentState = 0
                updateCheckedState()
                playerVisible = true
            } else {
                currentState = 1
                updateCheckedState()
                playerVisible = false
            }
        }

        onClicked: {
            if(playerVisible) {
                currentState = 1
                checked = true
                playerVisible = false
            } else {
                currentState = 0
                checked = false
                playerVisible = true
            }
        }

        Connections {
            target: musicPlayer
            onNewMediaListPlayed: {
                if(musicPlayer.started) {
                    chooseButton.clicked()
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
                chooseButton.clicked();
            }
        }
    }

    // Used to add playlists
    PasswordPrompt {
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
