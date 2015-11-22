import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import "."

DarkButton {
    inMainMenu: true
    style: DarkButtonStyle {
        label: Item {
            implicitWidth: col.implicitWidth
            implicitHeight: col.implicitHeight

            Column {
                id: col
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 5

                Image {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    asynchronous: true
                    source: control.iconSource
                    height: control.height * .7
                    width: Math.min(sourceSize.width, height)
                    fillMode: Image.PreserveAspectFit
                    horizontalAlignment: Image.AlignHCenter
                }

                StyledText {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: control.text
                    font.pixelSize: control.height * .13
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
