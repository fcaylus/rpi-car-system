/*
 * This file is part of RPI Car System.
 * Copyright (c) 2015 Fabien Caylus <toutjuste13@gmail.com>
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

pragma Singleton
import QtQuick 2.3

QtObject {

    readonly property int windowWidth: 800
    readonly property int windowHeight: 480

    property QtObject toolbar: QtObject {
        readonly property int height: windowHeight * .23
        readonly property int width: windowWidth

        readonly property color gradientStart: "#333"
        readonly property color gradientStartPressed: "#222"
        readonly property color gradientEnd: "#111"

        readonly property color topBorderColor: "#444"
        readonly property color bottomBorderColor: "#000"
    }

    property QtObject button: QtObject {
        readonly property color gradientStart: "#444"
        readonly property color gradientStartPressed: "#333"
        readonly property color gradientEnd: "#222"

        readonly property color topBorderColor: "#555"
        readonly property color bottomBorderColor: "#111"

        readonly property color clickedOverlayColor: "#1A6680"
        readonly property color checkedOverlayColor: "#33CCFF"
    }

    property QtObject progressBar: QtObject {
        readonly property color backgroundColor: "#707070"
        readonly property color borderColor: "#303030"
        readonly property color progressColor: "#33CCFF"
    }

    readonly property color fontColor: "#e7e7e7"

    readonly property color separatorColor: "#444"

    readonly property color backgroundColorStart: "#252525"
    readonly property color backgroundColorEnd: "#090909"

    readonly property color popupBackground: "#BB101010"

    readonly property color dangerousColor: "#8B0000"

    property QtObject mainMenu: QtObject {
        readonly property int marginH: 20
        readonly property int marginV: 20
        readonly property int buttonWidth: (windowWidth - marginH * 4) / 3
        readonly property int buttonHeight: (windowHeight - marginV * 4) / 3
    }
}
