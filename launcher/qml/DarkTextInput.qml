import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import "."

TextField {
    style: TextFieldStyle {
        textColor: Style.fontColor
        background: Rectangle {
            border.width: 2
            border.color: Style.button.checkedOverlayColor
            radius: 4
            color: "#F2111111"
        }
    }

    font.pixelSize: height * .6
}
