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

enum class BossState {
	IDLE_CHASE,     // 空闲或追逐玩家
	ATTACK_NORMAL,  // 普通攻击中
	SKILL_PRE,      // 大招前摇
	SKILL_MID,      // 大招中段
	SKILL_END       // 大招后摇
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
	int deathFxId = 0; //ID del efecto de sonido de muerte del enemigo


	// === 新增：Boss 专用变量 ===
	BossState bossState = BossState::IDLE_CHASE; // 当前状态
	float normalAttackTimer = 0.0f;              // 普通攻击计时器
	float skillTimer = 0.0f;                     // 大招计时器
	bool isAttacking = false;                    // 是否正在执行攻击动作（用于阻止移动）

	// 辅助函数：处理 Boss 的逻辑
	void UpdateBossBehavior(float dt);


};