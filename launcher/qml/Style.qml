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

pragma Singleton
import QtQuick 2.5

QtObject {

    readonly property int windowWidth: 800
    readonly property int windowHeight: 480

    property QtObject toolbar: QtObject {
        readonly property int height: windowHeight * .23
        readonly property int width: windowWidth

        readonly property color color: "#292929"
    }

    property QtObject button: QtObject {
        readonly property color color: "#292929"
        readonly property color colorPressed: "#202020"

        readonly property color borderColor: "#1a1a1a"

        readonly property color clickedOverlayColor: "#1A6680"
        readonly property color checkedOverlayColor: "#33CCFF"
    }

    property QtObject progressBar: QtObject {
        readonly property color backgroundColor: "#707070"
        readonly property color borderColor: "#303030"
        readonly property color progressColor: "#33CCFF"
        readonly property color selectedColor: "#2AE8C8"
    }

    readonly property color fontColor: "#e7e7e7"
    readonly property color fontColorDisabled: "#606060"

    readonly property color separatorColor: "#444"

    readonly property color backgroundColor: "#1B1B1B"
    readonly property color popupBackground: "#BB101010"
    readonly property color popupBorder: "#555"

    readonly property color dangerousColor: "#8B0000"
    readonly property color safeColor: "#29991D"

    readonly property color keyboardPanelBackground: "#111"

    property QtObject mainMenu: QtObject {
        readonly property int margin: 20
        readonly property int buttonWidth: (windowWidth - margin * 4) / 3
        readonly property int buttonHeight: (windowHeight - margin * 4) / 3
    }
}
