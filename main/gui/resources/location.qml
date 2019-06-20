import QtQuick 2.12
import QtPositioning 5.6
import QtLocation 5.9
import "componentCreation.js" as Script


MapCircle{
    property int id: 0
    property int population: 100
    property bool clickable: true
    objectName: "location"

    center {
        latitude: 50
        longitude: 4
    }
    radius: 50.0 // 5 km
    border.width: 2
    border.color: "black"

    MouseArea {
        anchors.fill: parent
        visible: clickable
        onClicked: {
            map.setInvisible();
            controller.SetSelectedLocation(id);
        }
    }
}
