import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import "."

CheckBox {
    style: CheckBoxStyle {
        indicator: Rectangle {
            border.width: 2
            border.color: control.checked ? Style.button.checkedOverlayColor : Style.progressBar.borderColor
            radius: 5
            color: "#F2111111"
            implicitWidth: 40
            implicitHeight: 40

            StyledText {
                visible: control.checked
                font.family: awesomeFont.name
                anchors.margins: 2
                anchors.fill: parent
                fontSizeMode: Text.Fit
                font.pixelSize: 100
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: "\uf00c"
            }
        }

        background: Rectangle {
            color: "transparent"
            height: control.height
            width: control.width
        }

        label: StyledText {
            text: control.text
            font.pixelSize: control.height * .6
            //font.bold: true
        }

        spacing: 15
    }
}
