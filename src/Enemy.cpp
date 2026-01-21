//#include "Enemy.h"
//#include "Engine.h"
//#include "Textures.h"
//#include "Audio.h"
//#include "Input.h"
//#include "Render.h"
//#include "Scene.h"
//#include "Log.h"
//#include "Physics.h"
//#include "EntityManager.h"
//#include "Map.h"
//#include "Player.h"
//
//Enemy::Enemy() : Entity(EntityType::ENEMY) {
//	name = "Enemy";
//
//	pbody = nullptr;
//	pathfinding = nullptr;
//	texture = nullptr;
//	texW = 0;
//	texH = 0;
//	velocity = { 0.0f, 0.0f };
//	isGrounded = false;
//}
//
//Enemy::~Enemy() {
//
//}
//
//bool Enemy::Awake() {
//	return true;
//}
//
//void Enemy::SetEnemyType(EnemyType type) {
//	enemyType = type;
//}
//bool Enemy::Start() {
//	
//	// load
//	if (pbody != nullptr) return true;
//	// ================== 【BOSS 多图逻辑】 ==================
//	if (enemyType == EnemyType::BOSS) {
//		LOG("Loading BOSS with Multiple Textures");
//
//		// 1. 加载所有独立的图片 (请改成你真实的文件名)
//		bossTextures["idle"] = Engine::GetInstance().textures->Load("Assets/Textures/Bidle.png");
//		bossTextures["attack1"] = Engine::GetInstance().textures->Load("Assets/Textures/Boss_Attack_1.png");
//		bossTextures["attack2"] = Engine::GetInstance().textures->Load("Assets/Textures/Boss_Attack_2.png");
//		bossTextures["walk"] = Engine::GetInstance().textures->Load("Assets/Textures/Boss_walk.png");
//		bossTextures["hit"] = Engine::GetInstance().textures->Load("Assets/Textures/hit.png");
//		bossTextures["Preattack3"] = Engine::GetInstance().textures->Load("Assets/Textures/Pre-Attack-3.png");
//		bossTextures["Midattack3"] = Engine::GetInstance().textures->Load("Assets/Textures/mid-Attack-3.png");
//		bossTextures["Endattack3"] = Engine::GetInstance().textures->Load("Assets/Textures/end-Attack-3.png");
//		bossTextures["death"] = Engine::GetInstance().textures->Load("Assets/Textures/death.png");
//
//		// 默认当前图片设为 idle
//		texture = bossTextures["idle"];
//
//		//------------------------------------------------------------------------Boss Anim-------------------------------------------------------------------------------//
//		Animation idleAnim;
//		// 因为是独立图片，第一帧通常就是从 0,0 开始！不用在大图里找位置了
//     // 【新增】第一帧左上角的 X 坐标 (前面的空白宽度)
//		int frameWidth =247;  // 每一帧的宽度
//		int frameHeight = 87;
//		int frameCount1 = 14;
//		int frameCount2 = 10;
//		int frameCount3 = 9;
//		int frameCount4 = 1;
//		int frameCount5 = 4;
//		int frameCount6 = 3;
//		int frameCount7 = 7;
//		int frameCount8 = 33;
//
//		for (int i = 0; i < frameCount1; i++) {
//			// 公式变了： 起始位置 + (第几个 * 宽度)
//			int currentX =i * frameWidth;
//
//			idleAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		}
//		anims.AddClip("idle", idleAnim);
//
//		//// 3. 定义 ATTACK 动画
//		//Animation attack1Anim;
//	
//		//for (int i = 0; i < frameCount2; i++) {
//		//	// 公式变了： 起始位置 + (第几个 * 宽度)
//		//	int currentX = i * frameWidth;
//
//		//	attack1Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		//}
//		//anims.AddClip("attack1", attack1Anim);
//
//		//Animation attack2Anim;
//		//for (int i = 0; i < frameCount3; i++) {
//		//	// 公式变了： 起始位置 + (第几个 * 宽度)
//		//	int currentX = i * frameWidth;
//
//		//	attack2Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		//}
//		//anims.AddClip("attack2", attack2Anim);
//
//
//		//Animation Preattack3Anim;
//		//for (int i = 0; i < frameCount6; i++) {
//		//	// 公式变了： 起始位置 + (第几个 * 宽度)
//		//	int currentX = i * frameWidth;
//
//		//	Preattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		//}
//		//anims.AddClip("Preattack3", Preattack3Anim);
//
//
//
//		//Animation Midattack3Anim;
//		//for (int i = 0; i < frameCount5; i++) {
//		//	// 公式变了： 起始位置 + (第几个 * 宽度)
//		//	int currentX = i * frameWidth;
//
//		//	Midattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		//}
//		//anims.AddClip("Midattack3", Midattack3Anim);
//
//
//
//		//Animation Endattack3Anim;
//		//for (int i = 0; i < frameCount7; i++) {
//		//	// 公式变了： 起始位置 + (第几个 * 宽度)
//		//	int currentX = i * frameWidth;
//
//		//	Endattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		//}
//		//anims.AddClip("Endattack3", Endattack3Anim);
//
//
//		////Walk Anim
//		//Animation WalkAnim;
//		//for (int i = 0; i < frameCount1; i++) {
//		//	// 公式变了： 起始位置 + (第几个 * 宽度)
//		//	int currentX = i * frameWidth;
//
//		//	WalkAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		//}
//		//anims.AddClip("walk", WalkAnim);
//
//		////Hit and Death Anim
//		//Animation HitAnim;
//		//for (int i = 0; i < frameCount4; i++) {
//		//	// 公式变了： 起始位置 + (第几个 * 宽度)
//		//	int currentX = i * frameWidth;
//
//		//	HitAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		//}
//		//anims.AddClip("hit", HitAnim);
//
//		//Animation DeathAnim;
//
//		//for (int i = 0; i < frameCount8; i++) {
//		//	// 公式变了： 起始位置 + (第几个 * 宽度)
//		//	int currentX = i * frameWidth;
//
//		//	DeathAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		//}
//		//anims.AddClip("death", DeathAnim);
//
//
//		// 3. 定义 ATTACK 动画 (必须设置为不循环！)
//		Animation attack1Anim;
//		attack1Anim.SetLoop(false); // 【新增】设置为不循环
//		for (int i = 0; i < frameCount2; i++) {
//			int currentX = i * frameWidth;
//			attack1Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		}
//		anims.AddClip("attack1", attack1Anim);
//
//		Animation attack2Anim;
//		attack2Anim.SetLoop(false); // 【新增】设置为不循环
//		for (int i = 0; i < frameCount3; i++) {
//			int currentX = i * frameWidth;
//			attack2Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		}
//		anims.AddClip("attack2", attack2Anim);
//
//
//		Animation Preattack3Anim;
//		Preattack3Anim.SetLoop(false); // 【新增】设置为不循环
//		for (int i = 0; i < frameCount6; i++) {
//			int currentX = i * frameWidth;
//			Preattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		}
//		anims.AddClip("Preattack3", Preattack3Anim);
//
//
//		Animation Midattack3Anim;
//		Midattack3Anim.SetLoop(false); // 【新增】设置为不循环
//		for (int i = 0; i < frameCount5; i++) {
//			int currentX = i * frameWidth;
//			Midattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		}
//		anims.AddClip("Midattack3", Midattack3Anim);
//
//
//		Animation Endattack3Anim;
//		Endattack3Anim.SetLoop(false); // 【新增】设置为不循环
//		for (int i = 0; i < frameCount7; i++) {
//			int currentX = i * frameWidth;
//			Endattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		}
//		anims.AddClip("Endattack3", Endattack3Anim);
//
//
//		// Walk Anim (走路需要循环)
//		Animation WalkAnim;
//		WalkAnim.SetLoop(true); // 显式设置为 true (虽然默认也是 true)
//		for (int i = 0; i < frameCount1; i++) {
//			int currentX = i * frameWidth;
//			WalkAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		}
//		anims.AddClip("walk", WalkAnim);
//
//		// Hit and Death Anim
//		Animation HitAnim;
//		HitAnim.SetLoop(false); // 【新增】受伤不循环
//		for (int i = 0; i < frameCount4; i++) {
//			int currentX = i * frameWidth;
//			HitAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		}
//		anims.AddClip("hit", HitAnim);
//
//		Animation DeathAnim;
//		DeathAnim.SetLoop(false); // 【新增】死亡不循环，停在最后一帧
//		for (int i = 0; i < frameCount8; i++) {
//			int currentX = i * frameWidth;
//			DeathAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
//		}
//		anims.AddClip("death", DeathAnim);
//
//
//
//
//
//
//
//
//
//		// 设置初始状态
//		anims.SetCurrent("idle");
//
//		// 注意：这里创建了 Boss 专属的大碰撞体
//		texW = 100; texH = 100;
//		pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), 40, bodyType::DYNAMIC);
//		pbody->listener = this;
//		pbody->ctype = ColliderType::ENEMY;
//		b2Body_SetGravityScale(pbody->body, 1.0f);
//
//
//		// ================== 【新增：Boss 独立参数设置】 ==================
//		// 1. 设置更大的检测范围 (假设 1格 = 32像素，5格 = 160像素，或者直接增加半径数值)
//		// 原本 detectionRadius 是 10 (单位取决于你的距离计算方式，通常是 Tile 数量)
//		detectionRadius = 15; // 10 + 5格
//
//		// 2. 性能优化：寻路计算间隔
//		// 普通怪是 0.5s，Boss 可以保持一样，或者因为范围大，稍微加长一点点也没关系
//		pathfindingInterval = 0.5f;
//
//		// 3. Boss 移动速度
//		speed = 2.0f; // 稍微慢一点显得有压迫感，或者快一点增加难度
//
//		//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//	
//	}else if (enemyType == EnemyType::FLYING) {
//		std::unordered_map<int, std::string>EnemyFlying = { {0,"idleflying"},{4,"fly-right"},{8,"fly-down"},{12,"fly-left"} };
//		deathFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/monster-death.mp3"); // Carga el audio
//		anims.LoadFromTSX("Assets/Textures/32x32-bat-sprite.tsx", EnemyFlying);
//		anims.SetCurrent("idleflying");
//		texture = Engine::GetInstance().textures->Load("Assets/Textures/32x32-bat-sprite.png");
//	}
//	else {
//		std::unordered_map<int, std::string> aliases = { {13,"move"},{24,"idle"},{35,"death"} };
//		deathFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/monster-death.wav"); // Carga el audio
//		anims.LoadFromTSX("Assets/Textures/slime-Sheet2.tsx", aliases);
//		anims.SetCurrent("idle");
//
//
//		//Initialize Player parameters
//		texture = Engine::GetInstance().textures->Load("Assets/Textures/slime-Sheet2.png");
//	}
//
//
//	if (pbody == nullptr) {
//		//Add physics to the enemy - initialize physics body
//		texW = 32;
//		texH = 32;
//
//		pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);
//
//		//Assign enemy class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
//		pbody->listener = this;
//
//		//ssign collider type
//		pbody->ctype = ColliderType::ENEMY;
//
//		if (enemyType == EnemyType::FLYING) {
//			b2Body_SetGravityScale(pbody->body, 0.0f);
//		}
//		else {
//			b2Body_SetGravityScale(pbody->body, 1.0f);
//		}
//	}
//
//	// Inicializar pathfinding (igual que antes)
//	pathfinding = std::make_shared<Pathfinding>();
//	Vector2D pos = GetPosition();
//	//Vector2D tilePos = Engine::GetInstance().map->WorldToMap((int)pos.getX(), (int)pos.getY() + 1);
//	//pathfinding->ResetPath(tilePos);
//	// 确保 Map 模块已加载
//	if (Engine::GetInstance().map != nullptr) {
//		Vector2D tilePos = Engine::GetInstance().map->WorldToMap((int)pos.getX(), (int)pos.getY() + 1);
//		pathfinding->ResetPath(tilePos);
//	}
//
//	return true;
//}
//
//bool Enemy::Update(float dt)
//{
//	if (isDead) {
//		anims.Update(dt);
//		if (anims.HasFinishedOnce()) {
//			Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
//		}
//		Draw(dt);
//		return true;
//	}
//
//	GetPhysicsValues();
//	// 1. 先更新行为状态 (决定是否 isAttacking)
//	if (enemyType == EnemyType::BOSS) {
//		UpdateBossBehavior(dt);
//	}
//
//	// 2. 再执行寻路 (如果 isAttacking 为 true，上面的修改会通过 PerformPathfinding 直接 return)
//	if (pathfinding != nullptr) {
//		PerformPathfinding(dt);
//	}
//
//	// 3. 最后执行移动 (MoveBoss 内部也已经有 isAttacking 的检查了)
//	if (enemyType == EnemyType::GROUND) Move();
//	else if (enemyType == EnemyType::FLYING) MoveFlying();
//	else if (enemyType == EnemyType::BOSS) MoveBoss();
//	else velocity = { 0.0f, 0.0f };
//
//	ApplyPhysics();
//	Draw(dt);
//
//	return true;
//}
//void Enemy::PerformPathfinding(float dt) {
//
//
//	// ================== 【修复：攻击时禁止寻路逻辑干扰】 ==================
//	// 如果是 Boss 且正在攻击，直接退出，不更新路径，也不重置为 Idle
//	// 这样可以保护 Attack 动画不被覆盖，同时节省性能
//	if (enemyType == EnemyType::BOSS && isAttacking) {
//		return;
//	}
//	// Pathfinding testing inputs
//
//	// Reset pathfinding with R key
//	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
//	Vector2D playerTile = Engine::GetInstance().map->WorldToMap((int)playerPos.getX(), (int)playerPos.getY());
//
//	// 2. Obtener mi posición (Origen)
//	Vector2D myPos = GetPosition();
//	int offsetY = (enemyType == EnemyType::BOSS) ? 32 : 0;
//	Vector2D myTile = Engine::GetInstance().map->WorldToMap((int)myPos.getX(), (int)myPos.getY());
//
//	float distance = myTile.distanceEuclidean(playerTile);
//
//	// Boss 忽略高度差检查 (targetTooHigh)，因为它可能需要跳下来或者单纯为了保持追踪
//	bool targetTooHigh = false;
//	if (enemyType == EnemyType::GROUND) {
//		float verticalDist = abs(playerPos.getY() - myPos.getY());
//		if (verticalDist > 64.0f) {
//			targetTooHigh = true;
//		}
//	}
//
//
//
//	//float verticalDist = abs(playerPos.getY() - myPos.getY());
//	//bool targetTooHigh = false;
//
//	// 3. 执行寻路
//	if (distance < detectionRadius && !targetTooHigh) {
//		pathfindingTimer += dt;
//		if (pathfindingTimer >= pathfindingInterval) {
//
//			// 计算路径
//			pathfinding->ComputeFullPathAStar(myTile, MANHATTAN);
//
//			// ================== 【调试日志】 ==================
//			// 如果 Boss 还是不动，请查看 Output 窗口
//			if (enemyType == EnemyType::BOSS) {
//				if (pathfinding->pathTiles.empty()) {
//					LOG("BOSS: Path not found! I am at Tile(%d, %d), Player at Tile(%d, %d)",
//						(int)myTile.getX(), (int)myTile.getY(), (int)playerTile.getX(), (int)playerTile.getY());
//				}
//				else {
//					// LOG("BOSS: Path found! Steps: %d", pathfinding->pathTiles.size());
//				}
//			}
//			// ================================================
//
//			pathfindingTimer = 0.0f;
//		}
//	}
//	else {
//		// 目标太远 -> 清空路径 -> MoveBoss 会检测到并 Idle
//		pathfinding->pathTiles.clear();
//		velocity.x = 0;
//
//		if (enemyType == EnemyType::FLYING) {
//			velocity.y = 0;
//		}
//
//		// 只有当前不是 idle 时才重置，防止动画鬼畜
//		if (anims.GetCurrentName() != "idle") {
//			anims.SetCurrent("idle");
//		}
//	}
//
//
//
//	
//	
//	// Propagate BFS with J key
//	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_J) == KEY_DOWN) {
//		pathfinding->PropagateBFS();
//	}
//
//	// Propagate BFS continuously with LShift + J
//	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_J) == KEY_REPEAT &&
//		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
//		pathfinding->PropagateBFS();
//	}
//
//	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_DOWN) {
//		pathfinding->PropagateDijkstra();
//	}
//
//	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_REPEAT &&
//		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
//		pathfinding->PropagateDijkstra();
//	}
//
//	// L13: TODO 3:	Add the key inputs to propagate the A* algorithm with different heuristics (Manhattan, Euclidean, Squared)
//
//	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_B) == KEY_DOWN) {
//		pathfinding->PropagateAStar(MANHATTAN);
//	}
//
//	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_B) == KEY_REPEAT &&
//		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
//		pathfinding->PropagateAStar(MANHATTAN);
//	}
//
//	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_N) == KEY_DOWN) {
//		pathfinding->PropagateAStar(EUCLIDEAN);
//	}
//
//	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_N) == KEY_REPEAT &&
//		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
//		pathfinding->PropagateAStar(EUCLIDEAN);
//	}
//
//	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_M) == KEY_DOWN) {
//		pathfinding->PropagateAStar(SQUARED);
//	}
//
//	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_M) == KEY_REPEAT &&
//		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
//		pathfinding->PropagateAStar(SQUARED);
//	}
//
//}
//
//void Enemy::GetPhysicsValues() {
//	// Read current velocity
//	velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
//	if (enemyType == EnemyType::GROUND && isGrounded) {
//		velocity.x = 0;
//	}
//}
//
//
//
//void Enemy::Move() {
//	if (pathfinding->pathTiles.size() > 1) {
//		auto it = pathfinding->pathTiles.rbegin();
//		it++; // Primer objetivo (inmediato)
//		Vector2D nextTile = *it;
//		Vector2D targetPos = Engine::GetInstance().map->MapToWorld((int)nextTile.getX(), (int)nextTile.getY());
//
//		// Centramos el objetivo en el tile
//		targetPos.setX(targetPos.getX() + 16);
//		targetPos.setY(targetPos.getY() + 16);
//
//		// --- TRUCO DE "LOOKAHEAD" PARA SALTOS ---
//		// Si el siguiente tile está justo encima (salto vertical), miramos un paso más allá
//		// para ver si hay que avanzar horizontalmente TAMBIÉN.
//		if (pathfinding->pathTiles.size() > 2) {
//			Vector2D currentTilePos = Engine::GetInstance().map->WorldToMap((int)GetPosition().getX(), (int)GetPosition().getY());
//
//			// Si el siguiente tile tiene la misma X que yo (es un salto vertical)...
//			if ((int)nextTile.getX() == (int)currentTilePos.getX() && nextTile.getY() < currentTilePos.getY()) {
//				it++; // Miramos el siguiente del siguiente
//				Vector2D futureTile = *it;
//				// Usamos la X del futuro tile como objetivo horizontal
//				Vector2D futurePos = Engine::GetInstance().map->MapToWorld((int)futureTile.getX(), (int)futureTile.getY());
//				targetPos.setX(futurePos.getX() + 16);
//			}
//		}
//
//		Vector2D currentPos = GetPosition();
//
//		// --- MOVIMIENTO HORIZONTAL ---
//		// Ahora 'targetPos' contiene la X del futuro tile si estamos saltando
//		float xTolerance = 4.0f;
//
//		if (currentPos.getX() < targetPos.getX() - xTolerance) {
//			velocity.x = speed;
//			anims.SetCurrent("move");
//			flipState = SDL_FLIP_HORIZONTAL;
//		}
//		else if (currentPos.getX() > targetPos.getX() + xTolerance) {
//			velocity.x = -speed;
//			anims.SetCurrent("move");
//			flipState = SDL_FLIP_NONE;
//		}
//
//	}
//}
//
//
//void Enemy::MoveFlying() {
//	// Si no hay camino, frenar
//	if (pathfinding->pathTiles.size() < 2) {
//		velocity.x = 0;
//		velocity.y = 0;
//		return;
//	}
//
//	// 1. Obtener destino (siguiente tile)
//	auto it = pathfinding->pathTiles.rbegin();
//	it++;
//	Vector2D nextTile = *it;
//	Vector2D targetPos = Engine::GetInstance().map->MapToWorld((int)nextTile.getX(), (int)nextTile.getY());
//
//	// Centrar objetivo (+16 asumiendo tiles de 32)
//	targetPos.setX(targetPos.getX() + 16);
//	targetPos.setY(targetPos.getY() + 16);
//
//	Vector2D currentPos = GetPosition();
//
//	// 2. Calcular vector dirección (Destino - Origen)
//	float dirX = targetPos.getX() - currentPos.getX();
//	float dirY = targetPos.getY() - currentPos.getY();
//
//	// 3. Normalizar (Teorema de Pitágoras)
//	float length = sqrt(dirX * dirX + dirY * dirY);
//
//	if (length > 0) {
//		// Normalizamos y aplicamos velocidad
//		velocity.x = (dirX / length) * speed;
//		velocity.y = (dirY / length) * speed;
//
//		// Animación básica
//		if (velocity.x > 0) anims.SetCurrent("fly-right"); // O usa flip
//		else anims.SetCurrent("fly-left");
//	}
//	else {
//		velocity.x = 0;
//		velocity.y = 0;
//	}
//}
//// Enemy.cpp 底部
//
//
//void Enemy::MoveBoss() {
//	// 修复点：如果正在攻击，禁止移动，确保 Boss 站定播放攻击动画
//	if (isAttacking) {
//		velocity.x = 0;
//		// 如果 Boss 会飞或受重力影响，这里保留 Y 轴速度可能更自然，
//		// 但为了稳妥，也可以设为 0（如果是地面 Boss）
//		return;
//	}
//
//	// 2. 没路走 (或者路径被清除) -> Idle
//	if (pathfinding == nullptr || pathfinding->pathTiles.size() < 2) {
//		velocity.x = 0;
//		// 只有不是 idle 才切换，微小优化
//		if (anims.GetCurrentName() != "idle") {
//			anims.SetCurrent("idle");
//		}
//		return;
//	}
//
//	// 2. 获取下一个目标图块
//	auto it = pathfinding->pathTiles.rbegin();
//	it++; // 跳过起始点
//	Vector2D nextTile = *it;
//	Vector2D targetPos = Engine::GetInstance().map->MapToWorld((int)nextTile.getX(), (int)nextTile.getY());
//
//	// 修正中心点
//	targetPos.setX(targetPos.getX() + 16);
//	targetPos.setY(targetPos.getY() + 16);
//
//	Vector2D currentPos = GetPosition();
//	float xTolerance = 5.0f;
//
//	// 3. 移动逻辑
//	if (currentPos.getX() < targetPos.getX() - xTolerance) {
//		velocity.x = speed;
//		// 强制播放走路动画
//		if (anims.GetCurrentName() != "walk") anims.SetCurrent("walk");
//		flipState = SDL_FLIP_HORIZONTAL;
//	}
//	else if (currentPos.getX() > targetPos.getX() + xTolerance) {
//		velocity.x = -speed;
//		if (anims.GetCurrentName() != "walk") anims.SetCurrent("walk");
//		flipState = SDL_FLIP_NONE;
//	}
//	else {
//		velocity.x = 0;
//	}
//}
//void Enemy::ApplyPhysics() {
//
//	// Apply velocity via helper
//	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
//}
//
//void Enemy::Draw(float dt) {
//
//	anims.Update(dt);
//
//	// ================== 【新增：自动切换图片】 ==================
//	if (enemyType == EnemyType::BOSS) {
//		// 获取当前正在播放的动画名字 (例如 "idle", "attack")
//		std::string currentAnimName = anims.GetCurrentName();
//
//		// 如果这个名字在我们的素材库里，就切换 texture
//		if (bossTextures.find(currentAnimName) != bossTextures.end()) {
//			texture = bossTextures[currentAnimName];
//		}
//	}
//	// ==========================================================
//	const SDL_Rect& animFrame = anims.GetCurrentFrame();
//
//	// Update render position using your PhysBody helper
//
//   if (pbody != nullptr) {
//		int x, y;
//		pbody->GetPosition(x, y);
//		position.setX((float)x);
//		position.setY((float)y);
//	}
//
//	// Draw pathfinding debug
//
//	if (Engine::GetInstance().physics->IsDebug()) {
//		pathfinding->DrawPath();
//	}
//
//	// 额外的 offsetY：有些 Boss 素材脚下有很多空白，你可能需要微调这个值让它脚踩地
//	int drawOffsetX = animFrame.w / 2;
//	int drawOffsetY = animFrame.h / 2;
//
//	// 如果是 Boss，可能需要特殊的微调（根据你的图片实际情况调整这个 -10 或 +10）
//	if (enemyType == EnemyType::BOSS) {
//		// drawOffsetY += 10; // 举例：如果 Boss 看起来飘在空中，增加这个值
//	}
//
//
//
//	//Draw the player using the texture and the current animation frame
//
//	/*Engine::GetInstance().render->DrawTexture(texture, (int)position.getX() - texW / 2, (int)position.getY() - texH / 2, &animFrame,1.0f,0.0f,INT_MAX,INT_MAX,flipState);*/
//
//	Engine::GetInstance().render->DrawTexture(
//		texture,
//		(int)position.getX() - drawOffsetX,
//		(int)position.getY() - drawOffsetY,
//		&animFrame,
//		1.0f,
//		0.0f,
//		INT_MAX,
//		INT_MAX,
//		flipState
//	);
//
//}
//
//bool Enemy::CleanUp()
//{
//	LOG("Cleanup enemy");
//
//	// 1. Borrar física (ya lo tenías)
//	if (pbody != nullptr) {
//		pbody->listener = nullptr;
//		Engine::GetInstance().physics->DeletePhysBody(pbody);
//		pbody = nullptr;
//	}
//
//	// 2. Borrar Pathfinding (NUEVO E IMPORTANTE)
//	pathfinding = nullptr;
//
//	// 【新增】清理 Boss 的多张图片
//	for (auto& entry : bossTextures) {
//		if (entry.second != nullptr) {
//			Engine::GetInstance().textures->UnLoad(entry.second);
//		}
//	}
//	bossTextures.clear();
//	// 3. Descargar textura
//	/*if (texture != nullptr) {
//		Engine::GetInstance().textures->UnLoad(texture);
//		texture = nullptr;
//	}*/
//
//	return true;
//}
//
//void Enemy::SetPosition(Vector2D pos) {
//	pbody->SetPosition((int)(pos.getX()), (int)(pos.getY()));
//}
//
//Vector2D Enemy::GetPosition() {
//	int x, y;
//	pbody->GetPosition(x, y);
//	// Adjust for center
//	//return Vector2D((float)x-texW/2,(float)y-texH/2);
//	return Vector2D((float)x, (float)y);
//}
//
////Define OnCollision function for the enemy. 
//void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {
//	if (physB->ctype == ColliderType::PLATFORM) {
//		isGrounded = true;
//	}
//	if (physB->ctype == ColliderType::PROJECTILE) {
//		if (!isDead) {
//			isDead = true;
//			Engine::GetInstance().audio->PlayFx(deathFxId, 0, 5.0f);
//
//			// Desactiva físicas para que no siga chocando ni se mueva
//			Engine::GetInstance().physics->DeletePhysBody(pbody);
//			pbody = nullptr;
//
//			// Reproducir animación de muerte
//			// Según tu código actual, ya cargas "death" en el Start()
//			if (enemyType == EnemyType::FLYING) {
//				// El murciélago no tiene animación de muerte, lo destruimos ya.
//				Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
//			}
//			else {
//				// El Slime sí tiene animación "death", dejamos que se reproduzca.
//				anims.SetCurrent("death");
//			}
//		}
//	}
//	else if (physB->ctype == ColliderType::DEATH) {
//		if (!isDead) {
//			isDead = true;
//
//			// Desactiva físicas
//			Engine::GetInstance().physics->DeletePhysBody(pbody);
//			pbody = nullptr;
//
//			// Simula la muerte (igual que al recibir un proyectil)
//			if (enemyType == EnemyType::FLYING) {
//				Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
//			}
//			else {
//				anims.SetCurrent("death");
//			}
//		}
//	}
//}
//
//void Enemy::UpdateBossBehavior(float dt) {
//
//	// 只有在攻击状态下，我们要检查动画是否播放完毕
//	if (isAttacking) {
//		// 更新动画状态（必须在 Update 里调用 anims.Update，或者在这里确认状态）
//
//		if (anims.HasFinishedOnce()) {
//			switch (bossState) {
//			case BossState::ATTACK_NORMAL:
//				// 普通攻击结束 -> 回到空闲
//				isAttacking = false;
//				bossState = BossState::IDLE_CHASE;
//				anims.SetCurrent("idle");
//				break;
//
//			case BossState::SKILL_PRE:
//				// 大招前摇结束 -> 进入中段
//				bossState = BossState::SKILL_MID;
//				anims.SetCurrent("Midattack3");
//				break;
//
//			case BossState::SKILL_MID:
//				// 大招中段结束 -> 进入后摇
//				bossState = BossState::SKILL_END;
//				anims.SetCurrent("Endattack3");
//				break;
//
//			case BossState::SKILL_END:
//				// 大招完全结束 -> 回到空闲
//				isAttacking = false;
//				bossState = BossState::IDLE_CHASE;
//				anims.SetCurrent("idle");
//				break;
//
//			default:
//				break;
//			}
//		}
//	}
//	else {
//		// === 非攻击状态 (IDLE_CHASE) ===
//		// 1. 计时
//		normalAttackTimer += dt;
//		skillTimer += dt;
//
//		// 2. 检查大招 (优先级最高，11秒一次)
//		if (skillTimer >= 11.0f) {
//			skillTimer = 0.0f; // 重置计时
//			isAttacking = true;
//			bossState = BossState::SKILL_PRE;
//			velocity = { 0, 0 }; // 攻击时停止移动
//			anims.SetCurrent("Preattack3");
//			// 可选：播放大招音效
//		}
//		// 3. 检查普通攻击 (3秒一次)
//		else if (normalAttackTimer >= 3.0f) {
//			normalAttackTimer = 0.0f; // 重置计时
//			isAttacking = true;
//			bossState = BossState::ATTACK_NORMAL;
//			velocity = { 0, 0 }; // 攻击时停止移动
//
//			// 随机选择 attack1 或 attack2
//			if (rand() % 2 == 0) {
//				anims.SetCurrent("attack1");
//			}
//			else {
//				anims.SetCurrent("attack2");
//			}
//			// 可选：播放普攻音效
//		}
//	}
//}
//
//void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
//{
//	if (physB->ctype == ColliderType::PLATFORM) {
//		isGrounded = false;
//	}
//}
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

		// 1. 加载所有独立的图片
		bossTextures["idle"] = Engine::GetInstance().textures->Load("Assets/Textures/Bidle.png");
		bossTextures["attack1"] = Engine::GetInstance().textures->Load("Assets/Textures/Boss_Attack_1.png");
		bossTextures["attack2"] = Engine::GetInstance().textures->Load("Assets/Textures/Boss_Attack_2.png");
		bossTextures["walk"] = Engine::GetInstance().textures->Load("Assets/Textures/Boss_walk.png");
		bossTextures["hit"] = Engine::GetInstance().textures->Load("Assets/Textures/hit.png");
		bossTextures["Preattack3"] = Engine::GetInstance().textures->Load("Assets/Textures/Pre-Attack-3.png");
		bossTextures["Midattack3"] = Engine::GetInstance().textures->Load("Assets/Textures/mid-Attack-3.png");
		bossTextures["Endattack3"] = Engine::GetInstance().textures->Load("Assets/Textures/end-Attack-3.png");
		bossTextures["death"] = Engine::GetInstance().textures->Load("Assets/Textures/death.png");

		// 默认当前图片设为 idle
		texture = bossTextures["idle"];

		//------------------------------------------------------------------------Boss Anim-------------------------------------------------------------------------------//
		Animation idleAnim;
		// 因为是独立图片，第一帧通常就是从 0,0 开始！不用在大图里找位置了
	 // 【新增】第一帧左上角的 X 坐标 (前面的空白宽度)
		int frameWidth = 247;  // 每一帧的宽度
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
			int currentX = i * frameWidth;
			idleAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("idle", idleAnim);


		// 3. 定义 ATTACK 动画 (必须设置为不循环！)
		Animation attack1Anim;
		attack1Anim.SetLoop(false); // 【新增】设置为不循环
		for (int i = 0; i < frameCount2; i++) {
			int currentX = i * frameWidth;
			attack1Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 80);
		}
		anims.AddClip("attack1", attack1Anim);

		Animation attack2Anim;
		attack2Anim.SetLoop(false); // 【新增】设置为不循环
		for (int i = 0; i < frameCount3; i++) {
			int currentX = i * frameWidth;
			attack2Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 80);
		}
		anims.AddClip("attack2", attack2Anim);


		Animation Preattack3Anim;
		Preattack3Anim.SetLoop(false); // 【新增】设置为不循环
		for (int i = 0; i < frameCount6; i++) {
			int currentX = i * frameWidth;
			Preattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 80);
		}
		anims.AddClip("Preattack3", Preattack3Anim);


		Animation Midattack3Anim;
		Midattack3Anim.SetLoop(false); // 【新增】设置为不循环
		for (int i = 0; i < frameCount5; i++) {
			int currentX = i * frameWidth;
			Midattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 80);
		}
		anims.AddClip("Midattack3", Midattack3Anim);


		Animation Endattack3Anim;
		Endattack3Anim.SetLoop(false); // 【新增】设置为不循环
		for (int i = 0; i < frameCount7; i++) {
			int currentX = i * frameWidth;
			Endattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 80);
		}
		anims.AddClip("Endattack3", Endattack3Anim);


		// Walk Anim (走路需要循环)
		Animation WalkAnim;
		WalkAnim.SetLoop(true); // 显式设置为 true
		for (int i = 0; i < frameCount1; i++) {
			int currentX = i * frameWidth;
			WalkAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("walk", WalkAnim);

		// Hit and Death Anim
		Animation HitAnim;
		HitAnim.SetLoop(false); // 【新增】受伤不循环
		for (int i = 0; i < frameCount4; i++) {
			int currentX = i * frameWidth;
			HitAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("hit", HitAnim);

		Animation DeathAnim;
		DeathAnim.SetLoop(false); // 【新增】死亡不循环，停在最后一帧
		for (int i = 0; i < frameCount8; i++) {
			int currentX = i * frameWidth;
			DeathAnim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 150);
		}
		anims.AddClip("death", DeathAnim);


		// 设置初始状态
		anims.SetCurrent("idle");

		// 注意：这里创建了 Boss 专属的大碰撞体
		texW = 100; texH = 100;
		pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), 40, bodyType::DYNAMIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::ENEMY;
		b2Body_SetGravityScale(pbody->body, 1.0f);


		// ================== 【新增：Boss 独立参数设置】 ==================
		// 1. 设置更大的检测范围 (假设 1格 = 32像素，5格 = 160像素，或者直接增加半径数值)
		detectionRadius = 30; // 10 + 5格

		// 2. 性能优化：寻路计算间隔
		pathfindingInterval = 0.5f;

		// 3. Boss 移动速度
		speed = 2.0f; // 稍微慢一点显得有压迫感，或者快一点增加难度

		//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

	}
	else if (enemyType == EnemyType::FLYING) {
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

	// 确保 Map 模块已加载
	if (Engine::GetInstance().map != nullptr) {
		Vector2D tilePos = Engine::GetInstance().map->WorldToMap((int)pos.getX(), (int)pos.getY() + 1);
		pathfinding->ResetPath(tilePos);
	}

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
	// 1. 先更新行为状态
	if (enemyType == EnemyType::BOSS) {
		UpdateBossBehavior(dt);
	}

	// 2. 再执行寻路
	if (pathfinding != nullptr) {
		PerformPathfinding(dt);
	}

	// 3. 最后执行移动
	if (enemyType == EnemyType::GROUND) Move();
	else if (enemyType == EnemyType::FLYING) MoveFlying();
	else if (enemyType == EnemyType::BOSS) MoveBoss();
	else velocity = { 0.0f, 0.0f };

	ApplyPhysics();
	Draw(dt);

	return true;
}


