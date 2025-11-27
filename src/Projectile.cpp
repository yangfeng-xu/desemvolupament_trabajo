#include "Projectile.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Physics.h"
#include"EntityManager.h"

Projectile::Projectile() : Entity(EntityType::PROJECTILE) {
    name = "projectile";
    pbody = nullptr;
    texture = nullptr;
    texW = 0;
    texH = 0;
    velocity = Vector2D(0, 0);
}

Projectile::~Projectile() {}

bool Projectile::Awake() { return true; }

bool Projectile::Start() {
    // Carga tu textura de bala aquí. Si no tienes, usa una temporal o reutiliza "goldCoin.png"
    texture = Engine::GetInstance().textures->Load("Assets/Textures/goldCoin.png");

    texW = 16; texH = 16;

    // Creamos un Sensor para que detecte colisión pero no empuje físicamente a los enemigos
    pbody = Engine::GetInstance().physics->CreateRectangleSensor((int)position.getX(), (int)position.getY(), texW, texH, bodyType::DYNAMIC);
    pbody->ctype = ColliderType::PROJECTILE;
    pbody->listener = this;

    // Desactiva gravedad para la bala
    b2Body_SetGravityScale(pbody->body, 0.0f);

    // Aplica la velocidad inicial
    Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity.getX(), velocity.getY());

    return true;
}

bool Projectile::Update(float dt) {
    if (!active) return true;

    // Actualizar vida útil
    lifeTime -= dt;
    if (lifeTime <= 0) {
        Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
        return true;
    }

    // Sincronizar posición visual con física
    if (pbody != nullptr) {
        int x, y;
        pbody->GetPosition(x, y);
        position.setX((float)x);
        position.setY((float)y);
    }

    // Dibujar
    Engine::GetInstance().render->DrawTexture(texture, (int)position.getX() - texW / 2, (int)position.getY() - texH / 2);
    return true;
}

bool Projectile::CleanUp() {
    if (pbody != nullptr) {
        Engine::GetInstance().physics->DeletePhysBody(pbody);
        pbody = nullptr; // Importante: Marcarlo como nulo después de borrarlo
    }
    return true;
}

void Projectile::SetPosition(Vector2D pos) {
    position = pos;
}

void Projectile::SetVelocity(Vector2D vel) {
    velocity = vel * speed;
}

void Projectile::OnCollision(PhysBody* physA, PhysBody* physB) {
    switch (physB->ctype) {
    case ColliderType::PLATFORM:
    case ColliderType::ENEMY:
        // Destruir bala al chocar
        Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
        break;
    }
}