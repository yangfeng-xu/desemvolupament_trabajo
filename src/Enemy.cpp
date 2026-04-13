
#include "Enemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"
#include "Map.h"
#include "Player.h"
#include"Projectile.h"
#include "Scene.h"

Enemy::Enemy() : Entity(EntityType::ENEMY) {
	name = "Enemy";

	pbody = nullptr;
	pathfinding = nullptr;
	texture = nullptr;
	texW = 0;
	texH = 0;
	velocity = { 0.0f, 0.0f };
	isGrounded = false;
}

Enemy::~Enemy() {

}

bool Enemy::Awake() {
	return true;
}

void Enemy::SetEnemyType(EnemyType type) {
	enemyType = type;
}

bool Enemy::Start() {

	
	if (pbody != nullptr) return true;
	
	// Load textures based on the enemy type
	if (enemyType == EnemyType::BOSS) {
		LOG("Loading BOSS with Multiple Textures");

		// Load specific textures for Boss states
		bossTextures["idle"] = Engine::GetInstance().textures->Load("Assets/Textures/Bidle.png");
		bossTextures["attack1"] = Engine::GetInstance().textures->Load("Assets/Textures/Boss_Attack_1.png");
		bossTextures["attack2"] = Engine::GetInstance().textures->Load("Assets/Textures/Boss_Attack_2.png");
		bossTextures["walk"] = Engine::GetInstance().textures->Load("Assets/Textures/Boss_walk.png");
		bossTextures["hit"] = Engine::GetInstance().textures->Load("Assets/Textures/hit.png");
		bossTextures["Preattack3"] = Engine::GetInstance().textures->Load("Assets/Textures/Pre-Attack-3.png");
		bossTextures["Midattack3"] = Engine::GetInstance().textures->Load("Assets/Textures/mid-Attack-3.png");
		bossTextures["Endattack3"] = Engine::GetInstance().textures->Load("Assets/Textures/end-Attack-3.png");
		bossTextures["death"] = Engine::GetInstance().textures->Load("Assets/Textures/death.png");
		texture = bossTextures["idle"];

		//------------------------------------------------------------------------Boss Anim-------------------------------------------------------------------------------//
		Animation idleAnim;
		int frameWidth = 247;  // Width of each frame
		int frameHeight = 87;
		int frameCount1 = 14;
		int frameCount2 = 10;
		int frameCount3 = 9;
		int frameCount4 = 1;
		int frameCount5 = 4;
		int frameCount6 = 3;
		int frameCount7 = 7;
		int frameCount8 = 33;

		// Setup Idle Animation
		for (int i = 0; i < frameCount1; i++) {
			int currentX = i * frameWidth;
			idleAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("idle", idleAnim);

		// Setup Attack 1 Animation
		Animation attack1Anim;
		attack1Anim.SetLoop(false);
		for (int i = 0; i < frameCount2; i++) {
			int currentX = i * frameWidth;
			attack1Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 80);
		}
		anims.AddClip("attack1", attack1Anim);

		// Setup Attack 2 Animation
		Animation attack2Anim;
		attack2Anim.SetLoop(false); 
		for (int i = 0; i < frameCount3; i++) {
			int currentX = i * frameWidth;
			attack2Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 80);
		}
		anims.AddClip("attack2", attack2Anim);

		// Setup Pre-Attack 3 Animation
		Animation Preattack3Anim;
		Preattack3Anim.SetLoop(false); 
		for (int i = 0; i < frameCount6; i++) {
			int currentX = i * frameWidth;
			Preattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 80);
		}
		anims.AddClip("Preattack3", Preattack3Anim);

		// Setup Mid-Attack 3 Animation
		Animation Midattack3Anim;
		Midattack3Anim.SetLoop(false); 
		for (int i = 0; i < frameCount5; i++) {
			int currentX = i * frameWidth;
			Midattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 80);
		}
		anims.AddClip("Midattack3", Midattack3Anim);

		// Setup End-Attack 3 Animation
		Animation Endattack3Anim;
		Endattack3Anim.SetLoop(false); 
		for (int i = 0; i < frameCount7; i++) {
			int currentX = i * frameWidth;
			Endattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 80);
		}
		anims.AddClip("Endattack3", Endattack3Anim);


		// Walk Animation
		Animation WalkAnim;
		WalkAnim.SetLoop(true);
		for (int i = 0; i < frameCount1; i++) {
			int currentX = i * frameWidth;
			WalkAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("walk", WalkAnim);

		// Hit and Death Anim
		Animation HitAnim;
		HitAnim.SetLoop(false);
		for (int i = 0; i < frameCount4; i++) {
			int currentX = i * frameWidth;
			HitAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("hit", HitAnim);

		// Death Animation
		Animation DeathAnim;
		DeathAnim.SetLoop(false);
		for (int i = 0; i < frameCount8; i++) {
			int currentX = i * frameWidth;
			DeathAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("death", DeathAnim);

		// Set Initial State
		anims.SetCurrent("idle");

		//Creating a specific large collider for the Boss
		texW = 100; texH = 100;
		pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), 40, bodyType::DYNAMIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::ENEMY;
		b2Body_SetGravityScale(pbody->body, 1.0f);


		// ================== [New: Boss Independent Parameters] ==================
	    //1. Set a larger detection range
		detectionRadius = 15; 

		// 2. Optimization: Pathfinding calculation interval
		pathfindingInterval = 0.5f;

		// 3. Boss Movement Speed
		speed = 2.0f; 

		// Set Boss Health
		maxHealth = 20;  
		health = maxHealth;

		// Level 2 Boss Battle Arena
		hasBattleArea = true;   
		battleAreaMinX = 2048;   
		battleAreaMaxX = 2944;  
		

	}
	else if (enemyType == EnemyType::FLYING) {
		std::unordered_map<int, std::string>EnemyFlying = { {0,"idleflying"},{4,"fly-right"},{8,"fly-down"},{12,"fly-left"} };
		deathFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/monster-death.mp3"); // Load audio
		anims.LoadFromTSX("Assets/Textures/32x32-bat-sprite.tsx", EnemyFlying);
		anims.SetCurrent("idleflying");
		texture = Engine::GetInstance().textures->Load("Assets/Textures/32x32-bat-sprite.png");
	}
	else {
		std::unordered_map<int, std::string> aliases = { {13,"move"},{24,"idle"},{35,"death"} };
		deathFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/monster-death.wav"); // Load audio
		anims.LoadFromTSX("Assets/Textures/slime-Sheet2.tsx", aliases);
		anims.SetCurrent("idle");


		//Initialize Player parameters
		texture = Engine::GetInstance().textures->Load("Assets/Textures/slime-Sheet2.png");
	}


	if (pbody == nullptr) {
		//Add physics to the enemy - initialize physics body
		texW = 32;
		texH = 32;

		pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

		//Assign enemy class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
		pbody->listener = this;

		//ssign collider type
		pbody->ctype = ColliderType::ENEMY;

		if (enemyType == EnemyType::FLYING) {
			b2Body_SetGravityScale(pbody->body, 0.0f);
		}
		else {
			b2Body_SetGravityScale(pbody->body, 1.0f);
		}
	}

	// Inicializar pathfinding (igual que antes)
	pathfinding = std::make_shared<Pathfinding>();
	Vector2D pos = GetPosition();

	// Ensure Map module is loaded
	if (Engine::GetInstance().map != nullptr) {
		Vector2D tilePos = Engine::GetInstance().map->WorldToMap((int)pos.getX(), (int)pos.getY() + 1);
		pathfinding->ResetPath(tilePos);
	}

	return true;
}

