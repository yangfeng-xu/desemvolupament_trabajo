#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"
#include"Map.h"
#include"Animation.h"
#include"Projectile.h"
#include <box2d/box2d.h> 
#include "Item.h"


Player::Player() : Entity(EntityType::PLAYER), IsDead(false)
{
	name = "Player";
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: TODO 2: Initialize Player parameters
	position = Vector2D(100, 600);
	startPosition = position;
	return true;
}

bool Player::Start() {

	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures->Load("Assets/Textures/player_sprite.png");

	std::unordered_map<int, std::string>animNames = { {13,"death"},{21,"run"},{39,"idle"},{156,"jump"} };
	anims.LoadFromTSX("Assets/Textures/player_sprite2.tsx", animNames);
	anims.SetCurrent("idle");

	//heart system
	heartTexture = Engine::GetInstance().textures->Load("Assets/Textures/heart.png");
	lives = maxLives;
	invulnerabilityTimer = 0.0f;

	score = 0;
	ammo = 15;

	// L08 TODO 5: Add physics to the player - initialize physics body
	startPosition = position;
	savePosition = position;
	texW = 32;
	texH = 32;

	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect
	pickCoinFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/coin-collision-sound-342335.wav");
	deathFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/out.wav");
	jumpFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/salto.wav"); // Load the skip audio
	saveFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/autosave.wav"); // Load the save audio
	return true;
}

bool Player::Update(float dt)
{
	ZoneScoped;

	if (Engine::GetInstance().scene->IsGameOver()) {
		b2Vec2 stopVel = { 0.0f, 0.0f };
		b2Body_SetLinearVelocity(pbody->body, stopVel);
		return true;
	}
	anims.Update(dt);
	if (invulnerabilityTimer > 0.0f) {
		invulnerabilityTimer -= dt;
	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		LOG("Debug: Returning to Start Position (F9)");

		// 1. Force the physical body into the starting position
		// We use startPosition for respawn

		b2BodyId body = pbody->body;
		b2Vec2 startPosMeters = {
			PIXEL_TO_METERS(startPosition.getX() + texW / 2),
			PIXEL_TO_METERS(startPosition.getY() + texH / 2)
		};
		b2Body_SetTransform(body, startPosMeters, b2MakeRot(0.0f));

		// 2. Stop any movement/jump
		b2Vec2 vel = { 0.0f, 0.0f };
		b2Body_SetLinearVelocity(body, vel);
		b2Body_SetAwake(body, true);// Ensure that the body awakens

		// 3. Reset states 
		isJumping = false;
		IsDead = false;
		anims.SetCurrent("idle");

		// 4. Force the camera to follow the initial position
		float limitLeft = Engine::GetInstance().render->camera.w / 4;
		float cameraX = -(startPosition.getX() - limitLeft);
		Engine::GetInstance().render->camera.x = (int)cameraX;
		Engine::GetInstance().render->camera.y = 0;
	}

	// LOGIC DIOS MODE (F10)
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		isGodMode = !isGodMode;
		if (isGodMode) {
			LOG("God Mode ENABLED: Fly and Invincibility");
			// Turns off gravity (gravity scale 0.0)
			b2Body_SetGravityScale(pbody->body, 0.0f);
			isJumping = false;
			IsDead = false;
			anims.SetCurrent("idle");
		}
		else {
			LOG("God Mode DISABLED");
			// Restores normal gravity (gravity scale 1.0)
			b2Body_SetGravityScale(pbody->body, 1.0f);
		}
	}
	if (shootCooldown > 0) shootCooldown -= dt;

	// Shoot with F key
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && shootCooldown <= 0) {

		if (ammo > 0) {

			// Create projectile
			auto projectile = std::dynamic_pointer_cast<Projectile>(Engine::GetInstance().entityManager->CreateEntity(EntityType::PROJECTILE));

			// Set initial position (player center)
			projectile->SetPosition(position);

			// Determine direction based on where the player is facing
			if (flipState == SDL_FLIP_NONE) {
				projectile->SetVelocity(Vector2D(1, 0)); // Right
			}
			else {
				projectile->SetVelocity(Vector2D(-1, 0)); // Left
			}

			// Initialize the projectile
			projectile->Awake();
			projectile->Start();
			ammo--;
			LOG("Disparo realizado! Balas restantes: %d", ammo);
			shootCooldown = 500.0f;
		}
		else {
			LOG("¡Sin munición! No puedes disparar.");
		}

	}
	if (IsDead) {
		if (anims.HasFinishedOnce()) {
			b2BodyId body = pbody->body;
			b2Vec2 vel = { 0.0f, 0.0f };
			b2Body_SetLinearVelocity(body, vel);

			b2Vec2 respawnPosMeters = {
				PIXEL_TO_METERS(savePosition.getX() + texW / 2),
				PIXEL_TO_METERS(savePosition.getY() + texH / 2)
			};
			b2Body_SetTransform(body, respawnPosMeters, b2Rot_identity);

			isJumping = false;
			IsDead = false;
			anims.SetCurrent("idle");

			Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Level.wav");
			float limitLeft = Engine::GetInstance().render->camera.w / 4;
			float cameraX = -(savePosition.getX() - limitLeft);

			Engine::GetInstance().render->camera.x = (int)cameraX;
			Engine::GetInstance().render->camera.y = 0;
			std::cout << "YOU ARE DEAD";

		}
		velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
		velocity.x = 0;
		Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
	}
	else {
		GetPhysicsValues();
		Move();

		// Only jump if you are NOT in God Mode
		if (!isGodMode) {
			Jump();
		}

		ApplyPhysics();

		Vector2D mapSize = Engine::GetInstance().map->GetMapSizeInPixels();// get map size to set a limit
		float limitLeft = Engine::GetInstance().render->camera.w / 4;
		float limitRight = mapSize.getX() - Engine::GetInstance().render->camera.w * 3 / 4;
		if (position.getX() - limitLeft > 0 && position.getX() < limitRight) {//specify where to start moving the camera while respecting the limit
			Engine::GetInstance().render->camera.x = -position.getX() + Engine::GetInstance().render->camera.w / 4;//It moves in the opposite direction to solve it, we put a - to reverse it
		}

		//We check if the player is outside the map size, that is, if they fall into a gap where there is no platform; if so, the player dies
		float mapBottom = mapSize.getY();
		// Only dies from falling if NOT in God Mode
		if (!isGodMode && position.getY() > mapBottom + (texH)) {
			LOG("player fell off the map");
			Die();
		}
	}
	Draw();
	return true;

}

