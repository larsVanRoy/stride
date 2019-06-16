import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.0
import Qt.labs.platform 1.0
import QtLocation 5.12
import QtPositioning 5.12
//import location 1.0

import "componentCreation.js" as Script


ApplicationWindow {
    id: root
    width: 1024
    height: 600
    visible: true
    title: "Stride"

    MessageDialog {
        id: errorDialog
    }

    header: ToolBar {
        leftPadding: 8

        Flow {
            id: flow_
            width: parent.width
            Row {
                id: tools
                ToolButton {
                    id: openButton
                    text: "\uF115" // icon-folder-open-empty
                    font.family: "fontello"
                    onClicked: Script.createSprites()
                }
                ToolSeparator {
                    contentItem.visible: openButton.y === previousDay.y
                }
                ToolButton {
                    id: previousDay
                    text: "Previous Day" // icon-docs
                    font.family: "fontello"
                    focusPolicy: Qt.TabFocus
                    onClicked: {
                        controller.previousDay();
                        Script.refreshSprites();
                    }
                }
                ToolButton {
                    id: nextDay
                    text: "Next Day" // icon-paste
                    font.family: "fontello"
                    focusPolicy: Qt.TabFocus
                    onClicked: {
                        controller.nextDay();
                        Script.refreshSprites();
                    }
                }
            }
        }
    }

    Rectangle
    {
        id: screen
        anchors.fill: parent

        Plugin {
            id: mapPlugin
            name: "osm"
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
             Component.onCompleted: {
                 Script.initializeMap();
             }
         }
    }

    Rectangle{
        id: sideRect
        height: parent.height
        anchors {right: parent.right}

        state: "closed"

        function open() {
            if(sideRect.state == "closed")
                sideRect.state = "open";
            return;
        }

        function toggle() {
            if(sideRect.state == "closed")
                sideRect.state = "open";
            else
                sideRect.state = "closed";
            return;
        }

        Rectangle {
            id: sideBar
            height: parent.height
            width: root.width/5
            anchors {right: parent.right}

            function setName(name){
                locName.text = name;
                locName.fontSizeMode = Text.Fit;
            }
            function setLongitude(longitude){
//                longitudeValue.text = longitude;
            }
            function setLatitude(latitude){
//                latitudeValue.text = latitude;
            }
            function setIll(ill){
                illValue.text = ill + "%";
                illValue.fontSizeMode = Text.Fit;
            }

            StackView{
                anchors {left: parent.left}

                Column{
                    width: parent.width
                    height: parent.height
//                    x: 0
//                    y: 0
                    spacing: 2
                    Row{
                        width: 140
                        height: 30
                        Text {
                            id: locName
                            width: 140
                            height: 30
                            text: qsTr("Location name...")
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: 100
                            minimumPointSize: 5
                            fontSizeMode: Text.Fit
                        }
                    }
                    Row{
                        spacing: 8
                        width: 140
                        height: 30
                        Text{
                            id: population
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("Population:")
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
//                            fontSizeMode: Text.Fit
                        }
                        Text{
                            id: populationValue
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("0")
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
//                            fontSizeMode: Text.Fit
                        }
                    }
                    Row{
                        spacing: 8
                        width: 140
                        height: 30
                        Text{
                            id: ill
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("ill:")
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
//                            fontSizeMode: Text.Fit
                        }
                        Text{
                            id: illValue
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("0%")
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
//                            fontSizeMode: Text.Fit
                        }
                    }
                }
            }
        }


        Button {
            id: sideBarButton
            width: root.width/30
            height: root.height/12
            anchors {right: sideBar.left; top: sideBar.top}
            onClicked: sideRect.toggle();
        }

        states: [
            State {
                name: "open"
                PropertyChanges { target: sideBar; width: root.width/5 }

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
