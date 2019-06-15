import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.0
import Qt.labs.platform 1.0
import QtLocation 5.12
import QtPositioning 5.12
import location 1.0

import "componentCreation.js" as Script


ApplicationWindow {
    id: root
    width: 1024
    height: 600
    visible: true
    title: "Stride"

    Component.onCompleted: {
        x = Screen.width / 2 - width / 2
        y = Screen.height / 2 - height / 2
    }


//    FileDialog {
//        id: openDialog
//        fileMode: FileDialog.OpenFile
//        selectedNameFilter.index: 1
//        nameFilters: ["Text files (*.txt)", "HTML files (*.html *.htm)"]
//        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
//        onAccepted: document.load(file)
//    }


    MessageDialog {
        id: errorDialog
    }

    header: ToolBar {
        leftPadding: 8

        Flow {
            id: flow
            width: parent.width

            Row {
                id: fileRow
                ToolButton {
                    id: openButton
                    text: "\uF115" // icon-folder-open-empty
                    font.family: "fontello"
                    onClicked: Script.createSprites()
//                    onClicked: openDialog.open()
                }
                ToolSeparator {
                    contentItem.visible: fileRow.y === dayRow.y
                }
            }

            Row {
                id: dayRow
                ToolButton {
                    id: previousDay
                    text: "Previous Day" // icon-docs
                    font.family: "fontello"
                    focusPolicy: Qt.TabFocus
                    onClicked: controller.print();

//                    enabled: textArea.selectedText
//                    onClicked: textArea.copy()
                }
//                ToolButton {
//                    id: cutButton
//                    text: "\uE802" // icon-scissors
//                    font.family: "fontello"
//                    focusPolicy: Qt.TabFocus
//                    enabled: textArea.selectedText
//                    onClicked: textArea.cut()
//                }
                ToolButton {
                    id: nextDay
                    text: "Next Day" // icon-paste
                    font.family: "fontello"
                    focusPolicy: Qt.TabFocus
                    onClicked: { console.log("Clicked"); Script.createSprites();}
                }
            }
        }
    }

//    DocumentHandler {
//        id: document
//        document: textArea.textDocument
//        cursorPosition: textArea.cursorPosition
//        selectionStart: textArea.selectionStart
//        selectionEnd: textArea.selectionEnd
//        textColor: colorDialog.color
//        Component.onCompleted: document.load("qrc:/gui.html")
//        onLoaded: {
//            textArea.text = text
//        }
//        onError: {
//            errorDialog.text = message
//            errorDialog.visible = true
//        }
//    }

    Rectangle
    {
        id: screen
        anchors.fill: parent

        Plugin {
            id: mapPlugin
            name: "osm" // bestaande map
        }

        Map {
             id: map
             anchors.fill: parent  // fill Item
             plugin: mapPlugin
             center: QtPositioning.coordinate(50.41136, 4.44448) // Charleroi
             zoomLevel: 7
             gesture.enabled: true
             gesture.acceptedGestures: MapGestureArea.PinchGesture | MapGestureArea.PanGesture

             property int firstClick: 0   // nog niet geClicked
             property point topLeft: Qt.point(0,0)

             MouseArea {
                 id: mouseArea
                 anchors.fill: parent

                 TapHandler {
                     onTapped: console.log("left clicked")

                 }
                 TapHandler {
                     acceptedButtons: Qt.RightButton
                     onTapped: console.log("right clicked")
                 }
             }
         }
    }

    Rectangle{
        id: sideRect
        height: parent.height
        anchors {right: parent.right}

        state: "closed"

        function open() {
            console.log("Pre Open: ", sideRect.state);
            if(sideRect.state == "closed")
                sideRect.state = "open";
            console.log("Post Open: ", sideRect.state);
            return;
        }

        function toggle() {
            console.log("toggle: ", sideRect.state);
            if(sideRect.state == "closed")
                sideRect.state = "open";
            else
                sideRect.state = "closed";
            return;
        }

        Rectangle {
                id: sideBar
                height: parent.height
                width: 0   // bij starten niets te zien
    //            color: "lightsteelblue"

                anchors {right: parent.right}



                Text {
                    id: allText
                    width: parent.width
                    height: parent.height

                }
        }
        Button {
            id: sideBarButton
            width: 30
            height: 50
            anchors {right: sideBar.left; top: sideBar.top}
            onClicked: sideRect.toggle();
        }

        states: [
            State {
                name: "open"
                PropertyChanges { target: sideBar; width: 200 }

            },
            State {
                name: "closed"
                PropertyChanges { target: sideBar; width: 0 }
            }
        ]

        transitions: Transition {
            NumberAnimation {target: sideBar; properties: "width"; duration: 500; easing.type: Easing.InOutQuad}
        }



    }
}
