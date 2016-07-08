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
import "./keyboard"

Rectangle {
    id: rootWindow
    width: Style.windowWidth
    height: Style.windowHeight

    color: Style.backgroundColor

    Loader {
        id: startupLoader
        anchors.fill: parent

        visible: false
        asynchronous: true
        active: false

        sourceComponent: MainStackView {
        }
    }

    PasswordPrompt {
        property string firstText: qsTr("It's your first radio boot !\nPlease enter a new password :")
        property string confirmText: qsTr("Please confirm :")
        property bool confirm: false

        property string firstPass

        id: askPwd
        titleText: confirm ? confirmText : firstText
        onPromptFinish: {
            if(!confirm) {
                if(text.length >= 1) {
                    firstPass = text
                    setText("")
                    showBackButton = true
                    confirm = true

                    if(!startupLoader.active) {
                        startupLoader.active = true
                    }
                }
            } else {
                if(text === firstPass) {
                    if(passwordManager.createPasswordFile(text)) {
                        visible = false
                        startupLoader.visible = true
                    }
                }
            }
        }

        onBackClicked: {
            showBackButton = false
            confirm = false
            setText(firstPass)
        }
    }
}


