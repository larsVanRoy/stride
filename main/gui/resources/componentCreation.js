var component;
var sprite;
var listSprites;

function createSpriteObjects()
{
    for (var i=0; i<listSprites.length; i++) {
        component = Qt.createComponent("location.qml");

        sprite = component.createObject( root, {});

        if (sprite === null) {
            // Error Handling
            console.log("Error creating object");
        }
        else
        {
            var c = 0.5-controller.GetColor(listSprites[i].ID)
            if (c < 0.0){
                c = 0.0;
            }
            else if(c > 1.0){
                c = 1.0;
            }
            sprite.color = Qt.hsla(0.35, 1, c,0.75);
            sprite.center.longitude = listSprites[i].longitude;
            sprite.center.latitude = listSprites[i].latitude;
            sprite.population = listSprites[i].population;
            var scale = 12/(10000-50)*(sprite.population-50)-6; //rescale size
            sprite.radius = 3000/(1+Math.exp(-scale)) + 1000;
            sprite.id = listSprites[i].ID;
            map.addMapItem(sprite);
        }
    }

    for (var i = 0; i<listSprites.length; i++) {
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
            sprite.id = listSprites[i].ID;
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
    // fill list
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
            var c = 0.5-controller.GetColor(map.mapItems[i].id);
            if (c < 0.0){
                c = 0.0
            }
            map.mapItems[i].color = Qt.hsla(0.35, 1, c,0.75);
        }
    }
}

function setSpritesInvisible() {
    for(var i = 0; i < map.mapItems.length; i++) {
        if(map.mapItems[i].objectName === "location"){
            map.mapItems[i].clickable = false;
        }
    }
}

function setSpritesVisible() {
    for(var i = 0; i < map.mapItems.length; i++) {
        if(map.mapItems[i].objectName === "location"){
            map.mapItems[i].clickable = true;
        }
    }
}