//void Enemy::PerformPathfinding(float dt) {
//	// 1. 攻击状态不寻路
//	if (enemyType == EnemyType::BOSS && isAttacking) {
//		return;
//	}
//
//	// 获取位置
//	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
//	Vector2D playerTile = Engine::GetInstance().map->WorldToMap((int)playerPos.getX(), (int)playerPos.getY());
//
//	Vector2D myPos = GetPosition();
//
//	// === 【核心修复：前瞻偏移量】 ===
//	// 解决向右走时，因走过中心点导致寻路判定由于“回头”而产生的卡顿
//	float lookAheadX = 0.0f;
//	if (velocity.x > 1.0f) {
//		lookAheadX = 24.0f; // 向右走时，判定点前移 (约半个身位)
//	}
//	else if (velocity.x < -1.0f) {
//		lookAheadX = -24.0f; // 向左走时，判定点后移
//	}
//
//	// 加上偏移量 (yOffset 保持不变，检测脚下)
//	int yOffset = (enemyType == EnemyType::BOSS) ? 20 : 0;
//
//	// 注意这里把 lookAheadX 加到了 X 坐标上
//	Vector2D myTile = Engine::GetInstance().map->WorldToMap((int)(myPos.getX() + lookAheadX), (int)myPos.getY() + yOffset);
//
//	float distance = myTile.distanceEuclidean(playerTile);
//
//	// 2. 只有在距离范围内才尝试寻路
//	if (distance < detectionRadius) {
//		pathfindingTimer += dt;
//
//		if (pathfindingTimer >= pathfindingInterval) {
//			// 计算新路径
//			pathfinding->ComputeFullPathAStar(myTile, MANHATTAN);
//			pathfindingTimer = 0.0f;
//		}
//	}
//	else {
//		// 目标太远
//		pathfinding->pathTiles.clear();
//		velocity.x = 0;
//		if (anims.GetCurrentName() != "idle") anims.SetCurrent("idle");
//	}
//}

