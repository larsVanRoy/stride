var component;
var sprite;
var maxElements = 20;
var listSprites  // array => []  object => () !!

function createSpriteObjects()
   {
    for (var i=0; i<listSprites.length; i++) {
        console.log("Create sprite objects: ", listSprites[i].latitude)
        component = Qt.createComponent("location.qml");

        sprite = component.createObject( root, {});

        if (sprite === null) {
            // Error Handling
            console.log("Error creating object");
        }
        else
        {
            var c = "#" + (listSprites[i].ill *100) + "FF0000"   // Rood toevoegen Let op 100% klopt nog niet
            console.log("c= ", c);
            sprite.color = Qt.rgba(254, 0, 0, 0.8) //listSprites[i].ill
//            sprite.color = Qt.tint(sprite.color, c);  // voor overgang kleur vb blauw -> rood
           // sprite.color = Qt.rgba(listSprites[i].ill *254, 0, (1-listSprites[i].ill)*254, 1)  /// werkt niet
//            sprite.color = Qt.rgba(254, 0, 0, listSprites[i].ill)
            sprite.center.longitude = listSprites[i].longitude;
            sprite.center.latitude = listSprites[i].latitude;
            sprite.population = listSprites[i].population;
            sprite.radius = 5000;

            map.addMapItem(sprite);
        }
    }
}


// listSprites vullen met elementen. Later aanpassen
//
//function getElements()
//{
//    listSprites = [
//                {
//                    latitude: 52.36469902,  // Amsterdam
//                    longitude: 4.89990234,
//                    point: Qt.point(52.36469902,4.89990234),
//                    population: 6000,
//                    color: "red",
//                    ill: 0.90
//                },
//                {
//                    latitude: 51.55847,    // Tilburg
//                    longitude: 5.083076,
//                    point: Qt.point(51,5),
//                    population: 7000,
//                    color: "yellow",
//                    ill: 0.70
//                },
//                {
//                    latitude: 51.245,   // Antwerpen
//                    longitude: 4.38,
//                    point: Qt.point(51.245,4.38),
//                    population: 9000,
//                    color: "blue",
//                    ill: 0.10
//                },
//                {
//                    latitude: 51.5,        // London
//                    longitude: 0.13,
//                    point: Qt.point(51.5,0.13),
//                    population: 10000,
//                    color: "green",
//                    ill: 0.20
//                },
//                {
//                    latitude: 53.40,   // Dublin
//                    longitude: -6.28,
//                    point: Qt.point(53.40,-6.28),
//                    population: 4000,
//                    color: "brown",
//                    ill: 0.70
//                }
//            ];

//}

function removeMapElements() {
    //remove entire map
    map.clearMapItems()


    // Alleen elementen die behoren tot mapItemgroep worden verwijderd
//    var count = map.mapItems.length;
    /// opletten: items schuiven steeds naar voor!! Daarom van achter naar voor
//    for (var i=count-1; i>=0; i--) {
//         if ((map.mapItems[i].name !== "markerCircle") && (map.mapItems[i].name !== "markerRect")) {
//                map.removeMapItem(map.mapItems[i]);
//            }
//    }
}

function createSprites(){
    // delete Sprites van vorige keer: 1e 2 moeten we laten zitten
    removeMapElements();
    console.log("DEBUG_1: ", elements.size());

    //backend.getElements();
    listSprites = elements.getElements();
    console.log("DEBUG_2");
    console.log("Opgehaalde elementen-length= ", listSprites.length);
            // elements.getOneElement();
    // btn2 kan ook worden getoond
    console.log("test: ", listSprites[0].latitude);

    // sideBar NIET tonen
//    sideBar.width = 0;

    // Lijst vullen


    createSpriteObjects(listSprites);

    // Change map so all Sprites are shown
    map.fitViewportToMapItems();
    // Clear text
}


function showText() {
    //property alias tekst: sideBar.allText.tekst
    var tekst = "";
    if (listSprites.length !== 0) {
        // window rechts tonen
        sideRect.open()

       for (var i=0; i<listSprites.length; i++) {
           tekst = tekst +
                   "Longitude: " + listSprites[i].longitude + "\n" +
                   "Latitude: " + listSprites[i].latitude + "\n" +
                   "Population: " + listSprites[i].population + "\n" +
                   "Ill: " + listSprites[i].ill + "\n" +
                   "\n";
       }
       allText.text = tekst;

    }

}
