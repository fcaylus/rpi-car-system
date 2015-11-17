import QtQuick 2.3
import QtQuick.Controls 1.2

Button {
    property bool inToolbar: false
    property bool inMainMenu: false
    property bool alignCenter: false

    property bool hasSecondIcon: false
    property url secondIcon;
    property int statesNumber: 1 + (checkable ?  1 : 0) + (hasSecondIcon ? 1 : 0)
    property int currentState: 0

    property real iconScale: .9
    property bool bold: false

    // Disable auto-management of currentState var
    property bool disableAutoMgr: false

    function updateCheckedState() {
        checked = checkable && currentState >= 1
    }

    onClicked: {
        // Increase current state
        if(!disableAutoMgr)
            currentState = (currentState + 1) % statesNumber
        updateCheckedState()
    }

    style: DarkButtonStyle {}
}
