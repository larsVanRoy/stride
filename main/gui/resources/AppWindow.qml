import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.0
import QtQuick.Layouts 1.2
import Qt.labs.platform 1.0
import QtLocation 5.12
import QtPositioning 5.12

import "componentCreation.js" as Script


ApplicationWindow {
    id: root
    width: 1024
    height: 600
    minimumWidth: flow_.implicitWidth
    minimumHeight: flow_.implicitHeight
    visible: true
    title: "Stride"


    function refreshMap() {
        Script.refreshSprites();
    }

    header: ToolBar {
        leftPadding: 8
        Flow {
            id: flow_
            width: parent.width
            Row {
                id: tools
                Row{
                    id: rowDay
                    Layout.alignment: "AlignVCenter"
                    Label {
                        Layout.minimumHeight: 30
                        text: "Day: "
                        font.family: "fontello"
                        horizontalAlignment: Label.AlignHCenter
                        verticalAlignment: Label.AlignVCenter
                        elide: Label.ElideRight
                    }
                    Label {
                        id: currentDay
                        objectName: "currentDay"
                        text: "0/0"
                        font.family: "fontello"
                        horizontalAlignment: Label.AlignHCenter
                        verticalAlignment: Label.AlignVCenter
                        elide: Label.ElideRight
                    }
                }
                ToolSeparator {
                    contentItem.visible: rowDay.y === previousDay.y
                }
                ToolButton {
                    id: previousDay
                    text: "Previous Day"
                    font.family: "fontello"
                    focusPolicy: Qt.TabFocus
                    onClicked: {
                        controller.previousDay();
                    }
                }
                ToolButton {
                    id: nextDay
                    text: "Next Day"
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
                    text: "Box"
                    font.family: "fontello"
                    focusPolicy: Qt.TabFocus
                    onClicked: {
                        Script.setSpritesInvisible();
                        map.setInvisible();
                        map.selectType = "box";
                    }
                }
                ToolButton {
                    id: circleSelect
                    text: "Radius"
                    font.family: "fontello"
                    focusPolicy: Qt.TabFocus
                    onClicked: {
                        Script.setSpritesInvisible();
                        map.setInvisible();
                        map.selectType = "circle";
                    }
                }
                    ToolSeparator {
                        contentItem.visible: circleSelect.y === selectAgeBracket.y
                    }
                    Row {
                        id: selectAgeBracket
                        ToolButton {
                            id: daycareSelect
                            text: "DC"
                            objectName: "daycare"
                            font.family: "fontello"
                            checkable: true
                            checked: true
                            ToolTip.delay: 500
                            ToolTip.visible: hovered
                            ToolTip.text: objectName
                            onClicked: {
                                if(checked){
                                    controller.AgeSelect(objectName.toString())
                                    daycare.visible = true;
                                }
                                else{
                                    controller.AgeDeSelect(objectName.toString())
                                    daycare.visible = false;
                                }
                            }
                        }
                        ToolButton {
                            id: preschoolSelect
                            text: "PRE"
                            font.family: "fontello"
                            objectName: "preschool"
                            checkable: true
                            checked: true
                            ToolTip.delay: 500
                            ToolTip.visible: hovered
                            ToolTip.text: objectName
                            onClicked: {
                                if(checked){
                                    controller.AgeSelect(objectName.toString())
                                    preSchool.visible = true;
                                }
                                else{
                                    controller.AgeDeSelect(objectName.toString())
                                    preSchool.visible = false;
                                }
                            }
                        }
                        ToolButton {
                            id: k12schoolSelect
                            text: "K12"
                            font.family: "fontello"
                            objectName: "k12school"
                            checkable: true
                            checked: true
                            ToolTip.delay: 500
                            ToolTip.visible: hovered
                            ToolTip.text: objectName
                            onClicked: {
                                if(checked){
                                    controller.AgeSelect(objectName.toString())
                                    k12School.visible = true;
                                }
                                else{
                                    controller.AgeDeSelect(objectName.toString())
                                    k12School.visible = false;
                                }
                            }
                        }
                        ToolButton {
                            id: collegeSelect
                            text: "COL"
                            font.family: "fontello"
                            objectName: "college"
                            checkable: true
                            checked: true
                            ToolTip.delay: 500
                            ToolTip.visible: hovered
                            ToolTip.text: objectName
                            onClicked: {
                                if(checked){
                                    controller.AgeSelect(objectName.toString())
                                    college.visible = true;
                                }
                                else{
                                    controller.AgeDeSelect(objectName.toString())
                                    college.visible = false;
                                }
                            }
                        }
                        ToolButton {
                            id: workplaceSelect
                            text: "WP"
                            font.family: "fontello"
                            objectName: "workplace"
                            checkable: true
                            checked: true
                            ToolTip.delay: 500
                            ToolTip.visible: hovered
                            ToolTip.text: objectName
                            onClicked: {
                                if(checked){
                                    controller.AgeSelect(objectName.toString())
                                    workplace.visible = true;
                                }
                                else{
                                    controller.AgeDeSelect(objectName.toString())
                                    workplace.visible = false;
                                }
                            }
                        }
                        ToolButton {
                            id: retiredSelect
                            text: "RET"
                            font.family: "fontello"
                            objectName: "retired"
                            checkable: true
                            checked: true
                            ToolTip.delay: 500
                            ToolTip.visible: hovered
                            ToolTip.text: objectName
                            onClicked: {
                                if(checked){
                                    controller.AgeSelect(objectName.toString())
                                    retired.visible = true;
                                }
                                else{
                                    controller.AgeDeSelect(objectName.toString())
                                    retired.visible = false;
                                }
                            }
                        }

                    }
                    ToolSeparator {
                        contentItem.visible: selectHealthStatus.y === selectAgeBracket.y
                    }
                    Row {
                        id: selectHealthStatus
                        ToolButton {
                            id: immuneSelect
                            text: "IM"
                            font.family: "fontello"
                            objectName: "immune"
                            checkable: true
                            checked: false
                            ToolTip.delay: 500
                            ToolTip.visible: hovered
                            ToolTip.text: objectName
                            onClicked: {
                                if(checked){
                                    controller.HealthSelect(objectName.toString())
                                }
                                else{
                                    controller.HealthDeSelect(objectName.toString())
                                }
                            }
                        }
                        ToolButton {
                            id: susceptibleSelect
                            text: "SU"
                            font.family: "fontello"
                            objectName: "susceptible"
                            checkable: true
                            checked: false
                            ToolTip.delay: 500
                            ToolTip.visible: hovered
                            ToolTip.text: objectName
                            onClicked: {
                                if(checked){
                                    controller.HealthSelect(objectName.toString())
                                }
                                else{
                                    controller.HealthDeSelect(objectName.toString())
                                }
                            }
                        }
                        ToolButton {
                            id: exposed
                            text: "EX"
                            font.family: "fontello"
                            objectName: "exposed"
                            checkable: true
                            checked: false
                            ToolTip.delay: 500
                            ToolTip.visible: hovered
                            ToolTip.text: objectName
                            onClicked: {
                                if(checked){
                                    controller.HealthSelect(objectName.toString())
                                }
                                else{
                                    controller.HealthDeSelect(objectName.toString())
                                }
                            }
                        }
                        ToolButton {
                            id: infectiousSelect
                            text: "IN"
                            font.family: "fontello"
                            objectName: "infectious"
                            checkable: true
                            checked: true
                            ToolTip.delay: 500
                            ToolTip.visible: hovered
                            ToolTip.text: objectName
                            onClicked: {
                                if(checked){
                                    controller.HealthSelect(objectName.toString())
                                }
                                else{
                                    controller.HealthDeSelect(objectName.toString())
                                }
                            }
                        }
                        ToolButton {
                            id: infectiousAndSymptomaticSelect
                            text: "IS"
                            font.family: "fontello"
                            objectName: "infectious and symptomatic"
                            checkable: true
                            checked: true
                            ToolTip.delay: 500
                            ToolTip.visible: hovered
                            ToolTip.text: objectName
                            onClicked: {
                                if(checked){
                                    controller.HealthSelect(objectName.toString())
                                }
                                else{
                                    controller.HealthDeSelect(objectName.toString())
                                }
                            }
                        }
                        ToolButton {
                            id: symptomaticSelect
                            text: "SY"
                            font.family: "fontello"
                            objectName: "symptomatic"
                            checkable: true
                            checked: true
                            ToolTip.delay: 500
                            ToolTip.visible: hovered
                            ToolTip.text: objectName
                            onClicked: {
                                if(checked){
                                    controller.HealthSelect(objectName.toString())
                                }
                                else{
                                    controller.HealthDeSelect(objectName.toString())
                                }
                            }
                        }
                        ToolButton {
                            id: recoveredSelect
                            text: "RE"
                            font.family: "fontello"
                            objectName: "recovered"
                            checkable: true
                            checked: true
                            ToolTip.delay: 500
                            ToolTip.visible: hovered
                            ToolTip.text: objectName
                            onClicked: {
                                if(checked){
                                    controller.HealthSelect(objectName.toString())
                                }
                                else{
                                    controller.HealthDeSelect(objectName.toString())

                                }
                            }
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

                         selectionBox.visible = true;
                         selectionBox.state = "created"
                     }
                     else if(map.selectType === "circle") {
                         var radius = selectionRadius.center.distanceTo(map.toCoordinate(Qt.point(mouse.x, mouse.y)));
                         selectionRadius.radius = radius
                         selectionRadius.visible = true;
                         selectionRadius.state = "created";
                         controller.RadiusSelect(radius/1000);
                         Script.setSpritesVisible();
                     }
                     if(map.selectType !== "default"){
                         Script.setSpritesVisible();
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
        objectName: "sideRect"
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

            StackView{
                anchors {left: parent.left}

                Column{
                    width: parent.width
                    height: parent.height
                    spacing: 2
                    Row{
                        width: 200
                        height: 30
                        Label {
                            id: locName
                            objectName: "locName"
                            width: parent.width
                            height: parent.height
                            text: qsTr("Location name...")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: 100
                            minimumPointSize: 5
                            fontSizeMode: Text.Fit
                            onTextChanged: fontSizeMode = Text.Fit;
                        }
                    }
                    Row{
                        spacing: 8
                        width: 200
                        height: 30
                        id: population
                        Label{
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("Population:")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
                        }
                        Label{
                            id: populationValue
                            objectName: "populationValue"
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("0")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
                        }
                    }
                    Row{
                        spacing: 8
                        width: 200
                        height: 30
                        id: total
                        Label{
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("Total:")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
                        }
                        Label{
                            id: totalValue
                            objectName: "totalValue"
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("0%")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
                        }
                    }
                    Row{
                        spacing: 8
                        width: 200
                        height: 30
                        id: daycare
                        Label{
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("Daycare:")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
                        }
                        Label{
                            id: daycareValue
                            objectName: "daycareValue"
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("0%")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
                        }
                    }
                    Row{
                        spacing: 8
                        width: 200
                        height: 30
                        id: preSchool
                        Label{
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("PreSchool:")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
                        }
                        Label{
                            id: preSchoolValue
                            objectName: "preschoolValue"
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("0%")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
                        }
                    }
                    Row{
                        spacing: 8
                        width: 200
                        height: 30
                        id: k12School
                        Label{
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("K12School:")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
                        }
                        Label{
                            id: k12schoolValue
                            objectName: "k12schoolValue"
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("0%")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
                        }
                    }
                    Row{
                        spacing: 8
                        width: 200
                        height: 30
                        id: college
                        Label{
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("College:")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
                        }
                        Label{
                            id: collegeValue
                            objectName: "collegeValue"
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("0%")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
                        }
                    }
                    Row{
                        spacing: 8
                        width: 200
                        height: 30
                        id: workplace
                        Label{
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("Workplace:")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
                        }
                        Label{
                            id: workplaceValue
                            objectName: "workplaceValue"
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("0%")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
                        }
                    }
                    Row{
                        spacing: 8
                        width: 200
                        height: 30
                        id: retired
                        Label{
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("Retired:")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
                        }
                        Label{
                            id: retiredValue
                            objectName: "retiredValue"
                            width: parent.width/2
                            height: parent.height
                            text: qsTr("0%")
                            font.family: "fontello"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: parent.height/3
                            minimumPointSize: 5
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
