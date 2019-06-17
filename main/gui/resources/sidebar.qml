import QtQuick 2.12
import QtQuick.Controls 2.12


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
    function setTotal(val){
        totalValue.text = val + "%";
        totalValue.fontSizeMode = Text.Fit;
    }
    function setDaycare(val){
        daycareValue.text = val + "%";
        daycareValue.fontSizeMode = Text.Fit;
    }
    function setPreschool(val){
        preschoolValue.text = val + "%";
        preschoolValue.fontSizeMode = Text.Fit;
    }
    function setK12School(val){
        k12SchoolValue.text = val + "%";
        k12SchoolValue.fontSizeMode = Text.Fit;
    }
    function setCollege(val){
        collegeValue.text = val + "%";
        collegeValue.fontSizeMode = Text.Fit;
    }
    function setWorkplace(val){
        workplaceValue.text = val + "%";
        workplaceValue.fontSizeMode = Text.Fit;
    }
    function setRetired(val){
        retiredValue.text = val + "%";
        retiredValue.fontSizeMode = Text.Fit;
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
                width: 200
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
                width: 200
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
                width: 200
                height: 30
                Label{
                    id: total
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
                    id: illValue
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
                Label{
                    id: daycare
                    width: parent.width/2
                    height: parent.height
                    text: qsTr("Daycare:")
                    font.family: "fontello"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                    font.pointSize: parent.height/3
                    minimumPointSize: 5
//                            fontSizeMode: Text.Fit
                }
                Label{
                    id: daycareValue
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
            Row{
                spacing: 8
                width: 200
                height: 30
                Label{
                    id: preSchool
                    width: parent.width/2
                    height: parent.height
                    text: qsTr("PreSchool:")
                    font.family: "fontello"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                    font.pointSize: parent.height/3
                    minimumPointSize: 5
//                            fontSizeMode: Text.Fit
                }
                Label{
                    id: preSchoolValue
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
            Row{
                spacing: 8
                width: 200
                height: 30
                Label{
                    id: k12School
                    width: parent.width/2
                    height: parent.height
                    text: qsTr("K12School:")
                    font.family: "fontello"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                    font.pointSize: parent.height/3
                    minimumPointSize: 5
//                            fontSizeMode: Text.Fit
                }
                Label{
                    id: k12SchoolValue
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
            Row{
                spacing: 8
                width: 200
                height: 30
                Label{
                    id: college
                    width: parent.width/2
                    height: parent.height
                    text: qsTr("College:")
                    font.family: "fontello"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                    font.pointSize: parent.height/3
                    minimumPointSize: 5
//                            fontSizeMode: Text.Fit
                }
                Label{
                    id: collegeValue
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
            Row{
                spacing: 8
                width: 200
                height: 30
                Label{
                    id: workplace
                    width: parent.width/2
                    height: parent.height
                    text: qsTr("Workplace:")
                    font.family: "fontello"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                    font.pointSize: parent.height/3
                    minimumPointSize: 5
//                            fontSizeMode: Text.Fit
                }
                Label{
                    id: workplaceValue
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
            Row{
                spacing: 8
                width: 200
                height: 30
                Label{
                    id: retired
                    width: parent.width/2
                    height: parent.height
                    text: qsTr("Retired:")
                    font.family: "fontello"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                    font.pointSize: parent.height/3
                    minimumPointSize: 5
//                            fontSizeMode: Text.Fit
                }
                Label{
                    id: retiredValue
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
