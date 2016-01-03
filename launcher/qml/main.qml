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
    visible: true
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

    property Component activityMusic: ActivityMusic {}
    property Component activityTransfer: ActivityTransfer {}
    property Component activitySettings: ActivitySettings {}

    property var activityMap: {
        1: activityMusic,
        7: activityTransfer,
        8: activitySettings
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

    StackView {
        id: stackView
        width: rootWindow.width
        height: rootWindow.height
        anchors.fill: parent
        anchors.left: parent.left

        visible: isPassFileCreated

        initialItem: GridView {
            width: rootWindow.width - Style.mainMenu.marginH
            height: rootWindow.height - Style.mainMenu.marginV
            cellWidth: Style.mainMenu.buttonWidth + Style.mainMenu.marginH
            cellHeight: Style.mainMenu.buttonHeight + Style.mainMenu.marginV

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: Style.mainMenu.marginH
            anchors.topMargin: Style.mainMenu.marginV

            model: ListModel {
                ListElement {
                    index: 1
                    isEnabled: true
                    title: qsTr("My Music")
                    icon: "qrc:/images/music"
                }
                ListElement {
                    index: 2
                    isEnabled: false
                    title: qsTr("Radio")
                    icon: "qrc:/images/radio"
                }
                ListElement {
                    index: 3
                    isEnabled: false
                    title: qsTr("My Videos")
                    icon: "qrc:/images/video"
                }
                ListElement {
                    index: 4
                    isEnabled: false
                    title: qsTr("Informations")
                    icon: "qrc:/images/car"
                }
                ListElement {
                    index: 5
                    isEnabled: false
                    title: qsTr("GPS")
                    icon: "qrc:/images/map"
                }
                ListElement {
                    index: 6
                    isEnabled: false
                    title: qsTr("Call")
                    icon: "qrc:/images/phone"
                }
                ListElement {
                    index: 7
                    isEnabled: true
                    title: qsTr("Transfer")
                    icon: "qrc:/images/upload"
                }
                ListElement {
                    index: 8
                    isEnabled: true
                    title: qsTr("Settings")
                    icon: "qrc:/images/settings"
                }
            }

            delegate: MainMenuButton {
                width: Style.mainMenu.buttonWidth
                height: Style.mainMenu.buttonHeight

                enabled: isEnabled
                text: title
                iconSource: icon

                onClicked: {
                    if(isEnabled && stackView.depth == 1) {
                        stackView.push({item: activityMap[index]});
                        stackView.currentItem.forceActiveFocus();
                    }
                }
            }
        }
    }

    // This rectangle is only shown when password file is not created
    PasswordPrompt {
        id: askPassword
        visible: !isPassFileCreated
        titleText: qsTr("It's your first radio boot !\nPlease enter a new password :")
        onPromptFinish: {
            if(text.length >= 1) {
                visible = false
                askPasswordConfirm.previousPass = text
                askPasswordConfirm.visible = true
            }
        }
    }

    // Confirmation prompt
    PasswordPrompt {
        id: askPasswordConfirm
        visible: false
        titleText: qsTr("Please confirm :")
        showBackButton: true

        property string previousPass

        onPromptFinish: {
            if(text === previousPass) {
                if(passwordManager.createPasswordFile(text)) {
                    visible = false
                    stackView.visible = true
                }
            }
        }

        onBackClicked: {
            visible = false
            askPassword.visible = true
        }
    }
}


