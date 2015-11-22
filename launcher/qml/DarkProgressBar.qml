import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import "."

ProgressBar {
    style: ProgressBarStyle {
        background: Rectangle {
            radius: 3
            color: Style.progressBar.backgroundColor
            border.color: Style.progressBar.borderColor
            border.width: 2
            implicitHeight: 8
        }

        progress: Rectangle {
            color: Style.progressBar.progressColor
            border.color: Style.progressBar.borderColor
        }
    }
}

