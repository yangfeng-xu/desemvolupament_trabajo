#pragma once
#include "Entity.h"
#include "Box2D/box2d.h"

class Projectile : public Entity
{
public:
    Projectile();
    virtual ~Projectile();

    bool Awake();
    bool Start();
    bool Update(float dt);
    bool CleanUp();

    void SetPosition(Vector2D pos);
    void SetVelocity(Vector2D vel); // To shoot left or right
    void OnCollision(PhysBody* physA, PhysBody* physB);

public:
    PhysBody* pbody;
    Vector2D velocity;
    float speed = 10.0f;
    float lifeTime = 2000.0f; // Milliseconds before disappearing if it doesn't collide

private:
    SDL_Texture* texture = nullptr; // Direct initialization
    int texW = 0, texH = 0;
}; 
