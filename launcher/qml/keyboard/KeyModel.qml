import QtQuick 2.0

Item {
    property QtObject firstRowModel
    property QtObject secondRowModel
    property QtObject thirdRowModel

    Component.onCompleted: {
        switch(Qt.locale().name.substring(0,2)) {
            case "fr":
                firstRowModel = firstAzerty
                secondRowModel = secondAzerty
                thirdRowModel = thirdAzerty
                break;
            default:
                firstRowModel = firstQwerty
                secondRowModel = secondQwerty
                thirdRowModel = thirdQwerty
                break;
        }
    }

    ListModel {
        id: firstQwerty
        ListElement { letter: "q"; firstSymbol: "1"}
        ListElement { letter: "w"; firstSymbol: "2"}
        ListElement { letter: "e"; firstSymbol: "3"}
        ListElement { letter: "r"; firstSymbol: "4"}
        ListElement { letter: "t"; firstSymbol: "5"}
        ListElement { letter: "y"; firstSymbol: "6"}
        ListElement { letter: "u"; firstSymbol: "7"}
        ListElement { letter: "i"; firstSymbol: "8"}
        ListElement { letter: "o"; firstSymbol: "9"}
        ListElement { letter: "p"; firstSymbol: "0"}
    }

    ListModel {
        id: secondQwerty
        ListElement { letter: "a"; firstSymbol: "!"}
        ListElement { letter: "s"; firstSymbol: "@"}
        ListElement { letter: "d"; firstSymbol: "#"}
        ListElement { letter: "f"; firstSymbol: "$"}
        ListElement { letter: "g"; firstSymbol: "%"}
        ListElement { letter: "h"; firstSymbol: "&"}
        ListElement { letter: "j"; firstSymbol: "*"}
        ListElement { letter: "k"; firstSymbol: "?"}
        ListElement { letter: "l"; firstSymbol: "/"}
    }

    ListModel {
        id: thirdQwerty
        ListElement { letter: "z"; firstSymbol: "_"}
        ListElement { letter: "x"; firstSymbol: "\""}
        ListElement { letter: "c"; firstSymbol: "'"}
        ListElement { letter: "v"; firstSymbol: "("}
        ListElement { letter: "b"; firstSymbol: ")"}
        ListElement { letter: "n"; firstSymbol: "-"}
        ListElement { letter: "m"; firstSymbol: "+"}
    }

    ListModel {
        id: firstAzerty
        ListElement { letter: "a"; firstSymbol: "1"}
        ListElement { letter: "z"; firstSymbol: "2"}
        ListElement { letter: "e"; firstSymbol: "3"}
        ListElement { letter: "r"; firstSymbol: "4"}
        ListElement { letter: "t"; firstSymbol: "5"}
        ListElement { letter: "y"; firstSymbol: "6"}
        ListElement { letter: "u"; firstSymbol: "7"}
        ListElement { letter: "i"; firstSymbol: "8"}
        ListElement { letter: "o"; firstSymbol: "9"}
        ListElement { letter: "p"; firstSymbol: "0"}
    }

    ListModel {
        id: secondAzerty
        ListElement { letter: "q"; firstSymbol: "@"}
        ListElement { letter: "s"; firstSymbol: "#"}
        ListElement { letter: "d"; firstSymbol: "€"}
        ListElement { letter: "f"; firstSymbol: "%"}
        ListElement { letter: "g"; firstSymbol: "&"}
        ListElement { letter: "h"; firstSymbol: "-"}
        ListElement { letter: "j"; firstSymbol: "+"}
        ListElement { letter: "k"; firstSymbol: "*"}
        ListElement { letter: "l"; firstSymbol: "("}
        ListElement { letter: "m"; firstSymbol: ")"}
    }

    ListModel {
        id: thirdAzerty
        ListElement { letter: "w"; firstSymbol: "_"}
        ListElement { letter: "x"; firstSymbol: "\""}
        ListElement { letter: "c"; firstSymbol: "'"}
        ListElement { letter: "v"; firstSymbol: "?"}
        ListElement { letter: "b"; firstSymbol: "!"}
        ListElement { letter: "n"; firstSymbol: "/"}
        ListElement { letter: "'"; firstSymbol: "\\"}
    }
}
