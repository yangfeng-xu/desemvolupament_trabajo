#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Log.h"
#include "Physics.h"
#include"EntityManager.h"
#include <math.h>
#include "Item.h" 
#include "Scene.h"

Map::Map() : Module(), mapLoaded(false)
{
  
    name = "map";
}

// Destructor
Map::~Map()
{
}

// Called before render is available
bool Map::Awake()
{
    name = "map";
    LOG("Loading Map Parser");

    return true;
}

bool Map::Start() {

    return true;
}

bool Map::Update(float dt)
{
    ZoneScoped;
    bool ret = true;

    if (mapLoaded) {

        // L07 TODO 5: Prepare the loop to draw all tiles in a layer + DrawTexture
       // iterate all tiles in a layer
        for (const auto& mapLayer : mapData.layers) {
            if (mapLayer->properties.GetProperty("Draw") != NULL && mapLayer->properties.GetProperty("Draw")->value) {
                for (int i = 0; i < mapData.width; i++) {
                    for (int j = 0; j < mapData.height; j++) {
                        // L07 TODO 9: Complete the draw function

                        //Get the gid from tile
                        int gid = mapLayer->Get(i, j);

                        //Check if the gid is different from 0 - some tiles are empty
                        if (gid != 0) {
                            //L09: TODO 3: Obtain the tile set using GetTilesetFromTileId
                            TileSet* tileSet = GetTilesetFromTileId(gid);
                            if (tileSet != nullptr) {
                                //Get the Rect from the tileSetTexture;
                                SDL_Rect tileRect = tileSet->GetRect(gid);
                                //Get the screen coordinates from the tile coordinates
                                Vector2D mapCoord = MapToWorld(i, j);
                                //Draw the texture
                                Engine::GetInstance().render->DrawTexture(tileSet->texture, (int)mapCoord.getX(), (int)mapCoord.getY(), &tileRect);
                            }
                        }
                    }
                }
            }
        }

       
    }


    return ret;
}

// L09: TODO 2: Implement function to the Tileset based on a tile id
TileSet* Map::GetTilesetFromTileId(int gid) const
{
    TileSet* set = nullptr;
    for (const auto& tileset : mapData.tilesets) {
        set = tileset;
        if (gid >= tileset->firstGid && gid < (tileset->firstGid + tileset->tileCount)) {
            break;
        }
    }
    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // L06: TODO 2: Make sure you clean up any memory allocated from tilesets/map
    for (const auto& tileset : mapData.tilesets) {
        delete tileset;
    }
    mapData.tilesets.clear();

    // L07 TODO 2: clean up all layer data
    for (const auto& layer : mapData.layers)
    {
        delete layer;
    }
    mapData.layers.clear();

    // REMOVED: Cleanup of savepointTexture and animatics
    // The Map module no longer has these assets.
    for (const auto& body : mapBodies) {
        Engine::GetInstance().physics->DeletePhysBody(body);

    }
    mapBodies.clear();
    return true;
}