//void Enemy::PerformPathfinding(float dt) {
//	// 1. 攻击状态不寻路 (节省性能，防止动画被打断)
//	if (enemyType == EnemyType::BOSS && isAttacking) {
//		return;
//	}
//
//	// 获取玩家位置
//	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
//	Vector2D playerTile = Engine::GetInstance().map->WorldToMap((int)playerPos.getX(), (int)playerPos.getY());
//
//	// 获取 Boss 位置
//	Vector2D myPos = GetPosition();
//
//	// === 【修复：移除不稳定的 lookAheadX，回归中心点寻路】 ===
//	// 之前的前瞻偏移会导致 Boss 寻路起点在身体外面，产生“倒退”现象。
//	// 现在直接用身体中心+脚底偏移，最稳定。
//
//	int yOffset = (enemyType == EnemyType::BOSS) ? 20 : 0; // 脚底偏移
//
//	Vector2D myTile = Engine::GetInstance().map->WorldToMap((int)myPos.getX(), (int)myPos.getY() + yOffset);
//
//	float distance = myTile.distanceEuclidean(playerTile);
//
//	// 2. 只有在距离范围内才尝试寻路
//	if (distance < detectionRadius) {
//		pathfindingTimer += dt;
//
//		// 稍微加快寻路频率 (0.5s -> 0.3s) 让 Boss 反应更灵敏
//		if (pathfindingTimer >= 0.3f) {
//			// 计算新路径
//			pathfinding->ComputeFullPathAStar(myTile, MANHATTAN);
//			pathfindingTimer = 0.0f;
//		}
//	}
//	else {
//		// 目标太远，清空路径并停下
//		pathfinding->pathTiles.clear();
//		velocity.x = 0;
//		if (anims.GetCurrentName() != "idle") anims.SetCurrent("idle");
//	}
//}

