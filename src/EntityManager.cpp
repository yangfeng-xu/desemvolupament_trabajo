#include "EntityManager.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Scene.h"
#include "Log.h"
#include "Item.h"
#include "Enemy.h"     
#include "Projectile.h"  

EntityManager::EntityManager() : Module()
{
    name = "entitymanager";
}

EntityManager::~EntityManager()
{
}

bool EntityManager::Awake()
{
    LOG("Loading Entity Manager");
    bool ret = true;
    for (const auto entity : entities)
    {
        if (entity->active == false) continue;
        ret = entity->Awake();
    }
    return ret;
}

bool EntityManager::Start() {
    bool ret = true;
    for (const auto entity : entities)
    {
        if (entity->active == false) continue;
        ret = entity->Start();
    }
    return ret;
}

bool EntityManager::CleanUp()
{
    bool ret = true;
    for (const auto entity : entities)
    {
        ret = entity->CleanUp();
    }
    entities.clear();
    entitiesToDestroy.clear(); // Limpiamos también la lista de pendientes
    return ret;
}

std::shared_ptr<Entity> EntityManager::CreateEntity(EntityType type)
{
    std::shared_ptr<Entity> entity = nullptr;

    switch (type)
    {
    case EntityType::PLAYER:
        entity = std::make_shared<Player>();
        break;
    case EntityType::ITEM:
        entity = std::make_shared<Item>();
        break;
    case EntityType::ENEMY:
        entity = std::make_shared<Enemy>();
        break;
    case EntityType::PROJECTILE:
        entity = std::make_shared<Projectile>();
        break;
    default:
        break;
    }

    if (entity != nullptr) {
        entities.push_back(entity);
    }

    return entity;
}

void EntityManager::DestroyEntity(std::shared_ptr<Entity> entity)
{
    // Evitar duplicados
    for (const auto& e : entitiesToDestroy) {
        if (e == entity) return;
    }
    entitiesToDestroy.push_back(entity);
}


void EntityManager::AddEntity(std::shared_ptr<Entity> entity)
{
    if (entity != nullptr) entities.push_back(entity);
}

bool EntityManager::Update(float dt)
{
    bool ret = true;
    for (const auto entity : entities)
    {
        if (entity->active == false) continue;
        ret = entity->Update(dt);
    }
    return ret;
}

bool EntityManager::PostUpdate()
{
    // Borramos las entidades pendientes de forma segura
    for (const auto& entity : entitiesToDestroy)
    {
        // 1. Limpiamos sus recursos internos
        entity->CleanUp();

        // 2. La quitamos de la lista principal
        entities.remove(entity);
    }

    // 3. Vaciamos la lista de pendientes
    entitiesToDestroy.clear();

    return true;
}

void EntityManager::DestroyEntitiesForReload()
{
    std::vector<std::shared_ptr<Entity>> entitiesToKill;

    // Identificar entidades que no sean el jugador
    for (const auto& entity : entities)
    {
        if (entity->type != EntityType::PLAYER)
        {
            entitiesToKill.push_back(entity);
        }
    }

    // Marcarlas para destruir
    for (const auto& entity : entitiesToKill)
    {
        DestroyEntity(entity);
    }
}