void Player::GetPhysicsValues() {
	// Read current velocity
	velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);

	// If in God Mode, reset both speeds for total flight control
	if (isGodMode) {
		velocity = { 0.0f, 0.0f };
	}
	else {
		// Normal behavior: Resets horizontal speed
		velocity = { 0, velocity.y };
	}
}

void Player::Move() {
	// This function can be used for more complex movement logic if needed
	if (IsDead) {
		velocity.x = 0;
		return;
	}

	// Vertical movement for God Mode (flying)
	if (isGodMode) {
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT || Engine::GetInstance().input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) {
			velocity.y = -speed;
		}
		else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || Engine::GetInstance().input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {
			velocity.y = speed;
		}
		else {
			velocity.y = 0.0f;
		}
	}

	// Move left/right
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		velocity.x = -speed;
		anims.SetCurrent("run");
		flipState = SDL_FLIP_HORIZONTAL;
	}
	else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		velocity.x = speed;
		anims.SetCurrent("run");
		flipState = SDL_FLIP_NONE;
	}
	// Logic for going to 'idle'
	else if (!isGodMode || (velocity.x == 0.0f && velocity.y == 0.0f)) {
		velocity.x = 0.0f;
		anims.SetCurrent("idle");
	}

	// In God Mode, if the player moves (horizontally or vertically), they use the running animation.
	if (isGodMode && (velocity.x != 0.0f || velocity.y != 0.0f)) {
		if (anims.GetCurrentName() == "idle" || anims.GetCurrentName() == "jump") {
			anims.SetCurrent("run");
		}
	}
}

void Player::Jump() {
	if (isGodMode) return;

	// This function can be used for more complex jump logic if needed
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false) {
		Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -jumpForce, true);
		anims.SetCurrent("jump");
		isJumping = true;
		Engine::GetInstance().audio->PlayFx(jumpFxId, 0, 5.0f);
	}
}