// 在 Enemy.cpp 中替换 PerformPathfinding 函数

void Enemy::PerformPathfinding(float dt) {
	// 1. Boss 攻击状态不寻路
	if (enemyType == EnemyType::BOSS && isAttacking) {
		return;
	}

	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D myPos = GetPosition();

	Vector2D myTile;
	Vector2D targetTile;

	// === 分类处理寻路逻辑 ===
	if (enemyType == EnemyType::BOSS) {
		// --------------------------------------------------------
		// 【BOSS 逻辑】：强制地面寻路 (防止跳跃抽搐)
		// --------------------------------------------------------
		int yOffset = 20;
		// 起点：Boss 脚下
		myTile = Engine::GetInstance().map->WorldToMap((int)myPos.getX(), (int)myPos.getY() + yOffset);
		// 终点：玩家 X + Boss Y (只追踪水平位置，忽略玩家高度)
		targetTile = Engine::GetInstance().map->WorldToMap((int)playerPos.getX(), (int)myPos.getY() + yOffset);
	}
	else {
		// --------------------------------------------------------
		// 【普通/飞行怪 逻辑】：正常全方位寻路
		// --------------------------------------------------------
		// 起点：自身中心
		myTile = Engine::GetInstance().map->WorldToMap((int)myPos.getX(), (int)myPos.getY());
		// 终点：玩家真实位置 (飞行怪需要知道玩家在头顶)
		targetTile = Engine::GetInstance().map->WorldToMap((int)playerPos.getX(), (int)playerPos.getY());
	}

	// 计算距离
	float distance = myTile.distanceEuclidean(targetTile);

	// 2. 只有在距离范围内才尝试寻路
	if (distance < detectionRadius) {
		pathfindingTimer += dt;

		if (pathfindingTimer >= 0.3f) {
			pathfinding->ComputeFullPathAStar(myTile, targetTile, MANHATTAN);
			pathfindingTimer = 0.0f;
		}
	}
	else {
		// 目标太远，清空路径
		pathfinding->pathTiles.clear();
		velocity.x = 0;

		// 特殊处理：飞行怪需要停止 Y 轴移动
		if (enemyType == EnemyType::FLYING) {
			velocity.y = 0;
			// 飞行怪的待机动画叫 "idleflying"
			if (anims.GetCurrentName() != "idleflying") anims.SetCurrent("idleflying");
		}
		else {
			// 地面怪/Boss 的待机动画叫 "idle"
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
	if (pathfinding->pathTiles.size() > 1) {
		auto it = pathfinding->pathTiles.rbegin();
		it++; // Primer objetivo (inmediato)
		Vector2D nextTile = *it;
		Vector2D targetPos = Engine::GetInstance().map->MapToWorld((int)nextTile.getX(), (int)nextTile.getY());

		targetPos.setX(targetPos.getX() + 16);
		targetPos.setY(targetPos.getY() + 16);

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

	Vector2D currentPos = GetPosition();

	float dirX = targetPos.getX() - currentPos.getX();
	float dirY = targetPos.getY() - currentPos.getY();
	float length = sqrt(dirX * dirX + dirY * dirY);

	if (length > 0) {
		velocity.x = (dirX / length) * speed;
		velocity.y = (dirY / length) * speed;

		if (velocity.x > 0) anims.SetCurrent("fly-right");
		else anims.SetCurrent("fly-left");
	}
	else {
		velocity.x = 0;
		velocity.y = 0;
	}
}



// ================== 【优化后的 Boss 移动逻辑】 ==================
// 在 Enemy.cpp 中替换这个函数

//void Enemy::MoveBoss() {
//	// 1. 攻击状态完全停止移动
//	if (isAttacking) {
//		velocity.x = 0;
//		return;
//	}
//
//	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
//	Vector2D currentPos = GetPosition();
//	float distanceToPlayerX = abs(playerPos.getX() - currentPos.getX());
//
//	// 2. 如果已经在攻击范围内（或者非常接近），就停止移动
//	if (distanceToPlayerX <= attackRange - 10.0f) {
//		velocity.x = 0;
//
//		// 站立时确保面向玩家
//		if (anims.GetCurrentName() != "idle") anims.SetCurrent("idle");
//
//		if (playerPos.getX() > currentPos.getX()) flipState = SDL_FLIP_NONE;
//		else flipState = SDL_FLIP_HORIZONTAL;
//
//		return;
//	}
//
//	// 3. 寻路异常时的简单追踪逻辑
//	if (pathfinding == nullptr || pathfinding->pathTiles.size() < 2) {
//		float yDiff = abs(playerPos.getY() - currentPos.getY());
//		if (distanceToPlayerX < 500.0f && yDiff < 250.0f) {
//			float moveDir = playerPos.getX() - currentPos.getX();
//			if (moveDir > 10.0f) {
//				velocity.x = speed;
//				flipState = SDL_FLIP_NONE; // 向右：不翻转
//				if (anims.GetCurrentName() != "walk") anims.SetCurrent("walk");
//			}
//			else if (moveDir < -10.0f) {
//				velocity.x = -speed;
//				flipState = SDL_FLIP_HORIZONTAL; // 向左：翻转
//				if (anims.GetCurrentName() != "walk") anims.SetCurrent("walk");
//			}
//			else velocity.x = 0;
//		}
//		else {
//			velocity.x = 0;
//			if (anims.GetCurrentName() != "idle") anims.SetCurrent("idle");
//		}
//		return;
//	}
//
//	// 4. 标准寻路移动
//	Vector2D targetPos;
//	bool hasTarget = false;
//
//	while (pathfinding->pathTiles.size() > 1) {
//		auto it = pathfinding->pathTiles.rbegin();
//		it++;
//		Vector2D nextTile = *it;
//
//		targetPos = Engine::GetInstance().map->MapToWorld((int)nextTile.getX(), (int)nextTile.getY());
//		targetPos.setX(targetPos.getX() + 16);
//		targetPos.setY(targetPos.getY() + 16);
//
//		float distX = targetPos.getX() - currentPos.getX();
//
//		// 防回头检查
//		bool isMovingRightAndTargetIsLeft = (velocity.x > 0.5f && distX < -5.0f);
//		bool isMovingLeftAndTargetIsRight = (velocity.x < -0.5f && distX > 5.0f);
//
//		if (abs(distX) < 8.0f || isMovingRightAndTargetIsLeft || isMovingLeftAndTargetIsRight) {
//			pathfinding->pathTiles.pop_back();
//		}
//		else {
//			hasTarget = true;
//			break;
//		}
//	}
//
//	if (!hasTarget) {
//		velocity.x = 0;
//		return;
//	}
//
//	// 执行最终移动
//	float xTolerance = 2.0f;
//
//	// === 【修复点在这里】 ===
//	if (currentPos.getX() < targetPos.getX() - xTolerance) {
//		// 目标在右边 -> 速度为正 -> 面向右(无翻转)
//		velocity.x = speed;
//		if (anims.GetCurrentName() != "walk") anims.SetCurrent("walk");
//		flipState = SDL_FLIP_NONE; // 之前这里写成了 HORIZONTAL
//	}
//	else if (currentPos.getX() > targetPos.getX() + xTolerance) {
//		// 目标在左边 -> 速度为负 -> 面向左(水平翻转)
//		velocity.x = -speed;
//		if (anims.GetCurrentName() != "walk") anims.SetCurrent("walk");
//		flipState = SDL_FLIP_HORIZONTAL; // 之前这里写成了 NONE
//	}
//	else {
//		velocity.x = 0;
//	}
//
//	// 跳跃逻辑
//	if (isGrounded && targetPos.getY() < currentPos.getY() - 32.0f) {
//		velocity.y = -jumpForce;
//		isGrounded = false;
//	}
//}
//void Enemy::MoveBoss() {
//	// 1. 攻击状态完全停止移动 (包括 X 和 Y)
//	if (isAttacking) {
//		velocity.x = 0;
//		// 如果不希望 Boss 攻击时跳跃或下落，可以把 velocity.y 也锁住，但通常保留重力比较自然
//		return;
//	}
//
//	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
//	Vector2D currentPos = GetPosition();
//	float distanceToPlayerX = abs(playerPos.getX() - currentPos.getX());
//
//	// 2. 攻击范围判定：如果已经很近了，就停下准备攻击
//	// 这里的判断范围要比 Trigger 攻击的范围稍微小一点点(90 vs 100)，防止鬼畜抖动
//	if (distanceToPlayerX <= attackRange - 10.0f) {
//		velocity.x = 0;
//
//		// 站立时确保面向玩家
//		if (anims.GetCurrentName() != "idle") anims.SetCurrent("idle");
//
//		// 简单的面向判断
//		if (playerPos.getX() > currentPos.getX()) flipState = SDL_FLIP_NONE;
//		else flipState = SDL_FLIP_HORIZONTAL;
//
//		return; // 已经到位，不需要再执行寻路移动
//	}
//
//	// 3. 寻路异常处理 / 简单视距追踪
//	// 如果寻路失败，但玩家很近，直接走直线，防止发呆
//	if (pathfinding == nullptr || pathfinding->pathTiles.size() < 2) {
//		float yDiff = abs(playerPos.getY() - currentPos.getY());
//
//		// 距离近且高度差不大时，强行直线追踪
//		if (distanceToPlayerX < 500.0f && yDiff < 250.0f) {
//			float moveDir = playerPos.getX() - currentPos.getX();
//			if (moveDir > 10.0f) {
//				velocity.x = speed;
//				flipState = SDL_FLIP_NONE; // 向右走，不翻转
//				if (anims.GetCurrentName() != "walk") anims.SetCurrent("walk");
//			}
//			else if (moveDir < -10.0f) {
//				velocity.x = -speed;
//				flipState = SDL_FLIP_HORIZONTAL; // 向左走，翻转
//				if (anims.GetCurrentName() != "walk") anims.SetCurrent("walk");
//			}
//			else velocity.x = 0;
//		}
//		else {
//			velocity.x = 0;
//			if (anims.GetCurrentName() != "idle") anims.SetCurrent("idle");
//		}
//		return;
//	}
//
//	// 4. 标准寻路移动
//	Vector2D targetPos;
//	bool hasTarget = false;
//
//	// 从路径中取下一个点
//	while (pathfinding->pathTiles.size() > 1) {
//		auto it = pathfinding->pathTiles.rbegin();
//		it++; // 第一个点是自己，取第二个点
//		Vector2D nextTile = *it;
//
//		targetPos = Engine::GetInstance().map->MapToWorld((int)nextTile.getX(), (int)nextTile.getY());
//		targetPos.setX(targetPos.getX() + 16);
//		targetPos.setY(targetPos.getY() + 16);
//
//		float distX = targetPos.getX() - currentPos.getX();
//
//		// === 防回头机制 ===
//		// 如果我们正向右跑，但下一个路径点却在左边很远，说明路径过时了，删掉这个点
//		bool isMovingRightAndTargetIsLeft = (velocity.x > 0.5f && distX < -10.0f);
//		bool isMovingLeftAndTargetIsRight = (velocity.x < -0.5f && distX > 10.0f);
//
//		if (abs(distX) < 5.0f || isMovingRightAndTargetIsLeft || isMovingLeftAndTargetIsRight) {
//			pathfinding->pathTiles.pop_back(); // 还没到或者点废了，删掉，找下一个
//		}
//		else {
//			hasTarget = true;
//			break; // 这个点有效，走起
//		}
//	}
//
//	if (!hasTarget) {
//		velocity.x = 0;
//		return;
//	}
//
//	// 5. 执行移动 & 设置朝向
//	float xTolerance = 2.0f;
//
//	if (currentPos.getX() < targetPos.getX() - xTolerance) {
//		// === 往右走 ===
//		velocity.x = speed;
//		if (anims.GetCurrentName() != "walk") anims.SetCurrent("walk");
//		flipState = SDL_FLIP_NONE; // 面向右 (假设素材默认朝右)
//	}
//	else if (currentPos.getX() > targetPos.getX() + xTolerance) {
//		// === 往左走 ===
//		velocity.x = -speed;
//		if (anims.GetCurrentName() != "walk") anims.SetCurrent("walk");
//		flipState = SDL_FLIP_HORIZONTAL; // 面向左
//	}
//	else {
//		velocity.x = 0;
//	}
//
//	// 6. 简单的跳跃辅助 (如果目标点在上方)
//	if (isGrounded && targetPos.getY() < currentPos.getY() - 32.0f) {
//		velocity.y = -jumpForce;
//		isGrounded = false;
//	}
//}
void Enemy::MoveBoss() {
	// ============================================================
	// 1. 攻击状态完全停止移动
	// ============================================================
	if (isAttacking) {
		velocity.x = 0;
		return;
	}

	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D currentPos = GetPosition();
	float distanceToPlayerX = abs(playerPos.getX() - currentPos.getX());

	// ============================================================
	// 2. 攻击范围判定：如果已经很近了，就停下准备攻击
	// ============================================================
	if (distanceToPlayerX <= attackRange - 10.0f) {
		velocity.x = 0;

		// 站立时确保面向玩家
		if (anims.GetCurrentName() != "idle") anims.SetCurrent("idle");

		if (playerPos.getX() > currentPos.getX()) flipState = SDL_FLIP_NONE;
		else flipState = SDL_FLIP_HORIZONTAL;

		return;
	}

	// ============================================================
	// 3. 寻路异常处理
	// ============================================================
	// 【核心修复】：如果没路可走（pathTiles为空），直接 Idle，不要瞎跑！
	// 之前的“简单追踪”会导致 Boss 在远距离或隔墙时一直撞墙抽搐。
	if (pathfinding == nullptr || pathfinding->pathTiles.size() < 2) {
		velocity.x = 0;
		if (anims.GetCurrentName() != "idle") anims.SetCurrent("idle");

		// 可选：即使站着不动，也看着玩家
		if (playerPos.getX() > currentPos.getX()) flipState = SDL_FLIP_NONE;
		else flipState = SDL_FLIP_HORIZONTAL;

		return;
	}

	// ============================================================
	// 4. 标准寻路移动
	// ============================================================
	Vector2D targetPos;
	bool hasTarget = false;

	while (pathfinding->pathTiles.size() > 1) {
		auto it = pathfinding->pathTiles.rbegin();
		it++; // 取下一个路径点
		Vector2D nextTile = *it;

		targetPos = Engine::GetInstance().map->MapToWorld((int)nextTile.getX(), (int)nextTile.getY());
		// 居中校正 (+16)
		targetPos.setX(targetPos.getX() + 16);
		targetPos.setY(targetPos.getY() + 16);

		float distX = targetPos.getX() - currentPos.getX();

		// 防回头机制：如果已经走过头了，就删掉这个点，去下一个
		bool isMovingRightAndTargetIsLeft = (velocity.x > 0.5f && distX < -10.0f);
		bool isMovingLeftAndTargetIsRight = (velocity.x < -0.5f && distX > 10.0f);

		// 容差稍微加大到 5.0f，防止在节点附近抖动
		if (abs(distX) < 5.0f || isMovingRightAndTargetIsLeft || isMovingLeftAndTargetIsRight) {
			pathfinding->pathTiles.pop_back();
		}
		else {
			hasTarget = true;
			break;
		}
	}

	// 如果所有点都过滤完了，就停下
	if (!hasTarget) {
		velocity.x = 0;
		if (anims.GetCurrentName() != "idle") anims.SetCurrent("idle");
		return;
	}

	// ============================================================
	// 5. 执行移动 & 设置朝向
	// ============================================================
	float xTolerance = 4.0f; // 加大容差，避免在目标点左右反复横跳

	if (currentPos.getX() < targetPos.getX() - xTolerance) {
		// === 往右走 ===
		velocity.x = speed;
		if (anims.GetCurrentName() != "walk") anims.SetCurrent("walk");
		flipState = SDL_FLIP_NONE;
	}
	else if (currentPos.getX() > targetPos.getX() + xTolerance) {
		// === 往左走 ===
		velocity.x = -speed;
		if (anims.GetCurrentName() != "walk") anims.SetCurrent("walk");
		flipState = SDL_FLIP_HORIZONTAL;
	}
	else {
		// 到达 X 轴目标，暂停水平移动
		velocity.x = 0;
	}

	// 6. 简单的跳跃辅助
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

	// 自动切换 Boss 图片
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

	// === 调试绘制 ===
	if (Engine::GetInstance().physics->IsDebug()) {
		pathfinding->DrawPath();

		Vector2D centerPos = GetPosition();

		// 1. 绘制【脚下】判定点 (绿色) - 这是 Boss 寻路用的点
		// 我们之前在 PerformPathfinding 里加了 offset (20或40)，这里画出来确认一下
		int yOffset = (enemyType == EnemyType::BOSS) ? 20 : 0; // 必须和 PerformPathfinding 里的 offset 一致
		Vector2D feetTile = Engine::GetInstance().map->WorldToMap((int)centerPos.getX(), (int)centerPos.getY() + yOffset);
		Vector2D feetWorld = Engine::GetInstance().map->MapToWorld((int)feetTile.getX(), (int)feetTile.getY());

		SDL_Rect feetRect = { (int)feetWorld.getX(), (int)feetWorld.getY(), 32, 32 };
		Engine::GetInstance().render->DrawRectangle(feetRect, 0, 255, 0, 150); // 绿色

		// 2. 绘制【中心】判定点 (红色) - 这是 Boss 身体中心
		Vector2D centerTile = Engine::GetInstance().map->WorldToMap((int)centerPos.getX(), (int)centerPos.getY());
		Vector2D centerWorld = Engine::GetInstance().map->MapToWorld((int)centerTile.getX(), (int)centerTile.getY());

		SDL_Rect centerRect = { (int)centerWorld.getX(), (int)centerWorld.getY(), 32, 32 };
		Engine::GetInstance().render->DrawRectangle(centerRect, 255, 0, 0, 150); // 红色
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
	int x, y;
	pbody->GetPosition(x, y);
	return Vector2D((float)x, (float)y);
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	if (physB->ctype == ColliderType::PLATFORM) {
		isGrounded = true;
	}
	if (physB->ctype == ColliderType::PROJECTILE) {
		if (!isDead) {
			isDead = true;
			Engine::GetInstance().audio->PlayFx(deathFxId, 0, 5.0f);

			Engine::GetInstance().physics->DeletePhysBody(pbody);
			pbody = nullptr;

			if (enemyType == EnemyType::FLYING) {
				Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
			}
			else {
				anims.SetCurrent("death");
			}
		}
	}
	else if (physB->ctype == ColliderType::DEATH) {
		if (!isDead) {
			isDead = true;
			Engine::GetInstance().physics->DeletePhysBody(pbody);
			pbody = nullptr;

			if (enemyType == EnemyType::FLYING) {
				Engine::GetInstance().entityManager->DestroyEntity(shared_from_this());
			}
			else {
				anims.SetCurrent("death");
			}
		}
	}
}

// === Boss 行为逻辑 ===
// ================== 【优化后的 Boss 行为逻辑】 ==================
void Enemy::UpdateBossBehavior(float dt) {
	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D myPos = GetPosition();

	// 1. 计算距离
	float distanceToPlayerX = abs(playerPos.getX() - myPos.getX());
	float yDiff = abs(playerPos.getY() - myPos.getY());

	Player* player = Engine::GetInstance().scene->player.get();

	// ===================================================
	// 2. 正在攻击中 (优先级最高，攻击未结束前不进行其他逻辑)
	// ===================================================
	if (isAttacking) {
		velocity.x = 0; // 锁死移动
		int currentFrame = anims.GetCurrentFrameIndex();

		// --- 伤害判定优化 ---
		// 我们确保只造成一次伤害 (!hasDealtDamage)
		// 判定帧：第 3 帧到第 6 帧之间视为“攻击生效期”
		if (!hasDealtDamage && currentFrame >= 3 && currentFrame <= 6) {
			// 再次检查范围：如果玩家还在攻击范围内 (宽容度 +30 像素)
			if (distanceToPlayerX < attackRange + 30.0f && yDiff < 60.0f) {
				if (player != nullptr) {
					player->TakeDamage(attackDamage);
					LOG("BOSS ATTACK HIT PLAYER!"); // 调试日志
				}
				hasDealtDamage = true;
			}
		}

		// --- 检查动画是否播放完毕 ---
		if (anims.HasFinishedOnce()) {
			isAttacking = false;              // 退出攻击状态
			bossState = BossState::IDLE_CHASE; // 切换回追逐状态
			attackCooldownTimer = attackCooldown; // 重置冷却时间
			anims.SetCurrent("idle");         // 播放待机动画
		}
		return; // 攻击期间直接返回
	}

	// ===================================================
	// 3. 冷却倒计时
	// ===================================================
	if (attackCooldownTimer > 0.0f) {
		attackCooldownTimer -= dt;
	}

	// ===================================================
	// 4. 触发攻击判定 (不在攻击中 + 距离够近 + 冷却完毕)
	// ===================================================
	if (distanceToPlayerX <= attackRange && attackCooldownTimer <= 0.0f && yDiff < 60.0f) {

		// --- 准备攻击 ---
		isAttacking = true;
		hasDealtDamage = false;
		velocity.x = 0; // 攻击瞬间停止移动

		// 面向修正：攻击前最后确认一下朝向，防止背对玩家攻击
		if (playerPos.getX() > myPos.getX()) flipState = SDL_FLIP_NONE;
		else flipState = SDL_FLIP_HORIZONTAL;

		// ================== 【核心修改：随机选择攻击动画】 ==================
		// 使用随机数 0 或 1 来决定使用哪一个攻击
		// 这样 Boss 就会不可预测地使用 Attack1 或 Attack2
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
		// ================================================================
	}
	else {
		// 不满足攻击条件时，保持追逐/空闲状态
		bossState = BossState::IDLE_CHASE;
	}
}
void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	if (physB->ctype == ColliderType::PLATFORM) {
		isGrounded = false;
	}
}