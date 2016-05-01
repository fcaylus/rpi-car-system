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

GridView {
    id: gridMenu
    anchors.fill: parent
    anchors.leftMargin: Style.mainMenu.margin
    anchors.topMargin: Style.mainMenu.margin

    cellWidth: Style.mainMenu.buttonWidth + Style.mainMenu.margin
    cellHeight: Style.mainMenu.buttonHeight + Style.mainMenu.margin

    property int expectedDepth: 1
    // Must be filled
    property var map

    // This is an example of model
    /*
    model: ListModel {
        ListElement {
            index: 1
            isEnabled: true
            title: "Title"
            icon: "qrc:/images/path"
        }
    }*/

    delegate: MainMenuButton {
        width: Style.mainMenu.buttonWidth
        height: Style.mainMenu.buttonHeight

        enabled: isEnabled
        text: title
        iconSource: icon

        onClicked: {
            if(isEnabled && mainStackView.depth === gridMenu.expectedDepth) {
                mainStackView.push({item: map[index]});
                mainStackView.currentItem.forceActiveFocus();
            }
        }
    }
}
