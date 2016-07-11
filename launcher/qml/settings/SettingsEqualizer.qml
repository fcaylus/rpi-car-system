/*
 * This file is part of RPI Car System.
 * Copyright (c) 2016 Fabien Caylus <tranqyll.dev@gmail.com>
 *
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.5
import ".."
import "."

Activity {
    id: settingsEqualizer
    subActivity: true

    Row {
        id: equalizerModifier
        visible: false

        height: Style.toolbar.height
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        spacing: 20

        property bool preamp: false
        property int freqId: 0
        property string freqLabel

        property string amp

        FlatButton {
            width: 80
            height: width
            iconSource: "qrc:/images/minus"
            anchors.verticalCenter: parent.verticalCenter

            repeatInterval: 30
            autoRepeat: true
            onClicked: {
                if(equalizerModifier.preamp) {
                    musicPlayer.increaseEqualizerPreamp(-0.2)
                } else {
                    musicPlayer.increaseEqualizerAmp(equalizerModifier.freqId, -0.2)
                }
            }
        }

        Column {
            spacing: 10
            width: 80
            anchors.verticalCenter: parent.verticalCenter

            StyledText {
                width: parent.width
                font.pixelSize: 25
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                text: equalizerModifier.freqLabel
            }

            StyledText {
                id: ampLabelModifier
                width: parent.width
                font.pixelSize: 25
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: equalizerModifier.amp
            }
        }

        FlatButton {
            width: 80
            height: width
            iconSource: "qrc:/images/plus"
            anchors.verticalCenter: parent.verticalCenter

            repeatInterval: 30
            autoRepeat: true
            onClicked: {
                if(equalizerModifier.preamp) {
                    musicPlayer.increaseEqualizerPreamp(0.2)
                } else {
                    musicPlayer.increaseEqualizerAmp(equalizerModifier.freqId, 0.2)
                }
            }
        }

        function updateText() {
            if(preamp) {
                amp = musicPlayer.equalizerPreamplificationString()
            } else {
                amp = musicPlayer.equalizerAmplificationString(freqId)
            }
        }

        function show(preamp, freqLabel, freqId) {
            equalizerModifier.preamp = preamp
            equalizerModifier.freqLabel = freqLabel
            equalizerModifier.freqId = freqId
            updateText()

            equalizerModifier.visible = true
        }

        Connections {
            target: musicPlayer
            onEqualizerConfigChanged: {
                equalizerModifier.updateText()
            }
        }
    }

    // Button with the equalizer label
    // Click to show the equalizer list
    ToolbarTextButton {
        id: labelConfig
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: Style.toolbar.height
        anchors.right: parent.right
        anchors.rightMargin: equalizerModifier.width + 10

        fontRatio: .4

        text: musicPlayer.equalizerConfigName()

        Connections {
            target: musicPlayer
            onNewEqualizerConfigLoaded: {
                labelConfig.text = musicPlayer.equalizerConfigName()
            }
        }

        onClicked: listPopup.visible = true
    }

    control: Rectangle {
        color: "transparent"

        Row {
            id: row
            anchors.fill: parent
            anchors.topMargin: 10
            anchors.bottomMargin: 10

            Rectangle {
                id: preampColumn
                height: parent.height
                width: 40

                color: "transparent"

                property bool itemSelected: false

                Column {
                    anchors.fill: parent
                    spacing: 7

                    DarkProgressBar {
                        id: preampSelect
                        orientation: Qt.Vertical
                        maximumValue: 40
                        minimumValue: 0

                        selected: preampColumn.itemSelected

                        height: parent.height - labelPreAmp.height - labelPreAmpVal.height - 2*parent.spacing
                        width: 25

                        anchors.horizontalCenter: parent.horizontalCenter

                        value: musicPlayer.equalizerPreamplification() + 20
                    }

                    StyledText {
                        id: labelPreAmp
                        width: parent.width
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                        text: qsTr("Pre-amp")
                    }

                    StyledText {
                        id: labelPreAmpVal
                        width: parent.width
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter

                        text: musicPlayer.equalizerPreamplificationString()
                    }

                    Connections {
                        target: musicPlayer
                        onEqualizerConfigChanged: {
                            preampSelect.value = musicPlayer.equalizerPreamplification() + 20
                            labelPreAmpVal.text = musicPlayer.equalizerPreamplificationString()
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if(preampColumn.itemSelected) {
                            equalizerModifier.visible = false
                            preampColumn.itemSelected = false
                            return
                        }

                        // Deselect all items
                        var c = repeater.count
                        for (var i=0; i < c; i++) {
                            repeater.itemAt(i).itemSelected = false
                        }

                        preampColumn.itemSelected = true
                        equalizerModifier.show(true, labelPreAmp.text, 0)
                    }
                }
            }

            Rectangle {
                width: 1
                height: parent.height
                color: Style.separatorColor
            }

            Repeater {
                id: repeater
                model: musicPlayer.nbOfFrequenciesAvailable()

                Rectangle {
                    height: parent.height
                    width: 35

                    color: "transparent"

                    property bool itemSelected: false

                    Column {
                        anchors.fill: parent
                        spacing: 7

                        DarkProgressBar {
                            id: ampSelect
                            orientation: Qt.Vertical
                            maximumValue: 40
                            minimumValue: 0

                            selected: itemSelected

                            value: musicPlayer.equalizerAmplification(index) + 20

                            height: parent.height - labelAmp.height - labelAmpVal.height - 2*parent.spacing
                            width: 20
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        StyledText {
                            id: labelAmp
                            width: parent.width
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.bold: true
                            text: musicPlayer.equalizerFrequencyString(index)
                        }

                        StyledText {
                            id: labelAmpVal
                            width: parent.width
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            text: musicPlayer.equalizerAmplificationString(index)
                        }

                        Connections {
                            target: musicPlayer
                            onEqualizerConfigChanged: {
                                ampSelect.value = musicPlayer.equalizerAmplification(index) + 20
                                labelAmpVal.text = musicPlayer.equalizerAmplificationString(index)
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if(itemSelected) {
                                equalizerModifier.visible = false
                                itemSelected = false
                                return
                            }

                            // Deselect all items
                            var c = repeater.count
                            for (var i=0; i < c; i++) {
                                repeater.itemAt(i).itemSelected = false
                            }
                            preampColumn.itemSelected = false

                            itemSelected = true
                            equalizerModifier.show(false, labelAmp.text, index)
                        }
                    }
                }
            }

            Component.onCompleted: {
                // Fix spacing for all progress bars
                var count = musicPlayer.nbOfFrequenciesAvailable()
                var widthSum = 35*count + 1 + 25
                row.spacing = (row.width - widthSum) / (count+3)
                row.anchors.leftMargin = row.spacing
            }
        }
    }

    Popup {
        id: listPopup
        visible: false

        Component {
            id: popupContent

            Rectangle {
                color: "transparent"

                ListViewBase {
                    id: equalizerListView

                    width: listPopup.popupContentWidth
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: 15
                    anchors.topMargin: 15
                    anchors.bottomMargin: 15

                    property string headerText: qsTr("Equalizer configs list ...")

                    model: ListModel {
                        id: model
                    }

                    delegate: Item {
                        width: equalizerListView.width
                        height: row.implicitHeight

                        Row {
                            id: row
                            spacing: 20

                            Image {
                                asynchronous: true
                                width: 65
                                height: 65
                                sourceSize: Qt.size(65, 65)
                                source: "qrc:/images/equalizer"
                            }

                            StyledText {
                                text: name
                                font.pixelSize: 22
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        MouseArea {
                            anchors.fill: row
                            onClicked: {
                                musicPlayer.setEqualizerConfig(index)
                                listPopup.visible = false
                            }
                        }

                        Image {
                            id: clearImage
                            visible: modified

                            asynchronous: true
                            width: 50
                            height: 50
                            sourceSize: Qt.size(50, 50)
                            source: "qrc:/images/remove"

                            anchors.right: parent.right
                            anchors.rightMargin: 15
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        MouseArea {
                            visible: modified
                            anchors.fill: clearImage
                            onClicked: {
                                musicPlayer.resetEqualizerConfig(index)
                                equalizerListView.updateModifiedStatus(index)
                                listPopup.visible = false
                            }
                        }
                    }

                    highlightMoveDuration: 0
                    highlight: Rectangle {
                        color: "transparent"

                        radius: 3
                        border.width: 2
                        border.color: Style.button.clickedOverlayColor
                    }

                    function fillData() {
                        model.clear()
                        var list = musicPlayer.equalizerConfigListNames()
                        for (var i=0; i < list.length; i++) {
                            model.append({"name": list[i],  "modified": musicPlayer.isEqualizerConfigModified(i)})
                        }
                    }

                    function updateModifiedStatus(idx) {
                        model.setProperty(idx, "modified", musicPlayer.isEqualizerConfigModified(idx))
                    }

                    Component.onCompleted: fillData()

                    // Center list on update
                    onVisibleChanged: {
                        if(visible) {
                            var currentId = musicPlayer.currentEqualizerConfigId()
                            equalizerListView.positionViewAtIndex(currentId, ListView.Center)
                            equalizerListView.currentIndex = currentId
                            updateModifiedStatus(currentId)
                        }
                    }
                }
            }
        }

        Component.onCompleted: setContent(popupContent)
    }
}
