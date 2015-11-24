import QtQuick 2.0
import "."
import "./keyboard"

Rectangle {
    x:0
    y:0
    width: Style.windowWidth
    height: Style.windowHeight

    property string titleText
    property bool showBackButton: false

    signal promptFinish(string text)
    signal backClicked()

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
            echoMode: TextInput.PasswordEchoOnEdit

            function appendText(newText) {
                text = text + newText
            }
            function doBack() {
                if (text.length >= 1) {
                    text = text.substring(0, text.length - 1)
                }
            }
        }

        DarkCheckBox {
            text: qsTr("Afficher le mot de passe")
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
}
