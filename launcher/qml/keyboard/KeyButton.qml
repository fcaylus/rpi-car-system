import QtQuick 2.3
import ".."

Item {
    id:root
    property color color: Style.button.gradientEnd
    property alias text: txt.text
    property alias font: txt.font
    property alias textColor: txt.color
    signal clicked()
    signal pressed()
    signal released()

    Rectangle {
        anchors.fill: parent
        radius: 5
        color: buttonMouseArea.pressed ? Qt.darker(root.color, 1.5) : root.color
        StyledText {
            id: txt
            anchors.margins: 2
            anchors.fill: parent
            fontSizeMode: Text.Fit
            font.pixelSize: 100
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.bold: true
        }
        MouseArea {
            id: buttonMouseArea
            anchors.fill: parent
            onClicked: root.clicked()
            onPressed: root.pressed()
            onReleased: root.released()
        }
    }
}
