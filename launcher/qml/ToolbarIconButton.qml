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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import "."

Button {
    property bool hasSecondIcon: false
    property url secondIcon
    property int statesNumber: 1 + (checkable ?  1 : 0) + (hasSecondIcon ? 1 : 0)
    property int currentState: 0

    property real iconScale: .9

    // Disable auto-management of currentState var
    property bool disableAutoMgr: false

    function updateCheckedState() {
        checked = checkable && currentState >= 1
    }

    height: Style.toolbar.height
    width: Style.toolbar.height * iconScale

    onClicked: {
        // Increase current state
        if(!disableAutoMgr)
            currentState = (currentState + 1) % statesNumber
        updateCheckedState()
    }

    style: ButtonStyle {
        background: Rectangle {
            color: control.pressed ? Style.button.colorPressed : Style.button.color
        }

        label: Image {
            asynchronous: true
            height: control.height
            width: control.width
            sourceSize.width: width
            sourceSize.height: height

            fillMode: Image.PreserveAspectFit

            source: (hasSecondIcon && currentState == (statesNumber-1)) ? secondIcon : control.iconSource

            ColorOverlay {
                anchors.fill: parent
                source: parent
                color: control.pressed ? Style.button.clickedOverlayColor
                                            : Style.button.checkedOverlayColor
                visible: control.pressed || control.checked
            }
        }
    }
}
