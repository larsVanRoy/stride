var component;
var sprite;
var listSprites  // array => []  object => () !!

function createSpriteObjects()
{
    var time = 0;
    for (var i=0; i<listSprites.length; i++) {
        component = Qt.createComponent("location.qml");

        sprite = component.createObject( root, {});

        if (sprite === null) {
            // Error Handling
            console.log("Error creating object");
        }
        else
        {
            var c = "#" + (listSprites[i].ill * 100) + "FF0000"   // Rood toevoegen Let op 100% klopt nog niet
            var ill = 0.5-controller.GetIllDouble(listSprites[i].ID)*2/3
            if (ill < 0.0){
                ill = 0.0
            }
            sprite.color = Qt.hsla(0.35, 1, ill,0.75);
            sprite.center.longitude = listSprites[i].longitude;
            sprite.center.latitude = listSprites[i].latitude;
            sprite.population = listSprites[i].population;
            var scale = 12/(10000-50)*(sprite.population-50)-6;
            sprite.radius = 3000/(1+Math.exp(-scale)) + 1000;
            sprite.id = listSprites[i].ID;
            // sprite.radius = sprite.population
            map.addMapItem(sprite);
        }
    }

    for (var i = 0; i<listSprites.length; i++) {
        // console.log("Create sprite objects: ", listSprites[i].latitude)
        component = Qt.createComponent("location.qml");

        sprite = component.createObject( root, {});

        if (sprite === null) {
            // Error Handling
            console.log("Error creating object center sprite");
        }
        else
        {
            sprite.color = Qt.rgba(0, 254, 0, 0.8);
            sprite.center.longitude = listSprites[i].longitude;
            sprite.center.latitude = listSprites[i].latitude;
            sprite.population = listSprites[i].population;
            sprite.radius = 100;
            sprite.time = time;
            sprite.id = listSprites[i].ID;
            time = time + 1;
            map.addMapItem(sprite);
        }
    }
}

function removeMapElements() {
    //remove entire map
    for(var i = map.mapItems.length - 1; i >= 0; i--) {
        if(map.mapItems[i].objectName === "location"){
            map.removeMapItem(map.mapItems[i]);
        }
    }
}

function createSprites(){
    // Lijst vullen
    listSprites = controller.getLocations();
    createSpriteObjects(listSprites);
}

function initializeMap(){
    removeMapElements();
    createSprites();
    map.fitViewportToVisibleMapItems();
}

function refreshSprites() {
    for(var i = 0; i < map.mapItems.length; i++) {
        if(map.mapItems[i].objectName === "location"){
            var ill = 0.5-controller.GetIllDouble(map.mapItems[i].id)*2/3
            if (ill < 0.0){
                ill = 0.0
            }
            map.mapItems[i].color = Qt.hsla(0.35, 1, ill,0.75);
        }
    }
}


function showText() {
    //property alias tekst: sideBar.allText.tekst
    var tekst = "";
    if (listSprites.length !== 0) {
        // open sidebar
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