// Load new map
bool Map::Load(std::string path, std::string fileName)
{
   
    bool ret = false;

    // Assigns the name of the map file and the path
    mapFileName = fileName;
    mapPath = path;
    std::string mapPathName = mapPath + mapFileName;

    pugi::xml_parse_result result = mapFileXML.load_file(mapPathName.c_str());

    if (result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
        ret = false;
    }
    else {

        // L06: TODO 3: Implement LoadMap to load the map properties
        // retrieve the paremeters of the <map> node and store the into the mapData struct
        mapData.width = mapFileXML.child("map").attribute("width").as_int();
        mapData.height = mapFileXML.child("map").attribute("height").as_int();
        mapData.tileWidth = mapFileXML.child("map").attribute("tilewidth").as_int();
        mapData.tileHeight = mapFileXML.child("map").attribute("tileheight").as_int();

        // L06: TODO 4: Implement the LoadTileSet function to load the tileset properties

        //Iterate the Tileset
        for (pugi::xml_node tilesetNode = mapFileXML.child("map").child("tileset"); tilesetNode != NULL; tilesetNode = tilesetNode.next_sibling("tileset"))
        {
            //Load Tileset attributes
            TileSet* tileSet = new TileSet();
            tileSet->firstGid = tilesetNode.attribute("firstgid").as_int();
            tileSet->name = tilesetNode.attribute("name").as_string();
            tileSet->tileWidth = tilesetNode.attribute("tilewidth").as_int();
            tileSet->tileHeight = tilesetNode.attribute("tileheight").as_int();
            tileSet->spacing = tilesetNode.attribute("spacing").as_int();
            tileSet->margin = tilesetNode.attribute("margin").as_int();
            tileSet->tileCount = tilesetNode.attribute("tilecount").as_int();
            tileSet->columns = tilesetNode.attribute("columns").as_int();

            //Load the tileset image
            std::string imgName = tilesetNode.child("image").attribute("source").as_string();
            tileSet->texture = Engine::GetInstance().textures->Load((mapPath + imgName).c_str());

            mapData.tilesets.push_back(tileSet);
        }

        // L07: TODO 3: Iterate all layers in the TMX and load each of them
        for (pugi::xml_node layerNode = mapFileXML.child("map").child("layer"); layerNode != NULL; layerNode = layerNode.next_sibling("layer")) {

            // L07: TODO 4: Implement the load of a single layer 
            //Load the attributes and saved in a new MapLayer
            MapLayer* mapLayer = new MapLayer();
            mapLayer->id = layerNode.attribute("id").as_int();
            mapLayer->name = layerNode.attribute("name").as_string();
            mapLayer->width = layerNode.attribute("width").as_int();
            mapLayer->height = layerNode.attribute("height").as_int();

            //L09: TODO 6 Call Load Layer Properties
            LoadProperties(layerNode, mapLayer->properties);
            //Iterate over all the tiles and assign the values in the data array
            for (pugi::xml_node tileNode = layerNode.child("data").child("tile"); tileNode != NULL; tileNode = tileNode.next_sibling("tile")) {
                mapLayer->tiles.push_back(tileNode.attribute("gid").as_int());
            }

            //add the layer to the map
            mapData.layers.push_back(mapLayer);
        }

        // L08 TODO 3: Create colliders
        // L08 TODO 7: Assign collider type

        for (const auto& mapLayer : mapData.layers) {
            if (mapLayer->name == "Collition") {
                LOG("Generado coliciones para la capa:%s", mapLayer->name.c_str());
                for (int i = 0; i < mapData.width; i++) {
                    for (int j = 0; j < mapData.height; j++) {
                        int gid = mapLayer->Get(i, j);// We obtain the IDs of the four we have painted in red to identify collisions, from where
                        if (gid == 49) {
                            Vector2D pos = MapToWorld(i, j);// pixel map
                            PhysBody* colliderBody = Engine::GetInstance().physics.get()->CreateRectangle((int)pos.getX() + mapData.tileWidth / 2,
                                (int)pos.getY() + mapData.tileHeight / 2, mapData.tileWidth, mapData.tileHeight, STATIC);
                            colliderBody->ctype = ColliderType::PLATFORM;// To say that it's a platform type where the player is playing on itself
                            mapBodies.push_back(colliderBody);
                        }
                        else if (gid == 50) {
                            Vector2D pos = MapToWorld(i, j);
                            PhysBody* colliderBody = Engine::GetInstance().physics.get()->CreateRectangle((int)pos.getX() + mapData.tileWidth / 2,
                                (int)pos.getY() + mapData.tileHeight / 2, mapData.tileWidth, mapData.tileHeight, STATIC);
                            colliderBody->ctype = ColliderType::DEATH;
                            mapBodies.push_back(colliderBody);
                        }

                    }
                }
                break;
            }
        }
        ret = true;

        // L06: TODO 5: LOG all the data loaded iterate all tilesetsand LOG everything
        if (ret == true)
        {
            LOG("Successfully parsed map XML file :%s", fileName.c_str());
            LOG("width : %d height : %d", mapData.width, mapData.height);
            LOG("tile_width : %d tile_height : %d", mapData.tileWidth, mapData.tileHeight);
            LOG("Tilesets----");

            //iterate the tilesets
            for (const auto& tileset : mapData.tilesets) {
                LOG("name : %s firstgid : %d", tileset->name.c_str(), tileset->firstGid);
                LOG("tile width : %d tile height : %d", tileset->tileWidth, tileset->tileHeight);
                LOG("spacing : %d margin : %d", tileset->spacing, tileset->margin);
            }

            LOG("Layers----");

            for (const auto& layer : mapData.layers) {
                LOG("id : %d name : %s", layer->id, layer->name.c_str());
                LOG("Layer width : %d Layer height : %d", layer->width, layer->height);
            }
        }
        else {
            LOG("Error while parsing map file: %s", mapPathName.c_str());
        }



    }

    mapLoaded = ret;
    return ret;
}

