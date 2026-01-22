
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

		Animation attack1Anim;
		attack1Anim.SetLoop(false);
		for (int i = 0; i < frameCount2; i++) {
			int currentX = i * frameWidth;
			attack1Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 80);
		}
		anims.AddClip("attack1", attack1Anim);

		Animation attack2Anim;
		attack2Anim.SetLoop(false); 
		for (int i = 0; i < frameCount3; i++) {
			int currentX = i * frameWidth;
			attack2Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 80);
		}
		anims.AddClip("attack2", attack2Anim);


		Animation Preattack3Anim;
		Preattack3Anim.SetLoop(false); 
		for (int i = 0; i < frameCount6; i++) {
			int currentX = i * frameWidth;
			Preattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 80);
		}
		anims.AddClip("Preattack3", Preattack3Anim);


		Animation Midattack3Anim;
		Midattack3Anim.SetLoop(false); 
		for (int i = 0; i < frameCount5; i++) {
			int currentX = i * frameWidth;
			Midattack3Anim.AddFrame({ currentX, 0, frameWidth, frameHeight }, 80);
		}
		anims.AddClip("Midattack3", Midattack3Anim);


		Animation Endattack3Anim;
		Endattack3Anim.SetLoop(false); 
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

		// === 新增/修改：设置 Boss 血量 ===
		maxHealth = 20;  // 设置最大血量为 20
		health = maxHealth; // 当前血量初始化

	/*	SetBounds(1952, 736);*/

		// ================== 【新增：设置 Level 2 Boss 决战圈】 ==================
		// 这里的数值你需要修改成你地图里实际的像素坐标！
		// 假设：Level 2 的决战空地是从 X=3000 到 X=3800

		hasBattleArea = true;    // 开启限制
		battleAreaMinX = 2048;   // 【请修改】空地最左边的 X 坐标
		battleAreaMaxX = 2944;   // 【请修改】空地最右边的 X 坐标

		// 提示：如果不确定，可以先填大概的数，运行游戏微调
		// =================================================================
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
	ZoneScoped;

	if (isDead && pbody != nullptr) {
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		pbody = nullptr;
	}
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

	// ================== 【新增：强制限制 Boss 在决战圈内】 ==================
	if (hasBattleArea) {
		Vector2D currentPos = GetPosition();

		// 1. 检查左边界
		if (currentPos.getX() < battleAreaMinX) {
			// 强制拉回到左边界
			Vector2D fixedPos = currentPos;
			fixedPos.setX((float)battleAreaMinX);
			SetPosition(fixedPos); // 更新物理身体位置

			// 如果它还在往左跑，把速度归零
			if (velocity.x < 0) velocity.x = 0;
		}

		// 2. 检查右边界
		else if (currentPos.getX() > battleAreaMaxX) {
			// 强制拉回到右边界
			Vector2D fixedPos = currentPos;
			fixedPos.setX((float)battleAreaMaxX);
			SetPosition(fixedPos); // 更新物理身体位置

			// 如果它还在往右跑，把速度归零
			if (velocity.x > 0) velocity.x = 0;
		}
	}
	// ====================================================================


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
	/*	pathfinding->DrawPath();*/
		if (pathfinding != nullptr) {
			pathfinding->DrawPath();
		}

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

	// === 新增：Boss 血条绘制逻辑 ===
	if (enemyType == EnemyType::BOSS && !isDead) {
		// 1. 定义血条尺寸
		int barWidth = 60;  // 血条总宽度
		int barHeight = 8;  // 血条高度
		int yOffset = 50;   // 距离 Boss 中心的垂直偏移量 (向上)

		// 2. 计算位置 (居中显示在 Boss 头顶)
		// position 是物理身体的中心点
		int barX = (int)position.getX() - (barWidth / 2);
		int barY = (int)position.getY() - yOffset;

		// 3. 计算当前血量比例
		float healthPercentage = (float)health / (float)maxHealth;
		// 确保不小于 0
		if (healthPercentage < 0) healthPercentage = 0;

		// 4. 绘制背景 (红色或黑色)
		SDL_Rect bgRect = { barX, barY, barWidth, barHeight };
		Engine::GetInstance().render->DrawRectangle(bgRect, 255, 0, 0, 255); // 红色背景

		// 5. 绘制前景 (绿色 - 根据血量缩放宽度)
		SDL_Rect fgRect = { barX, barY, (int)(barWidth * healthPercentage), barHeight };
		Engine::GetInstance().render->DrawRectangle(fgRect, 0, 255, 0, 255); // 绿色前景

		// (可选) 绘制边框 (白色)
		// Engine::GetInstance().render->DrawRectangle(bgRect, 255, 255, 255, 255, false); 
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
	// 如果物理身体没了，就返回最后记录的坐标
	return position;
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	if (physB->ctype == ColliderType::PLATFORM) {
		isGrounded = true;
	}

	//}// === 修改开始：子弹碰撞逻辑 ===
	if (physB->ctype == ColliderType::PROJECTILE) {
		if (!isDead) {
			//如果是 Boss，扣血而不是直接死
			if (enemyType == EnemyType::BOSS) {
				health--; // 假设子弹伤害为 1
				LOG("Boss Hit! HP: %d / %d", health, maxHealth);

				// 可以加一个受伤音效或变色效果
				// Engine::GetInstance().audio->PlayFx(hitFxId); 

				if (health <= 0) {
					active = false;
					isDead = true;
					Engine::GetInstance().audio->PlayFx(deathFxId, 0, 5.0f);
					//Engine::GetInstance().physics->DeletePhysBody(pbody);
					//pbody = nullptr;
					anims.SetCurrent("death"); // 播放死亡动画
				}
			}
			// 如果是普通敌人，直接死亡 (保持原有逻辑)
			else {
				isDead = true;
				Engine::GetInstance().audio->PlayFx(deathFxId, 0, 5.0f);
				/*Engine::GetInstance().physics->DeletePhysBody(pbody);
				pbody = nullptr;*/

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
		/*	Engine::GetInstance().physics->DeletePhysBody(pbody);
			pbody = nullptr;*/

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