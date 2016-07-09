import QtQuick 2.5
import QtGraphicalEffects 1.0
import ".."

Rectangle {
    id:root

    property alias text: txt.text
    property color textColor: Style.fontColor

    signal clicked()
    signal pressed()
    signal released()

    property string customIcon
    property bool useCustomIcon: false

    radius: 5
    color: buttonMouseArea.pressed ? Style.button.colorPressed : Style.button.color

    Text {
        id: txt
        anchors.margins: 2
        anchors.fill: parent
        fontSizeMode: Text.Fit
        font.pixelSize: 100
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.bold: true
        color: textColor
    }

    MouseArea {
        id: buttonMouseArea
        anchors.fill: parent
        onClicked: root.clicked()
        onPressed: root.pressed()
        onReleased: root.released()
    }

    Loader {
        id: customIconLoader
        anchors.margins: 2
        anchors.fill: parent

        asynchronous: true
        sourceComponent: useCustomIcon ? customIconComponent : undefined
        visible: useCustomIcon

        Component {
            id: customIconComponent

            Image {
                asynchronous: true
                source: root.customIcon
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                fillMode: Image.PreserveAspectFit

                ColorOverlay {
                    anchors.fill: parent
                    source: parent
                    color: root.textColor
                }
            }
        }
    }

}
