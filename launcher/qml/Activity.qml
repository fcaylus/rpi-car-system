/*
 * This file is part of RPI Car System.
 * Copyright (c) 2015 Fabien Caylus <toutjuste13@gmail.com>
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

Rectangle {
    id: view

    gradient: Gradient {
        GradientStop {
            color: Style.backgroundColorEnd
            position: 0
        }
        GradientStop {
            color: Style.backgroundColorStart
            position: .5
        }
        GradientStop {
            color: Style.backgroundColorEnd
            position: 1
        }
    }

    property Component control

    property alias controlItem: controlLoader.item

    property rect controlBounds: Qt.rect(largestControlItem.x + controlBoundsItem.x,
        largestControlItem.y + controlBoundsItem.y, controlBoundsItem.width, controlBoundsItem.height)

    // Substract the "home" button width (it's the toolbar height)
    property int availableToolBarWidth: toolbar.width - toolbar.height
    property int toolbarHeight: toolbar.height

    Item {
        id: largestControlItem
        x: 0
        y: 0
        width: parent.width
        height: parent.height - toolbar.height

        Item {
            id: controlBoundsItem
            x: parent.width / 2 - controlBoundsItem.width / 2
            y: 0
            width: parent.width
            height: parent.height

            Loader {
                id: controlLoader
                sourceComponent: control
                anchors.centerIn: parent

                property alias view: view
            }
        }
    }

    ActivityToolbar {
        id: toolbar
    }
}
