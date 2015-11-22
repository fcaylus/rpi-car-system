import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import "."

Rectangle {
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    height: Style.toolbar.height
    width: Style.toolbar.width

    gradient: Gradient {
        GradientStop {
            color: Style.toolbar.gradientStart
            position: 0
        }
        GradientStop {
            color: Style.toolbar.gradientEnd
            position: 1
        }
    }

    // Top Border
    Rectangle {
        height: 1
        width: parent.width
        anchors.top: parent.top
        color: Style.toolbar.topBorderColor
    }

    // Bottom border
    Rectangle {
        height: 1
        width: parent.width
        anchors.bottom: parent.bottom
        color: Style.toolbar.bottomBorderColor
    }

    // Home button
    // Back to the main menu on click
    DarkButton {
        id: back
        inToolbar: true
        width: parent.height
        height: parent.height
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        iconSource: "qrc:/images/home"
        onClicked: stackView.pop()
    }
}
