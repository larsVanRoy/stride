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

    function test() {
        console.log("test ma man")
    }

    function refreshMap() {
        Script.refreshSprites();
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
                    onClicked: controller.SetInfo();
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
                    }
                }
                ToolButton {
                    id: nextDay
                    text: "Next Day" // icon-paste
                    font.family: "fontello"
                    focusPolicy: Qt.TabFocus
                    onClicked: {
                        controller.nextDay();
                    }
                }
                ToolSeparator {
                    contentItem.visible: nextDay.y === boxSelect.y
                }
                ToolButton {
                    id: boxSelect
                    text: "Box" // icon-docs
                    font.family: "fontello"
                    focusPolicy: Qt.TabFocus
                    onClicked: {
                        map.setInvisible();
                        map.selectType = "box"
                    }
                }
                ToolButton {
                    id: circleSelect
                    text: "Radius" // icon-paste
                    font.family: "fontello"
                    focusPolicy: Qt.TabFocus
                    onClicked: {
                        map.setInvisible();
                        map.selectType = "circle";
                    }
                }
//                ToolSeparator {
//                    contentItem.visible: nextDay.y === boxSelect.y
//                }
//                ComboBox {

//                }
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

             property bool disable_panning : false
             property int no_panning : (MapGestureArea.PinchGesture)
             property int panning : (MapGestureArea.PanGesture | MapGestureArea.PinchGesture)

             property string selectType: "default"

             gesture.acceptedGestures: disable_panning ? no_panning : panning

             property point topLeft: Qt.point(0,0)

             MouseArea {
                 id: mouseArea
                 anchors.fill: parent
                 property int typeId: 0

                 onPressed: {
                     console.log("hold");
                    if(map.selectType !== "default")  {
                        controller.InitializeMultiSelect(map.toCoordinate(Qt.point(mouse.x, mouse.y)).longitude, map.toCoordinate(Qt.point(mouse.x, mouse.y)).latitude);
                        map.disable_panning = true;
                        if(map.selectType === "box"){
                            selectionBox.topLeft = map.toCoordinate(Qt.point(mouse.x, mouse.y));
                            selectionBox.bottomRight = map.toCoordinate(Qt.point(mouse.x, mouse.y));
                        }
                        else{
                            selectionRadius.center = map.toCoordinate(Qt.point(mouse.x, mouse.y));
                        }
                    }
                 }

                 onPositionChanged: {
                     if(map.disable_panning){
                         if(map.selectType === "box")  {
                             var co1 = selectionBox.topLeft;
                             var co2 = map.toCoordinate(Qt.point(mouse.x, mouse.y));
                             var topLeft = QtPositioning.coordinate(0,0);
                             var bottomRight = QtPositioning.coordinate(0,0);

                             if(co1.latitude < co2.latitude){
                                 topLeft.latitude = co2.latitude;
                                 bottomRight.latitude = co1.latitude;
                             }
                             else{
                                 topLeft.latitude = co1.latitude;
                                 bottomRight.latitude = co2.latitude;
                             }
                             if(co1.longitude > co2.longitude){
                                 topLeft.longitude = co2.longitude;
                                 bottomRight.longitude = co1.longitude;
                             }
                             else{
                                 topLeft.longitude = co1.longitude;
                                 bottomRight.longitude = co2.longitude;
                             }

                             selectionBox.topLeft = topLeft;
                             selectionBox.bottomRight = bottomRight;

                             selectionBox.visible = true;
                             selectionBox.state = "created"
                         }
                         else if(map.selectType === "circle") {
                             var radius = selectionRadius.center.distanceTo(map.toCoordinate(Qt.point(mouse.x, mouse.y)));
                             selectionRadius.radius = radius
                             selectionRadius.visible = true;
                             selectionRadius.state = "created";
                         }
                     }
                 }

                 onReleased: {
                     console.log("release");
                     if(map.selectType === "box")  {
                         var co1 = selectionBox.topLeft;
                         var co2 = map.toCoordinate(Qt.point(mouse.x, mouse.y));
                         var topLeft = QtPositioning.coordinate(0,0);
                         var bottomRight = QtPositioning.coordinate(0,0);

                         if(co1.latitude < co2.latitude){
                             topLeft.latitude = co2.latitude;
                             bottomRight.latitude = co1.latitude;
                         }
                         else{
                             topLeft.latitude = co1.latitude;
                             bottomRight.latitude = co2.latitude;
                         }
                         if(co1.longitude > co2.longitude){
                             topLeft.longitude = co2.longitude;
                             bottomRight.longitude = co1.longitude;
                         }
                         else{
                             topLeft.longitude = co1.longitude;
                             bottomRight.longitude = co2.longitude;
                         }

                         selectionBox.topLeft = topLeft;
                         selectionBox.bottomRight = bottomRight;
                         controller.BoxSelect(co2.longitude, co2.latitude);

//                         console.log("box2");
//                         console.log(co2);
                         selectionBox.visible = true;
                         selectionBox.state = "created"
//                         console.log("bottomright: ", selectionBox.bottomRight);
//                         console.log("topLeft: ", selectionBox.topLeft);
                     }
                     else if(map.selectType === "circle") {
                         var radius = selectionRadius.center.distanceTo(map.toCoordinate(Qt.point(mouse.x, mouse.y)));
                         selectionRadius.radius = radius
                         selectionRadius.visible = true;
                         selectionRadius.state = "created";
                         console.log("selectionRadius visible?", selectionRadius.visible, " ", selectionRadius.center);
                         controller.RadiusSelect(radius/1000);
                     }
                    map.selectType = "default"
                    map.disable_panning = false;
                 }

                 onClicked: {
                    if(selectionBox.state == "created"){
                        selectionBox.state = "visible";
                    }
                    else if(selectionBox.state == "visible"){
                        selectionBox.state = "invisible";
                        selectionBox.visible = false;
                    }
                    if(selectionRadius.state == "created"){
                        selectionRadius.state = "visible";

                    }
                    else if(selectionRadius.state == "visible"){
                        selectionRadius.state = "invisible";
                        selectionRadius.visible = false;
                    }
                 }
             }

             function setInvisible(){
                 selectionBox.state = "invisible"
                 selectionRadius.state = "invisible"
                 selectionBox.visible = false
                 selectionRadius.visible = false
             }

             MapRectangle{
                 id: selectionBox

                 visible: false
                 border.width: 2
                 border.color: "black"

                 state: "invisible"
             }
             MapCircle{
                 id: selectionRadius

                 visible: false
                 border.width: 2
                 border.color: "black"
                 state: "invisible"
             }


             Component.onCompleted: {
                 Script.initializeMap();
                 map.fitViewportToVisibleMapItems();
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
            function setPopulation(pop){
                populationValue.text = pop;
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
                        Label {
                            id: locName
                            width: parent.width
                            height: parent.height
                            text: qsTr("Location name...")
                            font.family: "fontello"
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
                        Label{
                            id: population
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("Population:")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
//                            fontSizeMode: Text.Fit
                        }
                        Label{
                            id: populationValue
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("0")
                            font.family: "fontello"
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
                        Label{
                            id: ill
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("ill:")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
//                            fontSizeMode: Text.Fit
                        }
                        Label{
                            id: illValue
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("0%")
                            font.family: "fontello"
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
            text: ">"
            font.family: "fontello"
            onClicked: sideRect.toggle();
        }

        states: [
            State {
                name: "open"
                PropertyChanges { target: sideBar; width: root.width/5;}
                PropertyChanges { target: sideBarButton; text: ">";}
            },
            State {
                name: "closed"
                PropertyChanges { target: sideBar; width: 0; }
                PropertyChanges { target: sideBarButton; text: "<";}
            }
        ]

        transitions: Transition {
            NumberAnimation {target: sideBar; properties: "width"; duration: 500; easing.type: Easing.InOutQuad}
        }
    }
}
