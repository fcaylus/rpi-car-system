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
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import "."

Button {
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
                source: control.iconSource
                height: control.height
                width: Math.min(sourceSize.width, height)
                fillMode: Image.PreserveAspectFit
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
