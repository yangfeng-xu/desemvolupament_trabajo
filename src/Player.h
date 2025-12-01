#pragma once

#include "Entity.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include"Animation.h"
struct SDL_Texture;

class Player : public Entity
{
public:

	Player();

	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	Vector2D GetPosition();
	void SetPosition(Vector2D pos);

	void Die();

private:

	void GetPhysicsValues();
	void Move();
	void Jump();
	void ApplyPhysics();
	void Draw();

public:

	//Declare player parameters
	float speed = 4.0f;
	SDL_Texture* texture = NULL;

	bool isGodMode = false;

	int texW = 0;
	int texH = 0;

	//Audio fx
	int pickCoinFxId = 0;
	int deathFxId = 0;
	int jumpFxId = 0; // NUEVO: ID del efecto de sonido de salto
	int saveFxId = 0;

	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody = nullptr;
	float jumpForce = 2.5f; // The force to apply when jumping
	bool isJumping = false; // Flag to check if the player is currently jumping
private:
	b2Vec2 velocity = { 0.0f,0.0f };
	AnimationSet anims;
	Vector2D startPosition;
	Vector2D savePosition;
	bool IsDead;
	float shootCooldown = 0.0f;
	SDL_FlipMode flipState = SDL_FLIP_NONE;//FlipMode para invertir imagen

};