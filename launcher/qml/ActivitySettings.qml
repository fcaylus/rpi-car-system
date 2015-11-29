import QtQuick 2.3
import QtQuick.Controls 1.2
import "."

Activity {
    id: activitySettings

    control: GridView {
        width: controlBounds.width
        height: controlBounds.height

        cellWidth: Style.mainMenu.buttonWidth + Style.mainMenu.marginH
        cellHeight: Style.mainMenu.buttonHeight + Style.mainMenu.marginV

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: Style.mainMenu.marginH
        anchors.topMargin: Style.mainMenu.marginV

        property var settingsMap: {
        }

        model: ListModel {
            ListElement {
                index: 1
                isEnabled: false
                title: qsTr("Equalizer")
                icon: "qrc:/images/equalizer"
            }
            ListElement {
                index: 2
                isEnabled: false
                title: qsTr("Remove files")
                icon: "qrc:/images/folder"
            }
            ListElement {
                index: 3
                isEnabled: false
                title: qsTr("Update")
                icon: "qrc:/images/update"
            }
            ListElement {
                index: 4
                isEnabled: false
                title: qsTr("Languages")
                icon: "qrc:/images/world"
            }
            ListElement {
                index: 5
                isEnabled: false
                title: qsTr("System")
                icon: "qrc:/images/build"
            }
            ListElement {
                index: 6
                isEnabled: false
                title: qsTr("About")
                icon: "qrc:/images/info"
            }
        }

        delegate: MainMenuButton {
            width: Style.mainMenu.buttonWidth
            height: Style.mainMenu.buttonHeight

            enabled: isEnabled
            text: title
            iconSource: icon

            onClicked: {
                if(isEnabled && stackView.depth == 1) {
                    stackView.push({item: settingsMap[index]});
                    stackView.currentItem.forceActiveFocus();
                }
            }
        }
    }
}