// L07: TODO 8: Create a method that translates x,y coordinates from map positions to world positions
Vector2D Map::MapToWorld(int i, int j) const
{
    Vector2D ret;

    ret.setX((float)(i * mapData.tileWidth));
    ret.setY((float)(j * mapData.tileHeight));

    return ret;
}

Vector2D Map::WorldToMap(int x, int y) {

    Vector2D ret(0, 0);
    ret.setX((float)(x / mapData.tileWidth));
    ret.setY((float)(y / mapData.tileHeight));

    return ret;
}

// L09: TODO 6: Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{

    bool ret = true;
    for (pugi::xml_node propertiesNode = node.child("properties").child("property"); propertiesNode != NULL; propertiesNode = propertiesNode.next_sibling("property")) {
        Properties::Property* p = new Properties::Property;
        p->name = propertiesNode.attribute("name").as_string();
        p->value = propertiesNode.attribute("value").as_bool();
        properties.propertiesList.push_back(p);
    }
    return ret;
}

Vector2D Map::GetMapSizeInPixels()
{
    Vector2D sizeInPixels;
    sizeInPixels.setX((float)(mapData.width * mapData.tileWidth));
    sizeInPixels.setY((float)(mapData.height * mapData.tileHeight));
    return sizeInPixels;

}

Vector2D Map::GetMapSizeInTiles()
{
    return Vector2D((float)mapData.width, (float)mapData.height);
}

// Method to get the navigation layer from the map
MapLayer* Map::GetNavigationLayer() {
    for (const auto& layer : mapData.layers) {
        if (layer->properties.GetProperty("Navigation") != NULL &&
            layer->properties.GetProperty("Navigation")->value) {
            return layer;
        }
    }

    return nullptr;
}


