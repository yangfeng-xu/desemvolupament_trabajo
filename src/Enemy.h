#pragma once

#include "Entity.h"
#include "Animation.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include "Pathfinding.h"
#include "Player.h"

struct SDL_Texture;

enum class EnemyType {
	GROUND,
	FLYING
};
class Enemy : public Entity
{
public:

	Enemy();
	virtual ~Enemy();
	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);
	void SetPosition(Vector2D pos);
	Vector2D GetPosition();
	void SetEnemyType(EnemyType type);
	SDL_FlipMode flipState = SDL_FLIP_NONE;

private:
	float pathfindingTimer = 0.0f;
	float pathfindingInterval = 0.5f;//calcular la ruta de pathfinding cada 0,5 segundo
	void PerformPathfinding(float dt);
	void GetPhysicsValues();
	void Move();
	void MoveFlying();
	void ApplyPhysics();
	void Draw(float dt);

public:

	//Declare enemy parameters
	float speed = 3.0f;
	SDL_Texture* texture = NULL;
	int texW, texH;
	PhysBody* pbody;

	Vector2D startPosition;
	float jumpForce = 8.0f;
	int detectionRadius = 10;
	EnemyType enemyType = EnemyType::GROUND;


private:
	b2Vec2 velocity;
	AnimationSet anims;
	std::shared_ptr<Pathfinding> pathfinding;
	bool isGrounded = false;
	bool isDead = false;
	int deathFxId = 0; // NUEVO: ID del efecto de sonido de muerte del enemigo
};