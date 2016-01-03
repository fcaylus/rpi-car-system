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
import "./settings"

Activity {
    id: activitySettings

    control: GridView {
        width: controlBounds.width
        height: controlBounds.height

        cellWidth: Style.mainMenu.buttonWidth + Style.mainMenu.marginH
        cellHeight: Style.mainMenu.buttonHeight + Style.mainMenu.marginV

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: Style.mainMenu.marginH
        anchors.topMargin: Style.mainMenu.marginV

        property Component settingsLanguage: SettingsLanguage {}
        property Component settingsAbout: SettingsAbout {}
        property Component settingsUpdate: SettingsUpdate {}

        property var settingsMap: {
            3: settingsUpdate,
            4: settingsLanguage,
            6: settingsAbout
        }

        model: ListModel {
            ListElement {
                index: 1
                isEnabled: false
                title: qsTr("Equalizer")
                icon: "qrc:/images/equalizer"
            }
            ListElement {
                index: 2
                isEnabled: false
                title: qsTr("Remove files")
                icon: "qrc:/images/folder"
            }
            ListElement {
                index: 3
                isEnabled: true
                title: qsTr("Update")
                icon: "qrc:/images/update"
            }
            ListElement {
                index: 4
                isEnabled: true
                title: qsTr("Languages")
                icon: "qrc:/images/world"
            }
            ListElement {
                index: 5
                isEnabled: false
                title: qsTr("System")
                icon: "qrc:/images/build"
            }
            ListElement {
                index: 6
                isEnabled: true
                title: qsTr("About")
                icon: "qrc:/images/info"
            }
        }

        delegate: MainMenuButton {
            width: Style.mainMenu.buttonWidth
            height: Style.mainMenu.buttonHeight

            enabled: isEnabled
            text: title
            iconSource: icon

            onClicked: {
                if(isEnabled && stackView.depth == 2) {
                    stackView.push({item: settingsMap[index]});
                    stackView.currentItem.forceActiveFocus();
                }
            }
        }
    }
}
