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
	/*position = startPosition;*/
	// load
	std::unordered_map<int, std::string> aliases = { {0,"idle"} };
	anims.LoadFromTSX("Assets/Textures/enemy_spritesheet.tsx", aliases);
	anims.SetCurrent("idle");
	
	//Initialize Player parameters
	texture = Engine::GetInstance().textures->Load("Assets/Textures/enemy_spritesheet.png");

	//Add physics to the enemy - initialize physics body
	texW = 32;
	texH = 32;
	/*pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX()+texW/2, (int)position.getY()+texH/2, texW / 2, bodyType::DYNAMIC);*/

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
	PerformPathfinding();
	GetPhysicsValues();
	if (enemyType == EnemyType::GROUND) {
		MoveAndJump();
	}
	else if (enemyType == EnemyType::FLYING) {
		MoveFlying();
	}
	ApplyPhysics();
	Draw(dt);

	return true;
}

void Enemy::PerformPathfinding() {

	// Pathfinding testing inputs

	// Reset pathfinding with R key
	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D playerTile = Engine::GetInstance().map->WorldToMap((int)playerPos.getX(), (int)playerPos.getY());

	// 2. Obtener mi posición (Origen)
	Vector2D myPos = GetPosition();
	Vector2D myTile = Engine::GetInstance().map->WorldToMap((int)myPos.getX(), (int)myPos.getY());

	float distance = myTile.distanceEuclidean(playerTile);
	if (distance < 10) {
		// 3. Calcular el camino automáticamente
// Usamos MANHATTAN como heurística por defecto
		pathfinding->ComputeFullPathAStar(myTile, MANHATTAN);
	}
	else {
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
	// ERROR ANTERIOR: velocity = { 0, velocity.y };
	if (enemyType == EnemyType::GROUND && isGrounded) {
		velocity.x = 0;
	}
}



void Enemy::MoveAndJump() {
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
			anims.SetCurrent("run");
			// flipState = SDL_FLIP_NONE; 
		}
		else if (currentPos.getX() > targetPos.getX() + xTolerance) {
			velocity.x = -speed;
			anims.SetCurrent("run");
			// flipState = SDL_FLIP_HORIZONTAL;
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
		if (velocity.x > 0) anims.SetCurrent("fly_right"); // O usa flip
		else anims.SetCurrent("fly_left");
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
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	// Update render position using your PhysBody helper
	int x, y;
	
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	// Draw pathfinding debug
	
	if (Engine::GetInstance().physics->IsDebug()) {
		pathfinding->DrawPath();
	}
	//Draw the player using the texture and the current animation frame
	Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2, &animFrame);
	
}

bool Enemy::CleanUp()
{
	LOG("Cleanup enemy");
	Engine::GetInstance().textures->UnLoad(texture);
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
}

void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	if (physB->ctype == ColliderType::PLATFORM) {
		isGrounded = false;
	}
}
