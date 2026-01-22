#include "Item.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"
#include "Animation.h"
#include "Map.h"
#include "Player.h"

Item::Item() : Entity(EntityType::ITEM) // It is initially created as ITEM
{
    name = "item";
    pbody = nullptr;

    texture = nullptr;
    texW = 0;
    texH = 0;
    isPicked = false;
    texturePath = nullptr;
    texH_offset = 0;

    id = -1;
}

Item::~Item() {}

bool Item::Awake() {
    return true;
}

bool Item::Start() {

    isPicked = false;
    active = true;

    if (pbody != nullptr) return true;
    position = startPosition;

    // 1. Detection of ASSET/PHYSICS type and load
    if (type == EntityType::SAVEPOINT) 
    {
        // ASSETS and ANIMATION of the Savepoint
        texture = Engine::GetInstance().textures->Load("Assets/Textures/savepoint_5espaciado.png");

        std::unordered_map<int, std::string> animNames = { {0,"save"} };
        anims.LoadFromTSX("Assets/Textures/savepoint_5espaciado.tsx", animNames);
        anims.SetCurrent("save");

        // DIMENSIONS
        texW = 32;
        texH = 32;

        // PHYSICS
        pbody = Engine::GetInstance().physics->CreateRectangleSensor(
            (int)position.getX() + texW / 2,
            (int)position.getY() + texH / 2,
            texW, texH, bodyType::STATIC);

        pbody->ctype = ColliderType::SAVEPOINT;

    }

    else // COLLECTIBLE ITEMS
    {
        if (name == "Star") {
            isStar = true;
            texture = Engine::GetInstance().textures->Load("Assets/Textures/resized-star.png"); 
        }
        else {
            isCoin = true; // Por defecto
            texture = Engine::GetInstance().textures->Load("Assets/Textures/goldCoin.png");
        }

        // DIMENSIONS
        int radius = 16; // Radio set by default for security
        if (texture != nullptr) {
            Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);
            radius = texW / 2;
        }
        else {
            texW = 32; texH = 32; // Avoid crash if texture fails to load
        }

        // PHYSICS (Static sensor to prevent it from falling and allow passage)
        // We use STATIC to make it float in the air; if you want it to fall, use DYNAMIC
        pbody = Engine::GetInstance().physics->CreateCircleSensor(
            (int)position.getX() + radius,
            (int)position.getY() + radius,
            radius, bodyType::STATIC);

        pbody->ctype = ColliderType::ITEM;
        pbody->listener = this;
    }

    return true;
}

bool Item::Update(float dt)
{
    if (!active) return true;

    if (isPicked) {
        CleanUp(); 
        active = false;
        return true;
    }

    // 3. Initialize variables
    int x = (int)position.getX();
    int y = (int)position.getY();
    SDL_Rect* srcRect = NULL;

    if (pbody != nullptr)
    {
        // 1. Obtain the physical position
        pbody->GetPosition(x, y);

        // 2. Specific logic for SAVEPOINT
        if (type == EntityType::SAVEPOINT)
        {
            // Update animation
            anims.Update(dt);
            srcRect = (SDL_Rect*)&anims.GetCurrentFrame();
            x -= texW / 2;
            y -= texH / 2;
        }
        else // It is a coin (ITEM)
        {
            // The coin has no default animation, it only updates its position
            position.setX((float)x);
            position.setY((float)y);

            // Adjust circle center to top left corner for drawing
            x -= texW / 2;
            y -= texH / 2;
        }
    }

    // Draw the texture (with or without an animation frame)
    Engine::GetInstance().render->DrawTexture(texture, x, y, srcRect);

    return true;
}

bool Item::CleanUp()
{
    // Resource cleanup
    if (pbody) {
        pbody->listener = nullptr;
        Engine::GetInstance().physics->DeletePhysBody(pbody);
        pbody = nullptr;
    }
    return true;
}

bool Item::Destroy()
{
    LOG("Destroying item");
    Disable();
    return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB) {

    //If it's already been collected, we don't do anything.
    if (isPicked) return;

    // If it collides with the PLAYER
    if (physB->ctype == ColliderType::PLAYER)
    {
        LOG("Item collected: %s", name.c_str());

        // 1. Mark as collected
        isPicked = true;

        Player* player = (Player*)physB->listener; // We get the pointer to the player
        if (player != nullptr) {
            if (isStar) {
                player->score += 100; // Add Points
                LOG("Star collected! Score: %d", player->score);

                // Star sound
                Engine::GetInstance().audio->PlayFx(Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/star_collection.wav"));
            }
            else if (isCoin) {
                player->ammo += 1;   // Add Bullets
                LOG("Coin collected! Ammo: %d", player->ammo);

                // Coin sound
                Engine::GetInstance().audio->PlayFx(Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/coin-collision-sound-342335.wav"));
            }
        }

        if (id != -1) {
            Engine::GetInstance().scene->collectedIDs.push_back(id);
        }

        // 2. Play sound
        if (isCoin) {
            Engine::GetInstance().audio->PlayFx(Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/coin-collision-sound-342335.wav"));
        }
        else if (isStar) {
            Engine::GetInstance().audio->PlayFx(Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/star_collection.wav"));
        }
    }
}