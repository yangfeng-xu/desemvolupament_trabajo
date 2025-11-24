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

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name = "Enemy";
}

Enemy::~Enemy() {

}

bool Enemy::Awake() {
	return true;
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

	// Initialize pathfinding
	pathfinding = std::make_shared<Pathfinding>();
	//Get the position of the enemy
	Vector2D pos = GetPosition();
	//Convert to tile coordinates
	Vector2D tilePos = Engine::GetInstance().map->WorldToMap((int)pos.getX(), (int)pos.getY()+1);
	//Reset pathfinding
	pathfinding->ResetPath(tilePos);

	return true;
}

bool Enemy::Update(float dt)
{
	PerformPathfinding();
	GetPhysicsValues();
	MoveAndJump();
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

	// 3. Calcular el camino automáticamente
	// Usamos MANHATTAN como heurística por defecto
	pathfinding->ComputeFullPathAStar(myTile, MANHATTAN);
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
	velocity = { 0, velocity.y }; 
}

//void Enemy::MoveAndJump() {
//	if (pathfinding->pathTiles.size() > 1) {
//
//		// El camino se guarda desde el Final hasta el Inicio.
//		// El último elemento es donde estamos (Inicio).
//		// El penúltimo elemento es el Siguiente Paso.
//
//		// Iteramos para coger el penúltimo
//		auto it = pathfinding->pathTiles.rbegin(); // Apunta al final (Inicio)
//		it++; // Avanzamos uno (Siguiente Paso)
//
//		Vector2D nextTile = *it;
//
//		// Convertimos la tile destino a pixeles (Centro del tile)
//		Vector2D nextPos = Engine::GetInstance().map->MapToWorld((int)nextTile.getX(), (int)nextTile.getY());
//		// Ajustamos al centro de la baldosa (32/2 = 16)
//		nextPos.setX(nextPos.getX() + 16);
//		nextPos.setY(nextPos.getY() + 16);
//
//		//necesitamos un auxiliar para guardar el posición del enemigo actualmente
//		Vector2D currentPos = GetPosition();
//
//
//		//Jump
//		const float TILE_HEIGHT = 32.0f;
//		if (nextPos.getY() < (currentPos.getY() - TILE_HEIGHT * 0.5f) && Engine::GetInstance().physics->isOnGround(pbody)) {
//			Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -jumpForce, true);
//		}
//
//
//		// Decidimos dirección
//		if (GetPosition().getX() < nextPos.getX()) {
//			velocity.x = speed; // Ir derecha
//		}
//		else if (GetPosition().getX() > nextPos.getX()) {
//			velocity.x = -speed; // Ir izquierda
//		}
//		
//
//		float distX = std::abs(currentPos.getX() - nextPos.getX());
//		float distY = std::abs(currentPos.getY() - nextPos.getY());
//
//		float TILE_TOLERANCE = 5.0f;
//		// Si estamos cerca en X y (cerca en Y O ya hemos superado la altura destino hacia arriba)
//		if (distX < TILE_TOLERANCE && (distY < TILE_TOLERANCE || currentPos.getY() < nextPos.getY())) {
//			pathfinding->pathTiles.pop_back();
//		}
//
//
//	}
//	// Move 
//}
void Enemy::MoveAndJump() {
	if (pathfinding->pathTiles.size() > 1) {

		// --- Tu estructura original ---
		auto it = pathfinding->pathTiles.rbegin();
		it++;

		Vector2D nextTile = *it;

		Vector2D nextPos = Engine::GetInstance().map->MapToWorld((int)nextTile.getX(), (int)nextTile.getY());
		nextPos.setX(nextPos.getX() + 16);
		nextPos.setY(nextPos.getY() + 16);

		Vector2D currentPos = GetPosition();
		const float TILE_HEIGHT = 32.0f;


		// --- Jump (Igual que tu código) ---
		// Detectamos si hay que saltar
		bool targetIsAbove = nextPos.getY() < (currentPos.getY() - TILE_HEIGHT * 0.5f);

		if (targetIsAbove && Engine::GetInstance().physics->isOnGround(pbody)) {
			Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -jumpForce, true);
		}


		// --- Decidimos dirección (AQUÍ ESTÁ LA CORRECCIÓN) ---

		// Añadimos una pequeña tolerancia (2.0f) para que no vibre al alinearse
		if (GetPosition().getX() < nextPos.getX() - 2.0f) {
			velocity.x = speed; // Ir derecha
		}
		else if (GetPosition().getX() > nextPos.getX() + 2.0f) {
			velocity.x = -speed; // Ir izquierda
		}
		else {
			// CAMBIO IMPORTANTE:
			// Si la X está alineada, normalmente pararíamos (velocity.x = 0).
			// PERO si el objetivo está ARRIBA (targetIsAbove), NO debemos parar,
			// o el enemigo saltará recto como un muelle y no avanzará.

			if (targetIsAbove) {
				// Si hay que subir, forzamos movimiento hacia donde esté el Player real
				float playerX = Engine::GetInstance().player->GetPosition().getX();
				if (playerX > currentPos.getX()) velocity.x = speed;
				else velocity.x = -speed;
			}
			else {
				velocity.x = 0; // Solo paramos si NO hay que subir
			}
		}


		// --- Tu lógica de limpieza (Pop Back) ---
		const float TILE_TOLERANCE = 5.0f;

		// Usamos abs para distancias positivas
		bool nearX = std::abs(currentPos.getX() - nextPos.getX()) < TILE_TOLERANCE;
		// Corregido ligeramente: si ya superamos la altura (estamos más arriba o igual en Y)
		bool nearYorAbove = (currentPos.getY() <= nextPos.getY() + TILE_TOLERANCE);

		if (nearX && nearYorAbove) {
			pathfinding->pathTiles.pop_back();
		}

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
	
	pathfinding->DrawPath();
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

}

void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{

}
