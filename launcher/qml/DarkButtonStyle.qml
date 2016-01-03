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
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import "."

ButtonStyle {

    background: Rectangle {
        property bool pressed: control.pressed

        radius: inMainMenu ? 20 : 0

        gradient: Gradient {
            GradientStop {
                color: inToolbar ? (pressed ? Style.toolbar.gradientStartPressed : Style.toolbar.gradientStart)
                                 : (pressed ? Style.button.gradientStartPressed : Style.button.gradientStart)
                position: 0
            }
            GradientStop {
                color: inToolbar ? Style.toolbar.gradientEnd : Style.button.gradientEnd
                position: 1
            }
        }

        Rectangle {
            height: 1
            width: parent.width
            anchors.top: parent.top
            color: inToolbar ? Style.toolbar.topBorderColor : Style.button.topBorderColor
            visible: inMainMenu ? false : !pressed
        }

        Rectangle {
            height: 1
            width: parent.width
            anchors.bottom: parent.bottom
            color: inToolbar ? Style.toolbar.bottomBorderColor : Style.button.bottomBorderColor
            visible: !inMainMenu
        }

        border.color: Style.toolbar.bottomBorderColor
        border.width: inMainMenu ? 1 : 0
    }

    label: Item {
        implicitWidth: col.implicitWidth
        implicitHeight: col.implicitHeight
        baselineOffset: col.y + hText.y + hText.baselineOffset

        Column {
            id: col
            anchors.left: control.text.length === 0 ? undefined : parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            Image {
                asynchronous: true
                source: (hasSecondIcon && currentState == (statesNumber-1)) ? secondIcon : control.iconSource
                height: control.height * iconScale
                width: Math.min(sourceSize.width, height)
                fillMode: Image.PreserveAspectFit
            }

            StyledText {
                id: hText
                text: control.text
                font.pixelSize: control.height * .35
                horizontalAlignment: alignCenter ? Text.AlignHCenter : Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                anchors.horizontalCenter: parent.horizontalCenter
                font.bold: bold
            }
        }
        ColorOverlay {
            anchors.fill: col
            source: col
            color: control.pressed ? Style.button.clickedOverlayColor
                                   : Style.button.checkedOverlayColor
            visible: control.pressed || control.checked
        }
    }
}
