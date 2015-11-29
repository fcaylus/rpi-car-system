import QtQuick 2.3
import QtQuick.Controls 1.2
import "."

Activity {
    id: activityTransfer

    control: Rectangle {
        width: controlBounds.width
        height: controlBounds.height
        color: "transparent"

        ListView {
            x: 30
            y: 15
            width: parent.width - 60
            height: parent.height - 30
            spacing: 10

            clip: true

            header: Item {
                id: header
                width: parent.width
                height: headerRow.implicitHeight + 10
                //clip: true

                Column {
                    id: headerRow
                    spacing: 5

                    StyledText {
                        text: qsTr("USB sticks list :")
                        font.pixelSize: 22
                        font.bold: true
                        font.italic: true
                    }

                    Rectangle {
                        height: 2
                        width: header.width
                        color: Style.separatorColor
                    }
                }
            }
        }
    }
}
