var component;
var sprite;
var listSprites  // array => []  object => () !!

function createSpriteObjects()
{
    var time = 0;
    for (var i=0; i<listSprites.length; i++) {
        // console.log("Create sprite objects: ", listSprites[i].latitude)
        component = Qt.createComponent("location.qml");

        sprite = component.createObject( root, {});

        if (sprite === null) {
            // Error Handling
            console.log("Error creating object");
        }
        else
        {
            var c = "#" + (listSprites[i].ill * 100) + "FF0000"   // Rood toevoegen Let op 100% klopt nog niet
            // console.log("c= ", c);
            sprite.color = Qt.rgba(254, 0, 0, 0.8) //listSprites[i].ill
            sprite.center.longitude = listSprites[i].longitude;
            sprite.center.latitude = listSprites[i].latitude;
            sprite.population = listSprites[i].population;
            var scale = 12/(10000-50)*(sprite.population-50)-6;
            sprite.radius = 3000/(1+Math.exp(-scale)) + 1000;
            sprite.time = time;
            time = time + 1;
            // sprite.radius = sprite.population
            map.addMapItem(sprite);
        }
    }
}

function removeMapElements() {
    //remove entire map
    map.clearMapItems()
}

function createSprites(){
    // delete Sprites van vorige keer: 1e 2 moeten we laten zitten
    removeMapElements();
    // console.log("DEBUG_1: ");

    controller.print();
    // console.log("DEBUG_2: ");
    listSprites = controller.getLocations();
    // console.log("DEBUG_3: ");

    // Lijst vullen
    createSpriteObjects(listSprites);
    // console.log("DEBUG_4: ");
    // Change map so all Sprites are shown
    map.fitViewportToMapItems();
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
                   "Ill: " + listSprites[i].radius + "\n" +
                   "\n";
       }
       allText.text = tekst;

    }

}
