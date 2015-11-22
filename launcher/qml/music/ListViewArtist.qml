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

        XmlRole { name: "name"; query: "@name/string()" }
    }

    delegate: Item {
        width: artistRow.implicitWidth
        height: artistRow.implicitHeight
        //clip: true

        Row {
            id: artistRow
            spacing: 20
            Image {
                asynchronous: true
                width: 65
                height: width
                sourceSize.width: width
                sourceSize.height: height
                source: "qrc:/images/person"
            }

            StyledText {
                text: name
                font.pixelSize: 22
                //font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            anchors.fill: artistRow
            onClicked: {
                loader.appendLastEntry()
                // show the selected artist
                loader.headerText = qsTr("Artiste: ") + name
                loader.sourceQuery = loader.sourceQuery + "[@name='" + name + "']/album"
                loader.source = "qrc:/qml/music/ListViewAlbum.qml"
            }
        }
    }
}