void Map::LoadEntities(std::shared_ptr<Player>& player) {

    Engine::GetInstance().entityManager->DestroyEntitiesForReload();

    for (pugi::xml_node objectGroupNode = mapFileXML.child("map").child("objectgroup");
        objectGroupNode != NULL;
        objectGroupNode = objectGroupNode.next_sibling("objectgroup")) {

        if (objectGroupNode.attribute("name").as_string() == std::string("Entities")) {
            for (pugi::xml_node objectNode = objectGroupNode.child("object");
                objectNode != NULL;
                objectNode = objectNode.next_sibling("object")) {

                std::string entityType = objectNode.attribute("type").as_string();
                std::string name = objectNode.attribute("name").as_string();

                // LEER EL ID AQUÍ PARA TODOS (Es común a todos los objetos de Tiled)
                int id = objectNode.attribute("id").as_int();

                // ---------------- PLAYER ----------------
                if (entityType == std::string("Player")) {
                    float x = objectNode.attribute("x").as_float();
                    float y = objectNode.attribute("y").as_float();
                    if (player == NULL) {
                        player = std::dynamic_pointer_cast<Player>(Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER));
                        player->position = Vector2D(x, y);
                        player->id = id; // Asignar ID
                        player->Start();
                    }
                    else {
                        player->SetPosition(Vector2D(x, y));
                        player->id = id; // Asignar ID
                    }
                }

                // ---------------- ENEMY ----------------
                else if (entityType == std::string("Enemy")) {
                    // 1. Verificar si este enemigo ya fue matado
                    bool isDead = false;
                    for (int deadId : Engine::GetInstance().scene->deadEnemyIDs) {
                        if (deadId == id) {
                            isDead = true;
                            break;
                        }
                    }

                    if (isDead) {
                        LOG("Enemy ID %d is dead. Skipping creation.", id);
                        continue; // Si está muerto, no lo creamos
                    }

                    float x = objectNode.attribute("x").as_float();
                    float y = objectNode.attribute("y").as_float();

                    std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));

                    enemy->position = Vector2D(x, y);
                    enemy->id = id; // --- ¡IMPORTANTE! ASIGNAR EL ID ---

                    if (name == "EnemyGround") {
                        enemy->SetEnemyType(EnemyType::GROUND);
                    }
                    else if (name == "EnemyFlying") {
                        enemy->SetEnemyType(EnemyType::FLYING);
                    }
                    else {
                        enemy->SetEnemyType(EnemyType::GROUND);
                    }
                    enemy->Awake();
                    enemy->Start();
                }

                // ---------------- BOSS ----------------
                else if (entityType == std::string("Boss")) {
                    float x = objectNode.attribute("x").as_float();
                    float y = objectNode.attribute("y").as_float();
                    std::shared_ptr<Enemy> boss = std::dynamic_pointer_cast<Enemy>(
                        Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY)
                    );

                    boss->position = Vector2D(x, y);
                    boss->id = id; // --- ¡IMPORTANTE! ASIGNAR EL ID ---
                    boss->SetEnemyType(EnemyType::BOSS);
                    boss->Awake();
                    boss->Start();

                    LOG("Created BOSS at x:%.2f y:%.2f with ID: %d", x, y, id);
                }

                // ---------------- SAVEPOINT ----------------
                else if (name == "Save" && entityType == "Map") {
                    float x = objectNode.attribute("x").as_float();
                    float y = objectNode.attribute("y").as_float();

                    // 1. Crear la entidad como ITEM (ya que usas Item para el savepoint)
                    std::shared_ptr<Item> savepoint = std::dynamic_pointer_cast<Item>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));

                    // 2. Configurar tipo y posición
                    savepoint->type = EntityType::SAVEPOINT; // ¡Importante!
                    savepoint->position = Vector2D(x, y - mapData.tileHeight); // Ajuste de coordenada Y de Tiled
                    savepoint->startPosition = savepoint->position;

                    // 3. Inicializar
                    savepoint->Awake();
                    savepoint->Start();

                    LOG("Savepoint created correctly at x:%.2f y:%.2f", x, y);
                }

                // ---------------- ITEM ----------------
                else if (entityType == std::string("Item")) {

                    // Lógica para verificar si ya fue recogido
                    bool foundInList = false;
                    for (int collectedId : Engine::GetInstance().scene->collectedIDs) {
                        if (collectedId == id) {
                            foundInList = true;
                            break;
                        }
                    }

                    if (foundInList) {
                        continue; // Si ya lo tenemos, saltar creación
                    }

                    float x = objectNode.attribute("x").as_float();
                    float y = objectNode.attribute("y").as_float();
                    std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));

                    item->id = id; // Ya lo tenías, ahora funcionará porque está en Entity.h
                    item->name = name;
                    item->position = Vector2D(x, y - mapData.tileHeight);
                    item->startPosition = item->position;
                    item->Awake();
                    item->Start();
                }
            }
        }
    }
}

void Map::SaveEntities(std::shared_ptr<Player>player) {

    for (pugi::xml_node objectGroupNode = mapFileXML.child("map").child("objectgroup");
        objectGroupNode != NULL;
        objectGroupNode = objectGroupNode.next_sibling("objectgroup")) {
        if (objectGroupNode.attribute("name").as_string() == std::string("Entities")) {
            for (pugi::xml_node objectNode = objectGroupNode.child("object");
                objectNode != NULL;
                objectNode = objectNode.next_sibling("object")) {
                std::string entityType = objectNode.attribute("type").as_string();
                //Read or modify the Player information
                if (entityType == std::string("Player")) {
                    Vector2D playerPos = player->GetPosition();
                    objectNode.attribute("x").set_value(playerPos.getX());
                    objectNode.attribute("y").set_value(playerPos.getY());

                }
            }
        }
    }
}