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
        XmlRole { name: "cover"; query: "@cover/string()" }
    }

    delegate: Item {
        width: albumRow.implicitWidth
        height: albumRow.implicitHeight
        //clip: true

        Row {
            id: albumRow
            spacing: 20
            Image {
                asynchronous: true
                width: 65
                height: width
                sourceSize.width: width
                sourceSize.height: height
                source: cover
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
            anchors.fill: albumRow
            onClicked: {
                loader.appendLastEntry()
                // show the selected album
                loader.headerText = qsTr("Album: ") + name
                loader.sourceQuery = loader.sourceQuery + "[@name='" + name + "']/track"
                loader.source = "qrc:/qml/music/ListViewTrack.qml"
            }
        }
    }
}
