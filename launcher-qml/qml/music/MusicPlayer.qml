import QtQuick 2.3
import QtQuick.Controls 1.2
import ".."
import "."

Rectangle {
    id: musicPlayer
    color: "transparent"

    property int coverWidth: 260
    property int rightWidth: Style.windowWidth - coverWidth - 30*3

    Column {
        spacing: 20
        x: 30

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Row {
            spacing: 30
            anchors.horizontalCenter: parent.horizontalCenter

            // Cover image
            Image {
                asynchronous: true
                source: soundManager.mediaCover
                height: coverWidth
                width: height
            }

            Column {
                spacing: 6
                height: coverWidth

                Rectangle {
                    height: 1
                    width: rightWidth
                    color: Style.separatorColor
                }

                //
                // Title row
                //

                StyledText {
                    width: rightWidth
                    text: soundManager.mediaTitle
                    font.bold: true
                    font.pixelSize: 30
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                StyledText {
                    width: rightWidth
                    text: soundManager.mediaArtist
                    font.italic: true
                    font.pixelSize: 20
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                Rectangle {
                    height: 1
                    width: rightWidth
                    color: Style.separatorColor
                }
                Rectangle {
                    height: 1
                    width: rightWidth
                    color: Style.separatorColor
                }

                //
                // Volume row
                //

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 20

                    FlatButton {
                        width: 40
                        height: width
                        iconSource: "qrc:/images/minus"
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: {
                            volumeBar.value = volumeBar.value - .05
                        }
                    }

                    DarkProgressBar {
                        id: volumeBar
                        width: 150
                        anchors.verticalCenter: parent.verticalCenter
                        onValueChanged: soundManager.setVolume(value*100)
                        Component.onCompleted: {
                            value = soundManager.volume / 100
                        }
                    }

                    FlatButton {
                        width: 40
                        height: width
                        iconSource: "qrc:/images/plus"
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: {
                            volumeBar.value = volumeBar.value + .05
                        }
                    }
                }

                Rectangle {
                    height: 1
                    width: rightWidth
                    color: Style.separatorColor
                }

                //
                // Queue row
                //

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 40

                    FlatButton {
                        width: 50
                        height: width
                        iconSource: "qrc:/images/fast_rewind"
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: soundManager.rewind()
                    }

                    FlatButton {
                        id: queueButton
                        width: 70
                        height: width
                        iconSource: "qrc:/images/playlist"
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: {
                            musicQueuePopup.visible = true
                        }

                    }

                    FlatButton {
                        width: 50
                        height: width
                        iconSource: "qrc:/images/fast_forward"
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: soundManager.forward()
                    }
                }
            }
        }

        // Timeline
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 18

            DarkProgressBar {
                id: timeBar
                width: Style.windowWidth - 160
                anchors.verticalCenter: parent.verticalCenter
                value: 0
            }

            StyledText {
                anchors.verticalCenter: parent.verticalCenter
                text: soundManager.formatedTime
                font.italic: true
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Connections {
                target: soundManager
                onTimeChanged: timeBar.value = soundManager.time / 100
                onEndReached: timeBar.value = 1
            }
        }
    }

    MusicQueuePopup {
        id: musicQueuePopup
        visible: false
    }

}

