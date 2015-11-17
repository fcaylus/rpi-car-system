import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.XmlListModel 2.0
import ".."
import "."

//
// Each mode had it's own list views
//

Item {
    id: musicLister

    property bool started: false

    Row {
        anchors.fill: parent
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        spacing: 10

        // Used to choose correct mode
        Column {
            width: 200

            DarkButton {
                id: artistButton
                width: parent.width
                height: 80
                alignCenter: true
                bold: true
                text: qsTr("Artistes")

                onClicked: {
                    // Little trick to avoid addition of the first entry
                    // Started is set to true only after the first add
                    if(started) {
                        loader.appendLastEntry()
                    }

                    loader.headerText = qsTr("Liste des artistes ...");
                    loader.sourceFile = soundManager.artistMapFilePath
                    loader.sourceQuery = "/artists/artist"
                    loader.source = "qrc:/qml/music/ListViewArtist.qml"
                }
            }

            DarkButton {
                id: albumButton
                width: parent.width
                height: 80
                alignCenter: true
                bold: true
                text: qsTr("Albums")

                onClicked: {
                    loader.appendLastEntry()
                    loader.headerText = qsTr("Liste des albums ...")
                    loader.sourceFile = soundManager.albumMapFilePath
                    loader.sourceQuery = "/albums/album"
                    loader.source = "qrc:/qml/music/ListViewAlbum.qml"
                }
            }

            DarkButton {
                id: trackButton
                width: parent.width
                height: 80
                alignCenter: true
                bold: true
                text: qsTr("Titres")

                onClicked: {
                    loader.appendLastEntry()
                    loader.headerText = qsTr("Liste des titres ...")
                    loader.sourceFile = soundManager.trackListFilePath
                    loader.sourceQuery = "/tracks/track"
                    loader.source = "qrc:/qml/music/ListViewTrack.qml"
                }
            }

            FlatButton {
                width: 120
                height: width
                anchors.horizontalCenter: parent.horizontalCenter
                iconSource: "qrc:/images/back"
                onClicked: {
                    // Load previous entry
                    loader.headerText = soundManager.lastHistoryEntryHeaderText()
                    loader.sourceFile = soundManager.lastHistoryEntrySourceFile()
                    loader.sourceQuery = soundManager.lastHistoryEntrySourceQuery()
                    loader.source = soundManager.lastHistoryEntrySource()

                    soundManager.removeLastHistoryEntry()
                }
                visible: soundManager.hasHistoryEntry
            }
        }

        Rectangle {
            height: parent.height
            width: 2
            color: Style.separatorColor
        }
        Rectangle {
            height: parent.height
            width: 2
            color: Style.separatorColor
        }
        Rectangle {
            height: parent.height
            width: 2
            color: Style.separatorColor
        }

        Loader {
            id: loader
            focus: true

            height: parent.height - 10

            property string headerText
            property string sourceFile
            property string sourceQuery

            function appendLastEntry() {
                soundManager.addHistoryEntry(source, sourceFile, sourceQuery, headerText)
            }
        }
    }

    Component.onCompleted: {
        artistButton.clicked()
        started = true
    }
}

