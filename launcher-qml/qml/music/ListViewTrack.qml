import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.XmlListModel 2.0
import ".."
import "."

ListViewBase {
    id: listViewTrack

    model: XmlListModel {
        id: model

        source: sourceFile
        query: sourceQuery

        XmlRole { name: "title"; query: "title/string()" }
        XmlRole { name: "path"; query: "path/string()" }
        XmlRole { name: "cover"; query: "cover/string()" }
    }

    delegate: Item {
        width: trackRow.implicitWidth
        height: trackRow.implicitHeight
        //clip: true
        Row {
            id: trackRow
            spacing: 20
            Image {
                asynchronous: true
                width: 65
                height: width
                source: cover
            }

            StyledText {
                text: title
                font.pixelSize: 22
                //font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            anchors.fill: trackRow
            onClicked :{
                soundManager.playFromFile(path, loader.sourceFile, loader.sourceQuery);
                chooseButton.clicked()
            }
        }
    }
}
