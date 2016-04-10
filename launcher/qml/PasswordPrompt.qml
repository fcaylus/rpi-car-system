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
import "."
import "./keyboard"

Rectangle {
    x:0
    y:0
    width: Style.windowWidth
    height: Style.windowHeight

    property string titleText
    property bool showBackButton: false
    property bool standardInput: false

    signal promptFinish(string text)
    signal backClicked()

    function setText(text) {
        passwordInput.clear()
        passwordInput.appendText(text)
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

    FlatButton {
        visible: showBackButton
        x: 10
        y: 10
        width: 60
        height: 60
        iconSource: "qrc:/images/back"
        onClicked: backClicked()
    }

    Column {
        x: 20
        y: 30
        width: parent.width - 2*x
        spacing: 25

        StyledText {
            width: parent.width
            text: titleText
            font.pixelSize: 27
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        DarkTextInput {
            id: passwordInput
            width: parent.width
            height: 50
            readOnly: true
            echoMode: standardInput ? TextInput.Normal : TextInput.PasswordEchoOnEdit

            function appendText(newText) {
                text = text + newText
            }
            function doBack() {
                if (text.length >= 1) {
                    text = text.substring(0, text.length - 1)
                }
            }
            function clear() {
                text = ""
            }
        }

        DarkCheckBox {
            visible: !standardInput
            text: qsTr("Show password")
            onClicked: passwordInput.echoMode = checked ? TextInput.Normal : TextInput.PasswordEchoOnEdit
        }
    }

    InputPanel {
        id: keyboard
        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height
        onKeyPressed: passwordInput.appendText(text)
        onBackPressed: passwordInput.doBack()
        onEnterPressed: promptFinish(passwordInput.text)
    }

    function resetText() {
        passwordInput.clear()
    }
}
