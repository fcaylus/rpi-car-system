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

    FontLoader {
        id: awesomeFont
        source: "qrc:/fonts/FontAwesome"
    }

    gradient: Gradient {
        GradientStop {
            color: Style.backgroundColorEnd
            position: 0
        }
        GradientStop {
            color: Style.backgroundColorStart
            position: .5
        }
        GradientStop {
            color: Style.backgroundColorEnd
            position: 1
        }
    }

    Loader {
        id: startupLoader
        anchors.fill: parent

        // Used by password prompts
        property string firstPass

        function loadStackView() {
            source = "qrc:/qml/MainStackView.qml"
        }

        Component {
            id: askPassword
            PasswordPrompt {
                //visible: !isPassFileCreated
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
                            startupLoader.visible = false
                            startupLoader.sourceComponent = undefined
                            startupLoader.loadStackView()
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
            console.log("pass !")
            passPromptsLoader.sourceComponent = askPassword
        } else {
            console.log("Stackview !")
            startupLoader.source = "qrc:/qml/MainStackView.qml"
        }
    }
}