bool Enemy::Update(float dt)
{
	ZoneScoped;

	// Handle cleanup if dead
	if (isDead && pbody != nullptr) {
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		pbody = nullptr;
	}
	// Handle death animation and destruction
	if (isDead) {
		anims.Update(dt);
		if (anims.HasFinishedOnce()) {
			Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
		}
		Draw(dt);
		return true;
	}

	GetPhysicsValues();
	if (enemyType == EnemyType::FLYING) {
		if (shootCooldownTimer > 0.0f) {
			shootCooldownTimer -= dt;
		}
	}

	//Enemy move logic
	// Update behavior state
	if (enemyType == EnemyType::BOSS) {
		UpdateBossBehavior(dt);
	}
	
	// 2. Execute pathfinding
	if (pathfinding != nullptr) {
		PerformPathfinding(dt);
	}

	// 3. Execute movement
	if (enemyType == EnemyType::GROUND) Move();
	else if (enemyType == EnemyType::FLYING) MoveFlying();
	else if (enemyType == EnemyType::BOSS) MoveBoss();
	else velocity = { 0.0f, 0.0f };
	ApplyPhysics();
	Draw(dt);

	return true;
}

void Enemy::PerformPathfinding(float dt) {
	// Boss does not calculate path during attack
	if (enemyType == EnemyType::BOSS && isAttacking) {
		return;
	}

	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D myPos = GetPosition();

	Vector2D myTile;
	Vector2D targetTile;

	// === Handle Logic Based on Type ===
	if (enemyType == EnemyType::BOSS) {
		// BOSS Logic
		int yOffset = 20;
		myTile = Engine::GetInstance().map->WorldToMap((int)myPos.getX(), (int)myPos.getY() + yOffset);
		targetTile = Engine::GetInstance().map->WorldToMap((int)playerPos.getX(), (int)myPos.getY() + yOffset);
	}
	else {
		// Normal/Flying Logic
		myTile = Engine::GetInstance().map->WorldToMap((int)myPos.getX(), (int)myPos.getY());
		targetTile = Engine::GetInstance().map->WorldToMap((int)playerPos.getX(), (int)playerPos.getY());
	}

	// Calculate distance
	float distance = myTile.distanceEuclidean(targetTile);

	// Only attempt pathfinding within detection range
	if (distance < detectionRadius) {
		pathfindingTimer += dt;

		if (pathfindingTimer >= 0.3f) { // Optimization: Calculate every 0.3s
			pathfinding->ComputeFullPathAStar(myTile, targetTile, MANHATTAN);
			pathfindingTimer = 0.0f;
		}
	}
	else {
		// Target too far, clear path
		pathfinding->pathTiles.clear();
		velocity.x = 0;

		// Special handling: Flying enemies need to stop Y movement
		if (enemyType == EnemyType::FLYING) {
			velocity.y = 0;
			if (anims.GetCurrentName() != "idleflying") anims.SetCurrent("idleflying");
		}
		else {
			if (anims.GetCurrentName() != "idle") anims.SetCurrent("idle");
		}
	}
}void Enemy::GetPhysicsValues() {
	// Read current velocity
	velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
	if (enemyType == EnemyType::GROUND && isGrounded) {
		velocity.x = 0;
	}
}

