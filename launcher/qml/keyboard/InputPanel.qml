import QtQuick 2.3
import QtQuick.Window 2.0
import "."
import ".."

Item {
    id: root

    signal keyPressed(string text)
    signal backPressed()
    signal enterPressed()

    KeyModel {
        id: keyModel
    }

    QtObject {
        id: pimpl
        property bool shiftModifier: false
        property bool symbolModifier: false
        property int verticalSpacing: 5
        property int horizontalSpacing: 5
        property int rowHeight: keyboard.height/4 - verticalSpacing
        property int buttonWidth:  (keyboard.width-column.anchors.margins)/10 - horizontalSpacing
    }

    Component {
        id: keyButtonDelegate
        KeyButton {
            width: pimpl.buttonWidth
            height: pimpl.rowHeight
            text: (pimpl.shiftModifier) ? letter.toUpperCase() : (pimpl.symbolModifier)? firstSymbol : letter
            onClicked: root.keyPressed(text)
        }
    }

    Rectangle {
        id: keyboard
        color: Style.button.bottomBorderColor
        width: parent.width
        height: parent.height * .4
        anchors.bottom: parent.bottom
        MouseArea {
            anchors.fill: parent
        }

        Column {
            id: column
            anchors.margins: 5
            anchors.fill: parent
            spacing: pimpl.verticalSpacing

            Row {
                height: pimpl.rowHeight
                spacing: pimpl.horizontalSpacing
                anchors.horizontalCenter: parent.horizontalCenter
                Repeater {
                    model: keyModel.firstRowModel
                    delegate: keyButtonDelegate
                }
            }
            Row {
                height: pimpl.rowHeight
                spacing: pimpl.horizontalSpacing
                anchors.horizontalCenter: parent.horizontalCenter
                Repeater {
                    model: keyModel.secondRowModel
                    delegate: keyButtonDelegate
                }
            }
            Item {
                height: pimpl.rowHeight
                width: parent.width

                // Switch upper/lower case
                KeyButton {
                    anchors.left: parent.left
                    width: 1.25*pimpl.buttonWidth
                    height: pimpl.rowHeight
                    font.family: awesomeFont.name
                    textColor: (pimpl.shiftModifier)? Style.button.checkedOverlayColor: Style.fontColor
                    text: "\uf0aa"
                    onClicked: {
                        if (pimpl.symbolModifier) {
                            pimpl.symbolModifier = false
                        }
                        pimpl.shiftModifier = !pimpl.shiftModifier
                    }
                }
                Row {
                    height: pimpl.rowHeight
                    spacing: pimpl.horizontalSpacing
                    anchors.horizontalCenter: parent.horizontalCenter
                    Repeater {
                        anchors.horizontalCenter: parent.horizontalCenter
                        model: keyModel.thirdRowModel
                        delegate: keyButtonDelegate
                    }
                }

                // Backspace
                KeyButton {
                    anchors.right: parent.right
                    width: 1.25 * pimpl.buttonWidth
                    height: pimpl.rowHeight
                    font.family: awesomeFont.name
                    text: "\uf177"
                    onClicked: root.backPressed()

                }
            }
            Row {
                height: pimpl.rowHeight
                spacing: pimpl.horizontalSpacing
                anchors.horizontalCenter: parent.horizontalCenter
                KeyButton {
                    width: 1.25 * pimpl.buttonWidth
                    height: pimpl.rowHeight
                    text: ""
                }
                KeyButton {
                    width: 1.25 * pimpl.buttonWidth
                    height: pimpl.rowHeight
                    text: ""
                }

                KeyButton {
                    width: pimpl.buttonWidth
                    height: pimpl.rowHeight
                    text: ","
                    onClicked: root.keyPressed(text)
                }

                KeyButton {
                    width: 3*pimpl.buttonWidth
                    height: pimpl.rowHeight
                    text: " "
                    onClicked: root.keyPressed(text)
                }

                KeyButton {
                    width: pimpl.buttonWidth
                    height: pimpl.rowHeight
                    text: "."
                    onClicked: root.keyPressed(text)
                }

                // Swicth alpha/numbers
                KeyButton {
                    width: 1.25*pimpl.buttonWidth
                    height: pimpl.rowHeight
                    text: (!pimpl.symbolModifier)? "12#" : "ABC"
                    onClicked: {
                        if (pimpl.shiftModifier) {
                            pimpl.shiftModifier = false
                        }
                        pimpl.symbolModifier = !pimpl.symbolModifier
                    }
                }

                // Enter
                KeyButton {
                    width: 1.25*pimpl.buttonWidth
                    height: pimpl.rowHeight
                    text: qsTr("Enter")
                    onClicked: root.enterPressed()
                }
            }
        }
    }
}
