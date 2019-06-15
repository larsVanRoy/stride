import QtQuick 2.12
import QtPositioning 5.6
import QtLocation 5.9
import "componentCreation.js" as Script


MapCircle{
    property int population: 100
    property int time: 0
    // center: QtPositioning.coordinate(50.41136, 4.44448)
    center {
        latitude: 50
        longitude: 4
    }
    radius: 50.0 // 5 km
    //color: "lightsteelblue"
    border.width: 2
    border.color: "black"

    MouseArea {
        anchors.fill: parent
        onClicked: {allText.text =   "Population: " + population + "\n" +
                                     "Radius:     " + radius + "\n" +
                                     "number:     " + time + "\n";
                    sideRect.open();
        }

//            onClicked: messages.text = "Population: " + population + "\n" +
//                                       "and a lot of people are ill" +  "\n" +
//                                       "derde regel";
    }
}