void Enemy::Move() {

	// Standard ground enemy movement logic
	if (pathfinding->pathTiles.size() > 1) {
		auto it = pathfinding->pathTiles.rbegin();
		it++;
		Vector2D nextTile = *it;
		Vector2D targetPos = Engine::GetInstance().map->MapToWorld((int)nextTile.getX(), (int)nextTile.getY());

		targetPos.setX(targetPos.getX() + 16);
		targetPos.setY(targetPos.getY() + 16);

		// Smooth cornering
		if (pathfinding->pathTiles.size() > 2) {
			Vector2D currentTilePos = Engine::GetInstance().map->WorldToMap((int)GetPosition().getX(), (int)GetPosition().getY());
			if ((int)nextTile.getX() == (int)currentTilePos.getX() && nextTile.getY() < currentTilePos.getY()) {
				it++;
				Vector2D futureTile = *it;
				Vector2D futurePos = Engine::GetInstance().map->MapToWorld((int)futureTile.getX(), (int)futureTile.getY());
				targetPos.setX(futurePos.getX() + 16);
			}
		}

		Vector2D currentPos = GetPosition();
		float xTolerance = 4.0f;

		if (currentPos.getX() < targetPos.getX() - xTolerance) {
			velocity.x = speed;
			anims.SetCurrent("move");
			flipState = SDL_FLIP_HORIZONTAL;
		}
		else if (currentPos.getX() > targetPos.getX() + xTolerance) {
			velocity.x = -speed;
			anims.SetCurrent("move");
			flipState = SDL_FLIP_NONE;
		}

	}
}


