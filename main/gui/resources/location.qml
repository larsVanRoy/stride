import QtQuick 2.12
import QtPositioning 5.6
import QtLocation 5.9
import "componentCreation.js" as Script


MapCircle{
    property int id: 0
    property int population: 100
    property int time: 0
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
            sideBar.setName(controller.GetLocationName(id));
            sideBar.setLatitude(controller.GetLatitude(id));
            sideBar.setLongitude(controller.GetLongitude(id));
            sideBar.setIll(controller.GetIll(id));
            sideRect.open()
        }
    }
}
