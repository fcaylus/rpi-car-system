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

    Component.onCompleted: {
        if(soundManager.mediaListReady) {
            waitText.visible = false
            mainRow.visible = true
            artistButton.clicked()
            started = true
        }

        // Update history status
        loader.loadPreviousHistory()
    }

    Component.onDestruction: {
        loader.appendLastEntry()
    }

    Connections {
        target: soundManager
        onMediaListReadyChanged: {
            if(soundManager.mediaListReady) {
                if(!mainRow.visible) {
                    waitText.visible = false
                    mainRow.visible = true
                    artistButton.clicked()
                    started = true
                } else {
                    // ListView is already visible
                    var src = loader.source
                    loader.source = ""
                    loader.source = src
                }
            }
        }
    }

    StyledText {
        id: waitText
        visible: true
        width: parent.width
        height: parent.height
        text: qsTr("Search for music files ...");
        font.pixelSize: 25
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Row {
        id: mainRow
        visible: false
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
                text: qsTr("Artists")

                onClicked: {
                    // Small trick to avoid addition of the first entry
                    // Started is set to true only after the first add
                    if(started) {
                        loader.appendLastEntry()
                    }

                    loader.headerText = qsTr("Artists list ...");
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
                    loader.headerText = qsTr("Albums list ...")
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
                text: qsTr("Tracks")

                onClicked: {
                    loader.appendLastEntry()
                    loader.headerText = qsTr("Tracks list ...")
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
                    loader.loadPreviousHistory()
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

            function loadPreviousHistory() {
                if(soundManager.hasHistoryEntry) {
                    headerText = soundManager.lastHistoryEntryHeaderText()
                    sourceFile = soundManager.lastHistoryEntrySourceFile()
                    sourceQuery = soundManager.lastHistoryEntrySourceQuery()
                    source = soundManager.lastHistoryEntrySource()

                    soundManager.removeLastHistoryEntry()
                }
            }
        }
    }
}

