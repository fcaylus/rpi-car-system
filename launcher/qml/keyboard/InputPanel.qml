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
import rpicarsystem.keyboard 1.0
import "."
import ".."

Rectangle {
    id: keyboard
    color: Style.keyboardPanelBackground

    width: parent.width
    height: parent.height * .4
    anchors.bottom: parent.bottom

    signal keyPressed(string text)
    signal backPressed()
    signal enterPressed()

    KeyboardLayoutModel {
        id: keyModel
    }

    //
    // Properties
    property bool shiftModifier: false
    property bool symbolModifier: false
    property int spacing: 5
    property int rowHeight: keyboard.height / 4 - spacing
    // 10 buttons per line
    property int buttonWidth: (keyboard.width - spacing) / 10 - spacing

    // Component delegate used in Repeaters
    Component {
        id: keyButtonDelegate

        KeyButton {
            width: buttonWidth
            height: rowHeight
            text: shiftModifier ? letter.toUpperCase() : (symbolModifier ? symbol : letter)
            onClicked: keyboard.keyPressed(text)
        }
    }

    Column {
        id: column
        anchors.margins: keyboard.spacing
        anchors.fill: parent
        spacing: keyboard.spacing

        // First row
        Row {
            height: rowHeight
            spacing: keyboard.spacing
            anchors.horizontalCenter: parent.horizontalCenter

            Repeater {
                model: keyModel.firstRowModel
                delegate: keyButtonDelegate
            }
        }

        // Second row
        Row {
            height: rowHeight
            spacing: keyboard.spacing
            anchors.horizontalCenter: parent.horizontalCenter

            Repeater {
                model: keyModel.secondRowModel
                delegate: keyButtonDelegate
            }
        }

        // Third row
        Item {
            height: rowHeight
            width: parent.width

            // Switch upper/lower case
            KeyIconButton {
                anchors.left: parent.left
                width: 1.25 * buttonWidth
                height: rowHeight

                color: shiftModifier ? Style.button.checkedOverlayColor: Style.fontColor

                iconSource: "qrc:/images/up"

                onClicked: {
                    // Desactivate symbol on when uppercases letters are activated
                    if(symbolModifier) {
                        symbolModifier = false
                    }
                    shiftModifier = !shiftModifier
                }
            }

            Row {
                spacing: keyboard.spacing
                anchors.horizontalCenter: parent.horizontalCenter

                Repeater {
                    anchors.horizontalCenter: parent.horizontalCenter
                    model: keyModel.thirdRowModel
                    delegate: keyButtonDelegate
                }
            }

            // Backspace
            KeyIconButton {
                anchors.right: parent.right
                width: 1.25 * buttonWidth
                height: rowHeight

                iconSource: "qrc:/images/backspace"

                onClicked: keyboard.backPressed()
            }
        }

        // Fourth row (filled manually since there is no letters)
        Row {
            height: rowHeight
            spacing: keyboard.spacing
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle {
                width: 1.25 * buttonWidth
                height: rowHeight
                radius: 5
                color: Style.button.color
            }

            Rectangle {
                width: 1.25 * buttonWidth
                height: rowHeight
                radius: 5
                color: Style.button.color
            }

            KeyButton {
                width: buttonWidth
                height: rowHeight
                text: ","
                onClicked: keyboard.keyPressed(text)
            }

            // Space
            KeyButton {
                width: 3 * buttonWidth
                height: rowHeight
                text: " "
                onClicked: keyboard.keyPressed(text)
            }

            KeyButton {
                width: buttonWidth
                height: rowHeight
                text: "."
                onClicked: keyboard.keyPressed(text)
            }

            // Swicth alpha/numbers (symbol modifier)
            KeyButton {
                width: 1.25 * buttonWidth
                height: rowHeight
                text: !symbolModifier ? "12#" : "ABC"
                onClicked: {
                    // Disable shift modifier on clicked
                    if(shiftModifier) {
                        shiftModifier = false
                    }
                    symbolModifier = !symbolModifier
                }
            }

            // Enter
            KeyButton {
                width: 1.25 * buttonWidth
                height: rowHeight
                text: qsTr("Enter")
                onClicked: keyboard.enterPressed()
            }
        }
    }
}
