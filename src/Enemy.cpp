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
	
	// load
	if (pbody != nullptr) return true;
	// ================== 【BOSS 多图逻辑】 ==================
	if (enemyType == EnemyType::BOSS) {
		LOG("Loading BOSS with Multiple Textures");

		// 1. 加载所有独立的图片 (请改成你真实的文件名)
		bossTextures["idle"] = Engine::GetInstance().textures->Load("Assets/Textures/Bidle.png");
		bossTextures["attack1"] = Engine::GetInstance().textures->Load("Assets/Textures/Boss_Attack_1.png");
		bossTextures["attack2"] = Engine::GetInstance().textures->Load("Assets/Textures/Boss_Attack_2.png");
		bossTextures["walk"] = Engine::GetInstance().textures->Load("Assets/Textures/Boss_walk.png");
		bossTextures["hit"] = Engine::GetInstance().textures->Load("Assets/Textures/hit.png");
		bossTextures["Preattack3"] = Engine::GetInstance().textures->Load("Assets/Textures/Pre-Attack-3.png");
		bossTextures["Midattack3"] = Engine::GetInstance().textures->Load("Assets/Textures/mid-Attack-3.png");
		bossTextures["Endattack3"] = Engine::GetInstance().textures->Load("Assets/Textures/end_Attack-3.png");
		bossTextures["death"] = Engine::GetInstance().textures->Load("Assets/Textures/death.png");

		// 默认当前图片设为 idle
		texture = bossTextures["idle"];

		//------------------------------------------------------------------------Boss Anim-------------------------------------------------------------------------------//
		Animation idleAnim;
		// 因为是独立图片，第一帧通常就是从 0,0 开始！不用在大图里找位置了
     // 【新增】第一帧左上角的 X 坐标 (前面的空白宽度)
		int frameWidth =247;  // 每一帧的宽度
		int frameHeight = 87;
		int frameCount1 = 14;
		int frameCount2 = 10;
		int frameCount3 = 9;
		int frameCount4 = 1;
		int frameCount5 = 4;
		int frameCount6 = 3;
		int frameCount7 = 7;
		int frameCount8 = 33;

		for (int i = 0; i < frameCount1; i++) {
			// 公式变了： 起始位置 + (第几个 * 宽度)
			int currentX =i * frameWidth;

			idleAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("idle", idleAnim);

		// 3. 定义 ATTACK 动画
		Animation attack1Anim;
		for (int i = 0; i < frameCount2; i++) {
			// 公式变了： 起始位置 + (第几个 * 宽度)
			int currentX = i * frameWidth;

			attack1Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("attack1", attack1Anim);

		Animation attack2Anim;
		for (int i = 0; i < frameCount3; i++) {
			// 公式变了： 起始位置 + (第几个 * 宽度)
			int currentX = i * frameWidth;

			attack2Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("attack2", attack2Anim);


		Animation Preattack3Anim;
		for (int i = 0; i < frameCount6; i++) {
			// 公式变了： 起始位置 + (第几个 * 宽度)
			int currentX = i * frameWidth;

			Preattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("Preattack3", Preattack3Anim);



		Animation Midattack3Anim;
		for (int i = 0; i < frameCount5; i++) {
			// 公式变了： 起始位置 + (第几个 * 宽度)
			int currentX = i * frameWidth;

			Midattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("Midattack3", Midattack3Anim);



		Animation Endattack3Anim;
		for (int i = 0; i < frameCount7; i++) {
			// 公式变了： 起始位置 + (第几个 * 宽度)
			int currentX = i * frameWidth;

			Endattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("Endattack3", Endattack3Anim);


		//Walk Anim
		Animation WalkAnim;
		for (int i = 0; i < frameCount1; i++) {
			// 公式变了： 起始位置 + (第几个 * 宽度)
			int currentX = i * frameWidth;

			WalkAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("walk", WalkAnim);

		//Hit and Death Anim
		Animation HitAnim;
		for (int i = 0; i < frameCount4; i++) {
			// 公式变了： 起始位置 + (第几个 * 宽度)
			int currentX = i * frameWidth;

			HitAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("hit", HitAnim);

		Animation DeathAnim;

		for (int i = 0; i < frameCount8; i++) {
			// 公式变了： 起始位置 + (第几个 * 宽度)
			int currentX = i * frameWidth;

			DeathAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("death", DeathAnim);

		// 设置初始状态
		anims.SetCurrent("idle");
		//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
		
		// 设置物理 (同之前)
		texW = 100; texH = 100;
		pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), 40, bodyType::DYNAMIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::ENEMY;
		b2Body_SetGravityScale(pbody->body, 1.0f);
	}else if (enemyType == EnemyType::FLYING) {
		std::unordered_map<int, std::string>EnemyFlying = { {0,"idleflying"},{4,"fly-right"},{8,"fly-down"},{12,"fly-left"} };
		deathFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/monster-death.mp3"); // Carga el audio
		anims.LoadFromTSX("Assets/Textures/32x32-bat-sprite.tsx", EnemyFlying);
		anims.SetCurrent("idleflying");
		texture = Engine::GetInstance().textures->Load("Assets/Textures/32x32-bat-sprite.png");
	}
	else {
		std::unordered_map<int, std::string> aliases = { {13,"move"},{24,"idle"},{35,"death"} };
		deathFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/monster-death.wav"); // Carga el audio
		anims.LoadFromTSX("Assets/Textures/slime-Sheet2.tsx", aliases);
		anims.SetCurrent("idle");


		//Initialize Player parameters
		texture = Engine::GetInstance().textures->Load("Assets/Textures/slime-Sheet2.png");
	}



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

	// Inicializar pathfinding (igual que antes)
	pathfinding = std::make_shared<Pathfinding>();
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map->WorldToMap((int)pos.getX(), (int)pos.getY() + 1);
	pathfinding->ResetPath(tilePos);

	return true;
}

bool Enemy::Update(float dt)
{
	if (isDead) {
		anims.Update(dt);
		if (anims.HasFinishedOnce()) {
			Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
		}
		Draw(dt);
		return true;
	}

	GetPhysicsValues();
	// === 【新增】Boss 逻辑处理 ===
	if (enemyType == EnemyType::BOSS) {
		UpdateBossBehavior(dt);
	}
	if (pathfinding != nullptr) {
		PerformPathfinding(dt);		
	}

	if (enemyType == EnemyType::GROUND) Move();
	else if (enemyType == EnemyType::FLYING) MoveFlying();
	else if (enemyType == EnemyType::BOSS) Move();
	else velocity = { 0.0f, 0.0f };
	ApplyPhysics();
	Draw(dt);

	return true;
}
void Enemy::PerformPathfinding(float dt) {

	// Pathfinding testing inputs

	// Reset pathfinding with R key
	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D playerTile = Engine::GetInstance().map->WorldToMap((int)playerPos.getX(), (int)playerPos.getY());

	// 2. Obtener mi posición (Origen)
	Vector2D myPos = GetPosition();
	Vector2D myTile = Engine::GetInstance().map->WorldToMap((int)myPos.getX(), (int)myPos.getY());

	float distance = myTile.distanceEuclidean(playerTile);

	float verticalDist = abs(playerPos.getY() - myPos.getY());
	bool targetTooHigh = false;
	if (enemyType == EnemyType::GROUND) {
		// 如果高度差大于 64 像素（根据你的图块大小调整，这里假设32x32）
		if (verticalDist > 64.0f) {
			targetTooHigh = true;
		}
	}
	// 修改判断条件：加入 && !targetTooHigh
	if (distance < detectionRadius && !targetTooHigh) {
		// 3. Calcular el camino automáticamente
		// Usamos MANHATTAN como heurística por defecto

		pathfindingTimer += dt;
		if (pathfindingTimer >= pathfindingInterval) {
			pathfinding->ComputeFullPathAStar(myTile, MANHATTAN);
			pathfindingTimer = 0.0f;
		}
	}
	else {
		// 目标太远 或者 目标太高 -> 停止移动，进入 Idle
		pathfinding->pathTiles.clear();
		velocity.x = 0;
		if (enemyType == EnemyType::FLYING) {
			velocity.y = 0;
		}

		// 3. Poner animación de Idle
		anims.SetCurrent("idle");
	}

	// Propagate BFS with J key
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_J) == KEY_DOWN) {
		pathfinding->PropagateBFS();
	}

	// Propagate BFS continuously with LShift + J
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_J) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateBFS();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_DOWN) {
		pathfinding->PropagateDijkstra();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateDijkstra();
	}

	// L13: TODO 3:	Add the key inputs to propagate the A* algorithm with different heuristics (Manhattan, Euclidean, Squared)

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_B) == KEY_DOWN) {
		pathfinding->PropagateAStar(MANHATTAN);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_B) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateAStar(MANHATTAN);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_N) == KEY_DOWN) {
		pathfinding->PropagateAStar(EUCLIDEAN);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_N) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateAStar(EUCLIDEAN);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_M) == KEY_DOWN) {
		pathfinding->PropagateAStar(SQUARED);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_M) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateAStar(SQUARED);
	}

}

