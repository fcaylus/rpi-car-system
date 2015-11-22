import QtQuick 2.5
import QtQuick.Controls 1.2
import ".."
import "."

ListView {
    spacing: 10
    width: 500

    //headerPositioning: ListView.OverlayHeader
    clip: true

    header: Item {
        width: headerRow.implicitWidth
        height: headerRow.implicitHeight + 10
        //clip: true

        Column {
            id: headerRow
            spacing: 5

            StyledText {
                text: headerText
                font.pixelSize: 22
                font.bold: true
                font.italic: true
            }

            Rectangle {
                height: 2
                width: 500
                color: Style.separatorColor
            }
        }
    }
}
