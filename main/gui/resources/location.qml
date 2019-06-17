import QtQuick 2.12
import QtPositioning 5.6
import QtLocation 5.9
import "componentCreation.js" as Script


MapCircle{
    property int id: 0
    property int population: 100
    property int time: 0
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
        onClicked: {
            map.setInvisible();
            controller->SetInfo(id);
            sideRect.open();
        }
    }
}
