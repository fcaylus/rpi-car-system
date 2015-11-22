import QtQuick 2.3
import QtQuick.Controls 1.2
import "."

Rectangle {
    id: view

    gradient: Gradient {
        GradientStop {
            color: Style.backgroundColorEnd
            position: 0
        }
        GradientStop {
            color: Style.backgroundColorStart
            position: .5
        }
        GradientStop {
            color: Style.backgroundColorEnd
            position: 1
        }
    }

    property Component control

    property alias controlItem: controlLoader.item

    property rect controlBounds: Qt.rect(largestControlItem.x + controlBoundsItem.x,
        largestControlItem.y + controlBoundsItem.y, controlBoundsItem.width, controlBoundsItem.height)

    // Substract the "home" button width (it's the toolbar height)
    property int availableToolBarWidth: toolbar.width - toolbar.height
    property int toolbarHeight: toolbar.height

    Item {
        id: largestControlItem
        x: 0
        y: 0
        width: parent.width
        height: parent.height - toolbar.height

        Item {
            id: controlBoundsItem
            x: parent.width / 2 - controlBoundsItem.width / 2
            y: 0
            width: parent.width
            height: parent.height

            Loader {
                id: controlLoader
                sourceComponent: control
                anchors.centerIn: parent

                property alias view: view
            }
        }
    }

    ActivityToolbar {
        id: toolbar
    }
}
