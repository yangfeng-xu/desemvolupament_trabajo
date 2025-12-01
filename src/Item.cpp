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
#include "Animation.h" // AÑADIDO
#include "Map.h"

Item::Item() : Entity(EntityType::ITEM) // Se crea inicialmente como ITEM
{
    name = "item";
    pbody = nullptr;

    texture = nullptr;
    texW = 0;
    texH = 0;
    isPicked = false;
    texturePath = nullptr;
    texH_offset = 0;
}

Item::~Item() {}

bool Item::Awake() {
    return true;
}

bool Item::Start() {

    if (pbody != nullptr) return true;
    position = startPosition;

    // 1. Detección de tipo y carga de ASSETS/PHYSICS
    if (type == EntityType::SAVEPOINT) // Si Map::LoadEntities lo marcó como SAVEPOINT
    {
        // ASSETS y ANIMACIÓN del Savepoint
        texture = Engine::GetInstance().textures->Load("Assets/Textures/savepoint_5espaciado.png");

        std::unordered_map<int, std::string> animNames = { {0,"save"} };
        anims.LoadFromTSX("Assets/Textures/savepoint_5espaciado.tsx", animNames);
        anims.SetCurrent("save");

        // DIMENSIONES
        texW = 32;
        texH = 32;

        // PHYSICS (Sensor estático)
        // La posición 'position' (startPosition) ya viene ajustada para el dibujo (esquina superior izquierda).
        // El centro para Box2D es la posición de dibujo + la mitad de las dimensiones.
        pbody = Engine::GetInstance().physics->CreateRectangleSensor(
            (int)position.getX() + texW / 2,
            (int)position.getY() + texH / 2,
            texW, texH, bodyType::STATIC);

        pbody->ctype = ColliderType::SAVEPOINT;

        // No necesita listener.
    }
    else // Es una moneda (ITEM por defecto)
    {
        // ASSETS de la Moneda
        texture = Engine::GetInstance().textures->Load("Assets/Textures/goldCoin.png");

        // DIMENSIONES
        Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);

        // PHYSICS (Cuerpo dinámico para caer)
        pbody = Engine::GetInstance().physics->CreateCircle(
            (int)position.getX() + texH / 2,
            (int)position.getY() + texH / 2,
            texH / 2, bodyType::DYNAMIC);

        pbody->ctype = ColliderType::ITEM;

        // LISTENER para detectar colisión con el jugador
        pbody->listener = this;
    }

    return true;
}

bool Item::Update(float dt)
{
    if (!active) return true;

    int x, y;
    SDL_Rect* srcRect = NULL;

    if (pbody != nullptr)
    {
        // 1. Obtener la posición física
        pbody->GetPosition(x, y);

        // 2. Lógica específica para SAVEPOINT
        if (type == EntityType::SAVEPOINT)
        {
            // Actualizar animación
            anims.Update(dt);
            srcRect = (SDL_Rect*)&anims.GetCurrentFrame();

            // La posición (x, y) de Box2D es el centro, ajustamos a la esquina superior izquierda
            x -= texW / 2;
            y -= texH / 2;

            // La posición del Savepoint se actualizó en Map::LoadEntities con la corrección de TMX (y - height),
            // por lo que position ahora es la esquina superior izquierda. La actualización del pbody
            // solo es necesaria si fuera dinámico, pero aquí la usamos para obtener la posición si se moviera.
        }
        else // Es una moneda (ITEM)
        {
            // La moneda no tiene animación por defecto, solo actualiza la posición
            position.setX((float)x);
            position.setY((float)y);

            // Ajuste de centro de círculo a esquina superior izquierda para dibujo
            x -= texW / 2;
            y -= texH / 2;
        }
    }

    // Dibujar la textura (con o sin frame de animación)
    Engine::GetInstance().render->DrawTexture(texture, x, y, srcRect);

    return true;
}

bool Item::CleanUp()
{
    // Limpieza de recursos
    if (texture) {
        Engine::GetInstance().textures->UnLoad(texture);
    }
    if (pbody) {
        Engine::GetInstance().physics->DeletePhysBody(pbody);
    }
    return true;
}

bool Item::Destroy()
{
    LOG("Destroying item");
    Disable();
    return true;
}