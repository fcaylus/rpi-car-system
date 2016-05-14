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
    objectName: "LauncherWindow"
    width: Style.windowWidth
    height: Style.windowHeight

    FontLoader {
        id: mainFont
        source: "qrc:/fonts/OpenSans-Regular"
    }

    color: Style.backgroundColor

    Loader {
        id: startupLoader
        anchors.fill: parent

        // Used by password prompts
        property string firstPass

        Component {
            id: askPassword
            PasswordPrompt {
                titleText: qsTr("It's your first radio boot !\nPlease enter a new password :")
                onPromptFinish: {
                    if(text.length >= 1) {
                        startupLoader.firstPass = text
                        startupLoader.sourceComponent = askPasswordConfirm
                    }
                }

                Component.onCompleted: setText(firstPass)
            }
        }

        Component {
            id: askPasswordConfirm
            // Confirmation prompt
            PasswordPrompt {
                //visible: false
                titleText: qsTr("Please confirm :")
                showBackButton: true

                onPromptFinish: {
                    if(text === firstPass) {
                        if(passwordManager.createPasswordFile(text)) {
                            startupLoader.source = "qrc:/qml/MainStackView.qml"
                        }
                    }
                }

                onBackClicked: {
                    startupLoader.sourceComponent = askPassword
                }
            }
        }
    }

    Component.onCompleted: {
        if(!passFileCreated) {
            startupLoader.sourceComponent = askPassword
        } else {
            startupLoader.source = "qrc:/qml/MainStackView.qml"
        }
    }
}


