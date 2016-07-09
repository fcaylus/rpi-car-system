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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import "."

Button {
    id: button

    property bool repeatClick: false
    property int repeatFirstDelay: 500
    property int repeatInterval: 100

    Timer {
        id: repeatTimer
        running: false
        onTriggered: {
            if(button.pressed) {
                button.clicked()

                if(interval === button.repeatFirstDelay) {
                    interval = button.repeatInterval
                    repeat = true
                    start()
                }
            }
        }
    }

    onPressedChanged: {
        if(repeatClick) {
            repeatTimer.stop()
            if(pressed) {
                repeatTimer.interval = button.repeatFirstDelay
                repeatTimer.repeat = false
                repeatTimer.start()
            }
        }
    }


    style: ButtonStyle {
        background: Rectangle {
            color: "transparent"
        }

        label: Item {
            implicitWidth: buttonImg.implicitWidth
            implicitHeight: buttonImg.implicitHeight

            Image {
                id: buttonImg
                asynchronous: true
                height: control.height
                width: control.width
                sourceSize.width: width
                sourceSize.height: height
                fillMode: Image.PreserveAspectFit

                source: control.iconSource
            }

            ColorOverlay {
                anchors.fill: buttonImg
                source: buttonImg
                color: Style.button.clickedOverlayColor
                visible: control.pressed
            }
        }
    }

}
