#pragma once

#include "Entity.h"
#include "Animation.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include "Pathfinding.h"
#include "Player.h"
#include <unordered_map>
#include <string>

struct SDL_Texture;

// Finite State Machine (FSM) for the Boss logic
enum class BossState {
	IDLE_CHASE,     
	ATTACK_NORMAL, 
	SKILL_PRE,     
	SKILL_MID,     
	SKILL_END     
};

enum class EnemyType {
	GROUND,
	FLYING,
	BOSS
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
	std::unordered_map<std::string, SDL_Texture*> bossTextures;

	// === Boss Core Attributes ===
	int health = 10;
	int maxHealth = 10;
	float detectionRange = 15.0f; // Radius to start chasing the player
	float attackRange = 100.0f;// Radius to start attacking the player
private:

	// === Pathfinding Logic ===
	float pathfindingTimer = 0.0f;
	float pathfindingInterval = 0.5f;//calcular la ruta de pathfinding cada 0,5 segundo
	void PerformPathfinding(float dt);
	void GetPhysicsValues();

	// Movement Logic
	void Move();
	void MoveFlying(); 
	void MoveBoss();
	void ApplyPhysics();
	void Draw(float dt);

	// Boss AI Helper: Manages state transitions (Idle -> Attack -> Skill)
	void UpdateBossBehavior(float dt);
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
	std::shared_ptr<Pathfinding> pathfinding;// Reference to Pathfinding module
	bool isGrounded = false;
	bool isDead = false;
	int deathFxId = 0; // Sound Effect ID for death

	// === Boss Internal Variables ===
	BossState bossState = BossState::IDLE_CHASE;// Current State
	float normalAttackTimer = 0.0f;             // Timer for basic attacks
	float skillTimer = 0.0f;                    // Timer for special skill duration
	bool isAttacking = false;                   // Flag to lock movement during attacks

	
	// === Boss Attack Logic Control ===
	bool hasDealtDamage = false;// Prevents one attack from hitting multiple times
	float attackCooldownTimer = 0.0f;// Time until next attack
	float attackDurationTimer = 0.0f;// How long the hitbox is active
	bool nextAttackIsOne = true;// Toggle between Attack 1 and Attack 2
	float attackCooldown = 2.0f;// 2 seconds between attacks
	int attackDamage = 1;

	// === Boss Arena Limits ===
	// Prevents the boss from walking out of the battle area
	bool hasBattleArea = false; 
	int battleAreaMinX = 0;// Left boundary    
	int battleAreaMaxX = 0; // Right boundary

	//-----enemy flying
	float flyingAttackDistance = 120.0f; // 停下来攻击的距离 (玩家跳跃能打到的高度/距离)
	float shootCooldownTimer = 0.0f;     // 射击冷却计时器
	float shootCooldown = 2.0f;          // 每 2 秒发射一次能量球
	bool isPreparingAttack = false;      // 是否正在蓄力前摇
	float attackWarningTimer = 0.0f;     // 前摇计时器
	float attackWarningDuration = 0.5f;  // 停下来 0.5 秒后再发射
	void ShootEnergyBall(Vector2D targetPos); // 发射能量球的函数
};