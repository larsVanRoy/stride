import QtQuick 2.12
import QtQuick.Controls 2.12


Frame {
    id: sideBar
    height: parent.height
    width: 200

    anchors {right: parent.right}

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
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 16
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
                    font.pixelSize: 12
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
                    font.pixelSize: 12
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
                    font.pixelSize: 12
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
                    font.pixelSize: 12
                }
            }
        }
    }
}
