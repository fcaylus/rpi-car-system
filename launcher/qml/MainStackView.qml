/*
 * This file is part of RPI Car System.
 * Copyright (c) 2016 Fabien Caylus <toutjuste13@gmail.com>
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

import QtQuick 2.3
import QtQuick.Controls 1.2
import "."

StackView {
    id: mainStackView

    Component.onCompleted: soundManager.init()

    delegate: StackViewDelegate {
        function transitionFinished(properties)
        {
            properties.exitItem.x = 0
        }

        pushTransition: StackViewTransition {
            PropertyAnimation {
                target: enterItem
                property: "x"
                from: Style.windowWidth
                to: 0
            }
            PropertyAnimation {
                target: exitItem
                property: "x"
                from: 0
                to: -Style.windowWidth
            }
        }

        popTransition: StackViewTransition {
            PropertyAnimation {
                target: enterItem
                property: "x"
                from: -Style.windowWidth
                to: 0
            }
            PropertyAnimation {
                target: exitItem
                property: "x"
                from: 0
                to: Style.windowWidth
            }
        }
    }

    initialItem: Item {
        GridMenu {
            expectedDepth: 1

            property Component activityMusic: ActivityMusic {}
            property Component activityTransfer: ActivityTransfer {}
            property Component activitySettings: ActivitySettings {}

            map: {
                1: activityMusic,
                7: activityTransfer,
                8: activitySettings
            }

            model: ListModel {
                ListElement {
                    index: 1
                    isEnabled: true
                    title: qsTr("My Music")
                    icon: "qrc:/images/music"
                }
                ListElement {
                    index: 2
                    isEnabled: false
                    title: qsTr("Radio")
                    icon: "qrc:/images/radio"
                }
                ListElement {
                    index: 3
                    isEnabled: false
                    title: qsTr("My Videos")
                    icon: "qrc:/images/video"
                }
                ListElement {
                    index: 4
                    isEnabled: false
                    title: qsTr("Informations")
                    icon: "qrc:/images/car"
                }
                ListElement {
                    index: 5
                    isEnabled: false
                    title: qsTr("GPS")
                    icon: "qrc:/images/map"
                }
                ListElement {
                    index: 6
                    isEnabled: false
                    title: qsTr("Call")
                    icon: "qrc:/images/phone"
                }
                ListElement {
                    index: 7
                    isEnabled: true
                    title: qsTr("Transfer")
                    icon: "qrc:/images/upload"
                }
                ListElement {
                    index: 8
                    isEnabled: true
                    title: qsTr("Settings")
                    icon: "qrc:/images/settings"
                }
            }
        }
    }
}
