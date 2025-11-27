#include "EntityManager.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Scene.h"
#include "Log.h"
#include "Item.h"
#include "Enemy.h"       // Asegúrate de incluir esto
#include "Projectile.h"  // Asegúrate de incluir esto

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

// ESTA ES LA VERSIÓN NUEVA (Asegúrate de que no haya otra función igual en este archivo)
void EntityManager::DestroyEntity(std::shared_ptr<Entity> entity)
{
    // En lugar de borrar inmediatamente, lo añadimos a la cola
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

// ESTA ES LA FUNCIÓN QUE FALTABA
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
    // Recorre la lista y destruye todo lo que NO sea el jugador
    for (const auto& entity : entities)
    {
        if (entity->type != EntityType::PLAYER)
        {
            DestroyEntity(entity);
        }
    }
}