void Player::ApplyPhysics() {
	// Preserve vertical speed while jumping (only applies if NOT in God Mode; in God Mode it is already calculated in Move())
	if (isJumping == true && !isGodMode) {
		velocity.y = Engine::GetInstance().physics->GetYVelocity(pbody);
	}

	// Apply velocity via helper
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Player::Draw() {
	// Update render position using your PhysBody helper
	int x, y;
	double dt = Engine::GetInstance().GetDt();
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	bool shouldDraw = true; 
	if (invulnerabilityTimer > 0.0f) {
	
		if (((int)invulnerabilityTimer / 100) % 2 == 0) {
			shouldDraw = false;
		}
	}

	if (shouldDraw) {
		const SDL_Rect& animFrame = anims.GetCurrentFrame();
		Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2, &animFrame, 1.0f, 0.0, INT_MAX, INT_MAX, flipState);
	}
	// ==========================================================
	// heart UI
	if (heartTexture != nullptr) {
		for (int i = 0; i < lives; ++i) {
			float heartScale = 0.1f;
			int spacing = 25;
			Engine::GetInstance().render->DrawTexture(
				heartTexture,
				20 + i * spacing,  // X
				20,           // Y 
				nullptr,      // 
				0.0f,          // speed = 0 
				0.0,
				INT_MAX, INT_MAX,
				SDL_FLIP_NONE,
				heartScale
			);
		}
	}
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().textures->UnLoad(heartTexture);
	if (pbody != nullptr) {
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		pbody = nullptr; 
	}
	return true;
}

void Player::Die()
{
	if (IsDead || isGodMode) return;

	LOG("Player Died!");


	// 1. Obtain the physics body
	IsDead = true;
	Engine::GetInstance().audio->PlayFx(deathFxId);
	anims.SetCurrent("death");

	lives = maxLives;
	invulnerabilityTimer = 0.0f;

	b2BodyId body = pbody->body;
	// 2. Reset the speed to zero
	b2Vec2 vel = b2Body_GetLinearVelocity(body);

	// 3. Reset the position to the initial position (converting from pixels to meters)
	vel.x = 0.0f;
	vel.y = 0.0f;
	b2Body_SetLinearVelocity(body, vel);


}

// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	// Invincibility: Ignores all collisions in God Mode
	if (isGodMode) return;

	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
	{
		int x, y;
		pbody->GetPosition(x, y); 
		int distToCheck = texH / 2 + 5;

		float normal_x = 0.0f;
		float normal_y = 0.0f;

	
		int hitDist = pbody->RayCast(x, y, x, y + distToCheck, normal_x, normal_y);

		if (hitDist != -1 && normal_y < -0.5f) {
			isJumping = false;
		}
	}
		//reset the jump flag when touching the ground
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::DEATH:
		LOG("Collision DEATH");
		Die(); // Calls the death function (already has the God Mode check)
		break;
	case ColliderType::SAVEPOINT:
		// When it collides with the savepoint, it updates the save position.
		//We obtain the position of the center of the savepoint for more accurate respawning.
		int spX, spY;
		physB->GetPosition(spX, spY);
		// The savepoint body is the center. The player's position is adjusted in Die/Update.
		savePosition.setX((float)spX - texW / 2); // We adjusted it to be the top left corner (Entity::position)
		savePosition.setY((float)spY - texH / 2); // Using texW/2 as the pixel setting for player size.
		LOG("Collision SAVEPOINT. Position updated to (%.2f, %.2f)", savePosition.getX(), savePosition.getY());
		Engine::GetInstance().audio->PlayFx(Engine::GetInstance().scene->saveFxId, 0, 10.0f);

		if (lives < maxLives) {
			lives++;
			LOG("Life restored!!! Current lives: %d", lives);
			//audios of recovering life

		}


		break;
	case ColliderType::ENEMY:
	{
		LOG("Collision ENEMY");
		Enemy* enemy = (Enemy*)physB->listener; // Get the pointer of the enemy that collided

		if (enemy != nullptr) {
			if (enemy->enemyType == EnemyType::BOSS) {
		
			}
			// If it's a regular mob (Ground/Flying), maintain the original "damage on contact" logic.
			else {
				if (invulnerabilityTimer <= 0.0f) {
					lives--;
					if (lives > 0) {
						invulnerabilityTimer = 2000.0f; 
						LOG("Player HIT by Enemy! Lives: %d", lives);
						Engine::GetInstance().audio->PlayFx(deathFxId);

						// Repel logic
						b2Vec2 velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
						float knockDir = (velocity.x > 0) ? -1.0f : 1.0f;
						Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, knockDir * 1.5f, -2.0f, true);
					}
					else {
						Die();
					}
				}
			}
		}

		break;
	}
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	default:
		break;
	}
}
Vector2D Player::GetPosition() {
	int x, y;
	pbody->GetPosition(x, y);
	// Adjust for center
	return Vector2D((float)x - texW / 2, (float)y - texH / 2);
}

void Player::SetPosition(Vector2D pos) {
	pbody->SetPosition((int)(pos.getX() + texW / 2), (int)(pos.getY() + texH / 2));
}
