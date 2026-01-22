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
    // Load bullet texture
    texture = Engine::GetInstance().textures->Load("Assets/Textures/goldCoin.png");

    texW = 16; texH = 16;

    // We created a sensor that detects collisions but doesn't physically push enemies.
    pbody = Engine::GetInstance().physics->CreateRectangleSensor((int)position.getX(), (int)position.getY(), texW, texH, bodyType::DYNAMIC);
    pbody->ctype = ColliderType::PROJECTILE;
    pbody->listener = this;

    // Turn off gravity for the bullet
    b2Body_SetGravityScale(pbody->body, 0.0f);

    // Apply the initial velocity
    Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity.getX(), velocity.getY());

    return true;
}

bool Projectile::Update(float dt) {
    if (!active) return true;

    // Update lifespan
    lifeTime -= dt;
    if (lifeTime <= 0) {
        Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
        return true;
    }

    // Synchronize visual position with physics
    if (pbody != nullptr) {
        int x, y;
        pbody->GetPosition(x, y);
        position.setX((float)x);
        position.setY((float)y);
    }

    // Draw
    Engine::GetInstance().render->DrawTexture(texture, (int)position.getX() - texW / 2, (int)position.getY() - texH / 2);
    return true;
}

bool Projectile::CleanUp() {
    if (pbody != nullptr) {
        Engine::GetInstance().physics->DeletePhysBody(pbody);
        pbody = nullptr;
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
        // Destroy bullet on impact
        Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
        break;
    }
}