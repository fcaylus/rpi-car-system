pragma Singleton
import QtQuick 2.3

QtObject {

    readonly property int windowWidth: 800
    readonly property int windowHeight: 480

    property QtObject toolbar: QtObject {
        readonly property int height: windowHeight * .23
        readonly property int width: windowWidth

        readonly property color gradientStart: "#333"
        readonly property color gradientStartPressed: "#222"
        readonly property color gradientEnd: "#111"

        readonly property color topBorderColor: "#444"
        readonly property color bottomBorderColor: "#000"
    }

    property QtObject button: QtObject {
        readonly property color gradientStart: "#444"
        readonly property color gradientStartPressed: "#333"
        readonly property color gradientEnd: "#222"

        readonly property color topBorderColor: "#555"
        readonly property color bottomBorderColor: "#111"

        readonly property color clickedOverlayColor: "#1A6680"
        readonly property color checkedOverlayColor: "#33CCFF"
    }

    property QtObject progressBar: QtObject {
        readonly property color backgroundColor: "#707070"
        readonly property color borderColor: "#303030"
        readonly property color progressColor: "#33CCFF"
    }

    readonly property color fontColor: "#e7e7e7"

    readonly property color separatorColor: "#444"

    readonly property color backgroundColorStart: "#252525"
    readonly property color backgroundColorEnd: "#090909"

    readonly property color popupBackground: "#BB101010"

    property QtObject mainMenu: QtObject {
        readonly property int marginH: 20
        readonly property int marginV: 20
        readonly property int buttonWidth: (windowWidth - marginH * 4) / 3
        readonly property int buttonHeight: (windowHeight - marginV * 4) / 3
    }
}
