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
    Component.onCompleted: {
        Script.createSprites();
    }

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
                    onClicked: controller.print();
                }
                ToolButton {
                    id: nextDay
                    text: "Next Day" // icon-paste
                    font.family: "fontello"
                    focusPolicy: Qt.TabFocus
                    onClicked: {
                        controller.nextDay();
                        Script.createSprites();
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
            width: 200
            anchors {right: parent.right}

            function setName(name){
                locName.text = name;
                locName.fontSizeMode = Text.Fit;
            }
            function setLongitude(longitude){
                longitudeValue.text = longitude;
            }
            function setLatitude(latitude){
                latitudeValue.text = latitude;
            }
            function setIll(ill){
                illValue.text = ill + "%";
            }

            StackView{
                anchors {left: parent.left}

                Column{
                    width: parent.width
                    height: parent.height
                    x: 0
                    y: 0
                    spacing: 2
                    Text {
                        id: locName
                        x: 0
                        y: 0
                        width: 140
                        height: 30
                        text: qsTr("Location name...")
                        verticalAlignment: Text.AlignVLeft
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: 100
                        minimumPointSize: 5
                        fontSizeMode: Text.Fit
                    }
                    Row{
                        width: 140
                        height: 30
                        x: 0
                        y: 38
                        spacing: 8
                        Text{
                            id: longitudeText
                            x: 0
                            y: 0
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("longitude:")
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            fontSizeMode: Text.Fit
                        }
                        Text{
                            id: longitudeValue
                            x: 0
                            y: 0
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("0.0")
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            fontSizeMode: Text.Fit
                        }
                    }
                    Row{
                        spacing: 8
                        width: 140
                        height: 30
                        x: 0
                        y: 72
                        Text{
                            id: latitudeText
                            x: 0
                            y: 0
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("latitude:")
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            fontSizeMode: Text.Fit
                        }
                        Text{
                            id: latitudeValue
                            x: 0
                            y: 0
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("0.0")
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            fontSizeMode: Text.Fit
                        }
                    }
                    Row{
                        spacing: 8
                        width: 140
                        height: 30
                        x: 0
                        y: 72
                        Text{
                            id: ill
                            x: 0
                            y: 0
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("ill:")
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            fontSizeMode: Text.Fit
                        }
                        Text{
                            id: illValue
                            x: 0
                            y: 0
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("0%")
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            fontSizeMode: Text.Fit
                        }
                    }
                }
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
