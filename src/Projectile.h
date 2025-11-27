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
    void SetVelocity(Vector2D vel); // Para disparar izquierda o derecha
    void OnCollision(PhysBody* physA, PhysBody* physB);

public:
    PhysBody* pbody;
    Vector2D velocity;
    float speed = 10.0f;
    int lifeTime = 2000; // Milisegundos antes de desaparecer si no choca

private:
    SDL_Texture* texture;
    int texW, texH;
}; 

#pragma once