void Enemy::GetPhysicsValues() {
	// Read current velocity
	velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
	if (enemyType == EnemyType::GROUND && isGrounded) {
		velocity.x = 0;
	}
}



void Enemy::Move() {
	if (pathfinding->pathTiles.size() > 1) {
		auto it = pathfinding->pathTiles.rbegin();
		it++; // Primer objetivo (inmediato)
		Vector2D nextTile = *it;
		Vector2D targetPos = Engine::GetInstance().map->MapToWorld((int)nextTile.getX(), (int)nextTile.getY());

		// Centramos el objetivo en el tile
		targetPos.setX(targetPos.getX() + 16);
		targetPos.setY(targetPos.getY() + 16);

		// --- TRUCO DE "LOOKAHEAD" PARA SALTOS ---
		// Si el siguiente tile está justo encima (salto vertical), miramos un paso más allá
		// para ver si hay que avanzar horizontalmente TAMBIÉN.
		if (pathfinding->pathTiles.size() > 2) {
			Vector2D currentTilePos = Engine::GetInstance().map->WorldToMap((int)GetPosition().getX(), (int)GetPosition().getY());

			// Si el siguiente tile tiene la misma X que yo (es un salto vertical)...
			if ((int)nextTile.getX() == (int)currentTilePos.getX() && nextTile.getY() < currentTilePos.getY()) {
				it++; // Miramos el siguiente del siguiente
				Vector2D futureTile = *it;
				// Usamos la X del futuro tile como objetivo horizontal
				Vector2D futurePos = Engine::GetInstance().map->MapToWorld((int)futureTile.getX(), (int)futureTile.getY());
				targetPos.setX(futurePos.getX() + 16);
			}
		}

		Vector2D currentPos = GetPosition();

		// --- MOVIMIENTO HORIZONTAL ---
		// Ahora 'targetPos' contiene la X del futuro tile si estamos saltando
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
	// Si no hay camino, frenar
	if (pathfinding->pathTiles.size() < 2) {
		velocity.x = 0;
		velocity.y = 0;
		return;
	}

	// 1. Obtener destino (siguiente tile)
	auto it = pathfinding->pathTiles.rbegin();
	it++;
	Vector2D nextTile = *it;
	Vector2D targetPos = Engine::GetInstance().map->MapToWorld((int)nextTile.getX(), (int)nextTile.getY());

	// Centrar objetivo (+16 asumiendo tiles de 32)
	targetPos.setX(targetPos.getX() + 16);
	targetPos.setY(targetPos.getY() + 16);

	Vector2D currentPos = GetPosition();

	// 2. Calcular vector dirección (Destino - Origen)
	float dirX = targetPos.getX() - currentPos.getX();
	float dirY = targetPos.getY() - currentPos.getY();

	// 3. Normalizar (Teorema de Pitágoras)
	float length = sqrt(dirX * dirX + dirY * dirY);

	if (length > 0) {
		// Normalizamos y aplicamos velocidad
		velocity.x = (dirX / length) * speed;
		velocity.y = (dirY / length) * speed;

		// Animación básica
		if (velocity.x > 0) anims.SetCurrent("fly-right"); // O usa flip
		else anims.SetCurrent("fly-left");
	}
	else {
		velocity.x = 0;
		velocity.y = 0;
	}
}

void Enemy::ApplyPhysics() {

	// Apply velocity via helper
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Enemy::Draw(float dt) {

	anims.Update(dt);

	// ================== 【新增：自动切换图片】 ==================
	if (enemyType == EnemyType::BOSS) {
		// 获取当前正在播放的动画名字 (例如 "idle", "attack")
		std::string currentAnimName = anims.GetCurrentName();

		// 如果这个名字在我们的素材库里，就切换 texture
		if (bossTextures.find(currentAnimName) != bossTextures.end()) {
			texture = bossTextures[currentAnimName];
		}
	}
	// ==========================================================
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	// Update render position using your PhysBody helper

   if (pbody != nullptr) {
		int x, y;
		pbody->GetPosition(x, y);
		position.setX((float)x);
		position.setY((float)y);
	}

	// Draw pathfinding debug

	if (Engine::GetInstance().physics->IsDebug()) {
		pathfinding->DrawPath();
	}
	//Draw the player using the texture and the current animation frame

	Engine::GetInstance().render->DrawTexture(texture, (int)position.getX() - texW / 2, (int)position.getY() - texH / 2, &animFrame,1.0f,0.0f,INT_MAX,INT_MAX,flipState);

}

bool Enemy::CleanUp()
{
	LOG("Cleanup enemy");

	// 1. Borrar física (ya lo tenías)
	if (pbody != nullptr) {
		pbody->listener = nullptr;
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		pbody = nullptr;
	}

	// 2. Borrar Pathfinding (NUEVO E IMPORTANTE)
	pathfinding = nullptr;

	// 【新增】清理 Boss 的多张图片
	for (auto& entry : bossTextures) {
		if (entry.second != nullptr) {
			Engine::GetInstance().textures->UnLoad(entry.second);
		}
	}
	bossTextures.clear();
	// 3. Descargar textura
	/*if (texture != nullptr) {
		Engine::GetInstance().textures->UnLoad(texture);
		texture = nullptr;
	}*/

	return true;
}

void Enemy::SetPosition(Vector2D pos) {
	pbody->SetPosition((int)(pos.getX()), (int)(pos.getY()));
}

Vector2D Enemy::GetPosition() {
	int x, y;
	pbody->GetPosition(x, y);
	// Adjust for center
	//return Vector2D((float)x-texW/2,(float)y-texH/2);
	return Vector2D((float)x, (float)y);
}

//Define OnCollision function for the enemy. 
void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	if (physB->ctype == ColliderType::PLATFORM) {
		isGrounded = true;
	}
	if (physB->ctype == ColliderType::PROJECTILE) {
		if (!isDead) {
			isDead = true;
			Engine::GetInstance().audio->PlayFx(deathFxId, 0, 5.0f);

			// Desactiva físicas para que no siga chocando ni se mueva
			Engine::GetInstance().physics->DeletePhysBody(pbody);
			pbody = nullptr;

			// Reproducir animación de muerte
			// Según tu código actual, ya cargas "death" en el Start()
			if (enemyType == EnemyType::FLYING) {
				// El murciélago no tiene animación de muerte, lo destruimos ya.
				Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
			}
			else {
				// El Slime sí tiene animación "death", dejamos que se reproduzca.
				anims.SetCurrent("death");
			}
		}
	}
	else if (physB->ctype == ColliderType::DEATH) {
		if (!isDead) {
			isDead = true;

			// Desactiva físicas
			Engine::GetInstance().physics->DeletePhysBody(pbody);
			pbody = nullptr;

			// Simula la muerte (igual que al recibir un proyectil)
			if (enemyType == EnemyType::FLYING) {
				Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
			}
			else {
				anims.SetCurrent("death");
			}
		}
	}
}

void Enemy::UpdateBossBehavior(float dt) {

	// 只有在攻击状态下，我们要检查动画是否播放完毕
	if (isAttacking) {
		// 更新动画状态（必须在 Update 里调用 anims.Update，或者在这里确认状态）

		if (anims.HasFinishedOnce()) {
			switch (bossState) {
			case BossState::ATTACK_NORMAL:
				// 普通攻击结束 -> 回到空闲
				isAttacking = false;
				bossState = BossState::IDLE_CHASE;
				anims.SetCurrent("idle");
				break;

			case BossState::SKILL_PRE:
				// 大招前摇结束 -> 进入中段
				bossState = BossState::SKILL_MID;
				anims.SetCurrent("Midattack3");
				break;

			case BossState::SKILL_MID:
				// 大招中段结束 -> 进入后摇
				bossState = BossState::SKILL_END;
				anims.SetCurrent("Endattack3");
				break;

			case BossState::SKILL_END:
				// 大招完全结束 -> 回到空闲
				isAttacking = false;
				bossState = BossState::IDLE_CHASE;
				anims.SetCurrent("idle");
				break;

			default:
				break;
			}
		}
	}
	else {
		// === 非攻击状态 (IDLE_CHASE) ===
		// 1. 计时
		normalAttackTimer += dt;
		skillTimer += dt;

		// 2. 检查大招 (优先级最高，11秒一次)
		if (skillTimer >= 11.0f) {
			skillTimer = 0.0f; // 重置计时
			isAttacking = true;
			bossState = BossState::SKILL_PRE;
			velocity = { 0, 0 }; // 攻击时停止移动
			anims.SetCurrent("Preattack3");
			// 可选：播放大招音效
		}
		// 3. 检查普通攻击 (3秒一次)
		else if (normalAttackTimer >= 3.0f) {
			normalAttackTimer = 0.0f; // 重置计时
			isAttacking = true;
			bossState = BossState::ATTACK_NORMAL;
			velocity = { 0, 0 }; // 攻击时停止移动

			// 随机选择 attack1 或 attack2
			if (rand() % 2 == 0) {
				anims.SetCurrent("attack1");
			}
			else {
				anims.SetCurrent("attack2");
			}
			// 可选：播放普攻音效
		}
	}
}

void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	if (physB->ctype == ColliderType::PLATFORM) {
		isGrounded = false;
	}
}