void Enemy::MoveFlying() {
	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D currentPos = GetPosition();

	float dirX = playerPos.getX() - currentPos.getX();
	float dirY = playerPos.getY() - currentPos.getY();
	float distance = sqrt(dirX * dirX + dirY * dirY);

	// ============================================
	// 1. 攻击逻辑：如果进入了攻击距离
	// ============================================
	if (distance <= flyingAttackDistance) {
		velocity.x = 0;
		velocity.y = 0;

		// 确保面朝玩家
		flipState = (dirX > 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

		if (shootCooldownTimer <= 0.0f) {
			// 开始前摇！不立刻射击，而是进入蓄力状态
			isPreparingAttack = true;
			attackWarningTimer = attackWarningDuration;
			shootCooldownTimer = shootCooldown; // 重置冷却

			// 如果你有蓄力动画，可以在这里播放：
			// anims.SetCurrent("charge-attack"); 
		}

		// 如果正在蓄力前摇
		if (isPreparingAttack) {
			attackWarningTimer -= 0.016f; // 假设每帧 16ms，也可以传入 dt
			if (attackWarningTimer <= 0.0f) {
				// 前摇结束，真正发射！
				ShootEnergyBall(playerPos);
				isPreparingAttack = false;
				if (anims.GetCurrentName() != "idleflying") anims.SetCurrent("idleflying");
			}
		}
		return; // 结束移动逻辑
	}

	// ============================================
	// 2. 移动逻辑（如果还没进入攻击距离）
	// ============================================
	isPreparingAttack = false; // 如果玩家跑出范围，取消蓄力

	// Flying enemy movement logic (Direct vector movement)
	if (pathfinding->pathTiles.size() < 2) {
		velocity.x = 0;
		velocity.y = 0;
		return;
	}

	auto it = pathfinding->pathTiles.rbegin();
	it++;
	Vector2D nextTile = *it;
	Vector2D targetPos = Engine::GetInstance().map->MapToWorld((int)nextTile.getX(), (int)nextTile.getY());

	targetPos.setX(targetPos.getX() + 16);
	targetPos.setY(targetPos.getY() + 16);

	float pathDirX = targetPos.getX() - currentPos.getX();
	float pathDirY = targetPos.getY() - currentPos.getY();
	float pathLength = sqrt(pathDirX * pathDirX + pathDirY * pathDirY);

	if (pathLength > 0) {
		velocity.x = (pathDirX / pathLength) * speed;
		velocity.y = (pathDirY / pathLength) * speed;

		if (velocity.x > 0) anims.SetCurrent("fly-right");
		else anims.SetCurrent("fly-left");
	}
	else {
		velocity.x = 0;
		velocity.y = 0;
	}
}



void Enemy::MoveBoss() {

	//Boss movement logic
	// Completely stop moving during attack
	if (isAttacking) {
		velocity.x = 0;
		return;
	}

	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D currentPos = GetPosition();
	float distanceToPlayerX = abs(playerPos.getX() - currentPos.getX());

	// 2. Attack Range Check: Stop if close enough to attack
	if (distanceToPlayerX <= attackRange - 10.0f) {
		velocity.x = 0;

		// Ensure facing player while standing still
		if (anims.GetCurrentName() != "idle") anims.SetCurrent("idle");
		if (playerPos.getX() > currentPos.getX()) flipState = SDL_FLIP_NONE;
		else flipState = SDL_FLIP_HORIZONTAL;

		return;
	}

	// Pathfinding Exception Handling
	if (pathfinding == nullptr || pathfinding->pathTiles.size() < 2) {
		velocity.x = 0;
		if (anims.GetCurrentName() != "idle") anims.SetCurrent("idle");
		if (playerPos.getX() > currentPos.getX()) flipState = SDL_FLIP_NONE;
		else flipState = SDL_FLIP_HORIZONTAL;

		return;
	}

	// Standard Pathfinding Movement
	Vector2D targetPos;
	bool hasTarget = false;

	while (pathfinding->pathTiles.size() > 1) {
		auto it = pathfinding->pathTiles.rbegin();
		it++; // Get next path node
		Vector2D nextTile = *it;

		targetPos = Engine::GetInstance().map->MapToWorld((int)nextTile.getX(), (int)nextTile.getY());
		targetPos.setX(targetPos.getX() + 16);
		targetPos.setY(targetPos.getY() + 16);
		float distX = targetPos.getX() - currentPos.getX();

		// Anti-turnaround mechanism: If overshoot, remove point and go to next
		bool isMovingRightAndTargetIsLeft = (velocity.x > 0.5f && distX < -10.0f);
		bool isMovingLeftAndTargetIsRight = (velocity.x < -0.5f && distX > 10.0f);

		// Increase tolerance to 5.0f to prevent jitter near nodes
		if (abs(distX) < 5.0f || isMovingRightAndTargetIsLeft || isMovingLeftAndTargetIsRight) {
			pathfinding->pathTiles.pop_back();
		}
		else {
			hasTarget = true;
			break;
		}
	}

	// If all points filtered, stop
	if (!hasTarget) {
		velocity.x = 0;
		if (anims.GetCurrentName() != "idle") anims.SetCurrent("idle");
		return;
	}

	//Execute Movement & Set Facing
	float xTolerance = 4.0f;
	if (currentPos.getX() < targetPos.getX() - xTolerance) {
		// === Move Right ===
		velocity.x = speed;
		if (anims.GetCurrentName() != "walk") anims.SetCurrent("walk");
		flipState = SDL_FLIP_NONE;
	}
	else if (currentPos.getX() > targetPos.getX() + xTolerance) {
		// === Move Left ===
		velocity.x = -speed;
		if (anims.GetCurrentName() != "walk") anims.SetCurrent("walk");
		flipState = SDL_FLIP_HORIZONTAL;
	}
	else {
		velocity.x = 0;
	}

	// ================== Force Limit Boss inside Battle Area ==================
	if (hasBattleArea) {
		Vector2D currentPos = GetPosition();

		// Check Left Boundary
		if (currentPos.getX() < battleAreaMinX) {
			// Force back to boundary
			Vector2D fixedPos = currentPos;
			fixedPos.setX((float)battleAreaMinX);
			SetPosition(fixedPos); // Update physics body position

			// If still trying to move left, zero out velocity
			if (velocity.x < 0) velocity.x = 0;
		}

		// Check Right Boundary
		else if (currentPos.getX() > battleAreaMaxX) {
	
			Vector2D fixedPos = currentPos;
			fixedPos.setX((float)battleAreaMaxX);
			SetPosition(fixedPos); 

			if (velocity.x > 0) velocity.x = 0;
		}
	}
	// ====================================================================


	// 6. Simple Jump Assist
	if (isGrounded && targetPos.getY() < currentPos.getY() - 32.0f) {
		velocity.y = -jumpForce;
		isGrounded = false;
	}
}

void Enemy::ApplyPhysics() {
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Enemy::Draw(float dt) {
	anims.Update(dt);

	// Automatically switch textures for Boss based on state
	if (enemyType == EnemyType::BOSS) {
		std::string currentAnimName = anims.GetCurrentName();
		if (bossTextures.find(currentAnimName) != bossTextures.end()) {
			texture = bossTextures[currentAnimName];
		}
	}

	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	if (pbody != nullptr) {
		int x, y;
		pbody->GetPosition(x, y);
		position.setX((float)x);
		position.setY((float)y);
	}

	// === Debug Drawing ===
	if (Engine::GetInstance().physics->IsDebug()) {

		if (pathfinding != nullptr) {
			pathfinding->DrawPath();
		}

		Vector2D centerPos = GetPosition();

		// Draw [Feet] Detection Point (Green) - Used for pathfinding logic
		int yOffset = (enemyType == EnemyType::BOSS) ? 20 : 0; 
		Vector2D feetTile = Engine::GetInstance().map->WorldToMap((int)centerPos.getX(), (int)centerPos.getY() + yOffset);
		Vector2D feetWorld = Engine::GetInstance().map->MapToWorld((int)feetTile.getX(), (int)feetTile.getY());
		SDL_Rect feetRect = { (int)feetWorld.getX(), (int)feetWorld.getY(), 32, 32 };
		Engine::GetInstance().render->DrawRectangle(feetRect, 0, 255, 0, 150); 

		// Draw [Center] Point (Red) - Physics body center
		Vector2D centerTile = Engine::GetInstance().map->WorldToMap((int)centerPos.getX(), (int)centerPos.getY());
		Vector2D centerWorld = Engine::GetInstance().map->MapToWorld((int)centerTile.getX(), (int)centerTile.getY());

		SDL_Rect centerRect = { (int)centerWorld.getX(), (int)centerWorld.getY(), 32, 32 };
		Engine::GetInstance().render->DrawRectangle(centerRect, 255, 0, 0, 150); 
	}

	int drawOffsetX = animFrame.w / 2;
	int drawOffsetY = animFrame.h / 2;

	Engine::GetInstance().render->DrawTexture(
		texture,
		(int)position.getX() - drawOffsetX,
		(int)position.getY() - drawOffsetY,
		&animFrame,
		1.0f,
		0.0f,
		INT_MAX,
		INT_MAX,
		flipState
	);

	// === Boss Health Bar Rendering Logic ===
	if (enemyType == EnemyType::BOSS && !isDead) {
		// Define dimensions
		int barWidth = 60;  
		int barHeight = 8; 
		int yOffset = 50; 

		// Calculate position (Centered above Boss)
		int barX = (int)position.getX() - (barWidth / 2);
		int barY = (int)position.getY() - yOffset;

		// Calculate health percentage
		float healthPercentage = (float)health / (float)maxHealth;
		// 确保不小于 0
		if (healthPercentage < 0) healthPercentage = 0;

		// Draw Background (Red)
		SDL_Rect bgRect = { barX, barY, barWidth, barHeight };
		Engine::GetInstance().render->DrawRectangle(bgRect, 255, 0, 0, 255); // 红色背景

		// Draw Foreground (Green - Scaled by health)
		SDL_Rect fgRect = { barX, barY, (int)(barWidth * healthPercentage), barHeight };
		Engine::GetInstance().render->DrawRectangle(fgRect, 0, 255, 0, 255); // 绿色前景

	}
}
bool Enemy::CleanUp()
{
	LOG("Cleanup enemy");

	if (pbody != nullptr) {
		pbody->listener = nullptr;
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		pbody = nullptr;
	}

	pathfinding = nullptr;

	for (auto& entry : bossTextures) {
		if (entry.second != nullptr) {
			Engine::GetInstance().textures->UnLoad(entry.second);
		}
	}
	bossTextures.clear();

	return true;
}

void Enemy::SetPosition(Vector2D pos) {
	pbody->SetPosition((int)(pos.getX()), (int)(pos.getY()));
}

Vector2D Enemy::GetPosition() {
	if (pbody != nullptr) {
		int x, y;
		pbody->GetPosition(x, y);
		return Vector2D((float)x, (float)y);
	}
	return position;
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	if (physB->ctype == ColliderType::PLATFORM) {
		isGrounded = true;
	}

	// Projectile Collision Logic
	if (physB->ctype == ColliderType::PROJECTILE) {
		if (!isDead) {
			// If Boss, take damage instead of dying instantly
			if (enemyType == EnemyType::BOSS) {
				health--;
				LOG("Boss Hit! HP: %d / %d", health, maxHealth);
				if (health <= 0) {
					active = false;
					isDead = true;
					Engine::GetInstance().scene->deadEnemyIDs.push_back(this->id);
					Engine::GetInstance().audio->PlayFx(deathFxId, 0, 5.0f);
					anims.SetCurrent("death");
				}
			}
			// If normal enemy, die instantly (Keep original logic)
			else {
				isDead = true;
				Engine::GetInstance().scene->deadEnemyIDs.push_back(this->id);
				Engine::GetInstance().audio->PlayFx(deathFxId, 0, 5.0f);
				if (enemyType == EnemyType::FLYING) {
					Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
				}
				else {
					anims.SetCurrent("death");
				}
			}
		}
	}
	else if (physB->ctype == ColliderType::DEATH) {
		if (!isDead) {
			isDead = true;
			Engine::GetInstance().scene->deadEnemyIDs.push_back(this->id);
			if (enemyType == EnemyType::FLYING) {
				Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
			}
			else {
				anims.SetCurrent("death");
			}
		}
	}
}

// Boss Behavior Logic
void Enemy::UpdateBossBehavior(float dt) {
	
	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D myPos = GetPosition();

	// Calculate distances
	float distanceToPlayerX = abs(playerPos.getX() - myPos.getX());
	float yDiff = abs(playerPos.getY() - myPos.getY());
	Player* player = Engine::GetInstance().scene->player.get();

	// Currently Attacking (Highest Priority)
	if (isAttacking) {
		velocity.x = 0;
		int currentFrame = anims.GetCurrentFrameIndex();

		// Optimized Damage Hitbox
		if (!hasDealtDamage && currentFrame >= 3 && currentFrame <= 6) {
			if (distanceToPlayerX < attackRange + 30.0f && yDiff < 60.0f) {
				if (player != nullptr) {
					player->TakeDamage(attackDamage);
					LOG("BOSS ATTACK HIT PLAYER!");
				}
				hasDealtDamage = true;
			}
		}

		// Check if animation finished
		if (anims.HasFinishedOnce()) {
			isAttacking = false;              
			bossState = BossState::IDLE_CHASE; 
			attackCooldownTimer = attackCooldown; 
			anims.SetCurrent("idle");         
		}
		return;
	}

	// Cooldown Timer
	if (attackCooldownTimer > 0.0f) {
		attackCooldownTimer -= dt;
	}

	// Trigger Attack (Not attacking + In Range + Cooldown Ready)
	if (distanceToPlayerX <= attackRange && attackCooldownTimer <= 0.0f && yDiff < 60.0f) {
		isAttacking = true;
		hasDealtDamage = false;
		velocity.x = 0;

		if (playerPos.getX() > myPos.getX()) flipState = SDL_FLIP_NONE;
		else flipState = SDL_FLIP_HORIZONTAL;

		// Random Attack Selection
		int randomAttack = rand() % 2;

		if (randomAttack == 0) {
			anims.SetCurrent("attack1");
			bossState = BossState::ATTACK_NORMAL;
			LOG("BOSS USES: ATTACK 1");
		}
		else {
			anims.SetCurrent("attack2");
			bossState = BossState::ATTACK_NORMAL;
			LOG("BOSS USES: ATTACK 2");
		}
	}
	else {

		bossState = BossState::IDLE_CHASE;
	}
}

void Enemy::ShootEnergyBall(Vector2D targetPos) {
	std::shared_ptr<Entity> entity = Engine::GetInstance().entityManager->CreateEntity(EntityType::PROJECTILE);
	std::shared_ptr<Projectile> p = std::dynamic_pointer_cast<Projectile>(entity);

	if (p != nullptr) {
		p->SetAsEnemyProjectile();

		Vector2D currentPos = GetPosition();
		Vector2D direction(targetPos.getX() - currentPos.getX(), targetPos.getY() - currentPos.getY());

		// 归一化方向向量
		float length = sqrt(direction.getX() * direction.getX() + direction.getY() * direction.getY());
		if (length > 0) {
			direction.setX(direction.getX() / length);
			direction.setY(direction.getY() / length);
		}

		// 【优化】计算出生成位置：往开火方向偏移 20 个像素
		Vector2D spawnPos(currentPos.getX() + direction.getX() * 20.0f,
			currentPos.getY() + direction.getY() * 20.0f);

		p->SetPosition(spawnPos);  // 使用带有偏移的位置
		p->SetVelocity(direction); // 设置速度

		// 播放开火音效 (如果你有的话)
		// Engine::GetInstance().audio->PlayFx(shootFxId); 
	}
}
void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	if (physB->ctype == ColliderType::PLATFORM) {
		isGrounded = false;
	}
}