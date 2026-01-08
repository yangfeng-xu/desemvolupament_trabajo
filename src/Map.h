#pragma once

#include "Module.h"
#include <list>
#include <vector>
#include "Player.h"

// L09: TODO 5: Add attributes to the property structure
struct Properties
{
    struct Property
    {
        std::string name;
        bool value = false;

    };
    std::list<Property*>propertiesList;
    // L09: TODO 7: Implement a method to get the value of a custom property
    Property* GetProperty(const char* name) {
        for (const auto& property : propertiesList) {
            if (property->name == name) return property;
        }
        return NULL;
    }
};

struct MapLayer
{
    // L07: TODO 1: Add the info to the MapLayer Struct
    int id=0;
    std::string name;
    int width=0;
    int height=0;
    std::vector<int> tiles;
    Properties properties;
    // L07: TODO 6: Short function to get the gid value of i,j
    unsigned int Get(int i, int j) const
    {
        return tiles[(j * width) + i];
    }
};

// L06: TODO 2: Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!

struct TileSet
{
    int firstGid=0;
    std::string name;
    int tileWidth=0;
    int tileHeight=0;
    int spacing=0;
    int margin=0;
    int tileCount=0;
    int columns=0;
    SDL_Texture* texture=nullptr;

    // L07: TODO 7: Implement the method that receives the gid and returns a Rect
    SDL_Rect GetRect(unsigned int gid) {
        SDL_Rect rect = { 0 };

        int relativeIndex = gid - firstGid;
        rect.w = tileWidth;
        rect.h = tileHeight;
        rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
        rect.y = margin + (tileHeight + spacing) * (relativeIndex / columns);

        return rect;
    }

};

// L06: TODO 1: Create a struct needed to hold the information to Map node
struct MapData
{
    int width = 0;
    int height = 0;
    int tileWidth = 0;
    int tileHeight = 0;
    std::list<TileSet*> tilesets;

    // L07: TODO 2: Add the info to the MapLayer Struct
    std::list<MapLayer*> layers;
};

class Map : public Module
{
public:

    Map();

    // Destructor
    virtual ~Map();

    // Called before render is available
    bool Awake();

    // Called before the first frame
    bool Start();

    // Called each loop iteration
    bool Update(float dt);

    // Called before quitting
    bool CleanUp();

    // Load new map
    bool Load(std::string path, std::string mapFileName);

    // L07: TODO 8: Create a method that translates x,y coordinates from map positions to world positions
    Vector2D MapToWorld(int i, int j) const;
    Vector2D WorldToMap(int x, int y);
    // L09: TODO 2: Implement function to the Tileset based on a tile id
    TileSet* GetTilesetFromTileId(int gid) const;

    // L09: TODO 6: Load a group of properties 
    bool LoadProperties(pugi::xml_node& node, Properties& properties);

    Vector2D GetMapSizeInPixels();
    Vector2D GetMapSizeInTiles();

    MapLayer* GetNavigationLayer();

    int GetTileWidth() {
        return mapData.tileWidth;
    }

    int GetTileHeight() {
        return mapData.tileHeight;
    }

    void LoadEntities(std::shared_ptr<Player>& player);
    void SaveEntities(std::shared_ptr<Player>player);
public:
    std::string mapFileName;
    std::string mapPath;
    SDL_Texture* texture = NULL;

private:
    bool mapLoaded;
    AnimationSet anims;
    // L06: DONE 1: Declare a variable data of the struct MapData
    MapData mapData;
    pugi::xml_document mapFileXML;
    SDL_Texture* savepointTexture = nullptr; // Textura
    Vector2D savepointDrawPosition = { -1.0f, -1.0f };
    std::list<PhysBody*> mapBodies;
};