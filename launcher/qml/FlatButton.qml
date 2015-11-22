import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import "."

Button {
    style: ButtonStyle {
        background: Rectangle {
            color: "transparent"
        }

        label: Item {
            implicitWidth: buttonImg.implicitWidth
            implicitHeight: buttonImg.implicitHeight

            Image {
                id: buttonImg
                asynchronous: true
                source: control.iconSource
                height: control.height
                width: Math.min(sourceSize.width, height)
                fillMode: Image.PreserveAspectFit
            }

            ColorOverlay {
                anchors.fill: buttonImg
                source: buttonImg
                color: Style.button.clickedOverlayColor
                visible: control.pressed
            }
        }
    }

}
