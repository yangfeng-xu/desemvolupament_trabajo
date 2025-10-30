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
Player::Player() : Entity(EntityType::PLAYER),IsDead(false)
{
	name = "Player";
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: TODO 2: Initialize Player parameters
	position = Vector2D(100, 600);//donde esta player en la mapa
	startPosition = position;
	return true;
}

bool Player::Start() {

	//L03: TODO 2: Initialize Player parameters
	/*texture = Engine::GetInstance().textures->Load("Assets/Textures/Owlet_Monster.png");*/
	texture = Engine::GetInstance().textures->Load("Assets/Textures/player_sprite.png");

	std::unordered_map<int, std::string>animNames = { {13,"death"},{21,"run"},{39,"idle"},{156,"jump"}};
	anims.LoadFromTSX("Assets/Textures/player_sprite2.tsx", animNames);
	anims.SetCurrent("idle");
	// L08 TODO 5: Add physics to the player - initialize physics body
	texW = 32;
	texH = 32;
	//Engine::GetInstance().textures->GetSize(texture, texW, texH);
	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect
	pickCoinFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/coin-collision-sound-342335.wav");

	return true;
}

bool Player::Update(float dt)
{
	anims.Update(dt);
	if (IsDead) {
		if (anims.HasFinishedOnce()) {
			b2BodyId body = pbody->body;
			b2Vec2 vel = { 0.0f, 0.0f };
			b2Body_SetLinearVelocity(body, vel);
			b2Vec2 startPosMeters = { PIXEL_TO_METERS(startPosition.getX()), PIXEL_TO_METERS(startPosition.getY()) };
			b2Body_SetTransform(body, startPosMeters, b2Rot_identity);
			isJumping = false;
			IsDead = false;
			anims.SetCurrent("idle");
		//cuando player se muere y vulverá al principio del juego, reinicializar la camara
			Engine::GetInstance().render->camera.x = 0;
			Engine::GetInstance().render->camera.y = 0;
			std::cout << "YOU ARE DEAD";

			for (const auto& entity : Engine::GetInstance().entityManager->entities) {
				if (entity->type == EntityType::ITEM) {
					entity->Enable();
				}
			}
		}
		velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
		velocity.x = 0;
		Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
	}
	else {
		GetPhysicsValues();
		Move();
		Jump();
		ApplyPhysics();
		
		Vector2D mapSize = Engine::GetInstance().map->GetMapSizeInPixels();//coger tamaño de la mapa para ponerla un limite
		float limitLeft = Engine::GetInstance().render->camera.w / 4;
		float limitRight = mapSize.getX() - Engine::GetInstance().render->camera.w * 3 / 4;
		if (position.getX() - limitLeft > 0 && position.getX() < limitRight) {//decir a donde empieza a mover la camara respectando el limite
			Engine::GetInstance().render->camera.x = -position.getX() + Engine::GetInstance().render->camera.w / 4;//se mueve contrario para solucionar lo, ponemos un - para invertirlo
		}

		//comprovamos si el player esta fuera de la tamaño de la mapa, es dacir se cae al hueco donde no es plataforma, si lo es player se muere
		float mapBottom = mapSize.getY();
		if (position.getY() > mapBottom + (texH)) {//lo sumamos texH para que asegurar de que el player esta absolutamente a fuera
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
	velocity = { 0, velocity.y }; // Reset horizontal velocity by default, this way the player stops when no key is pressed
}

void Player::Move() {
	// This function can be used for more complex movement logic if needed
	if (IsDead) {
		velocity.x = 0;
		return;
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
	else {
		velocity.x = 0.0f;
		anims.SetCurrent("idle");
	}
	
	

}

void Player::Jump() {
	// This function can be used for more complex jump logic if needed
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false) {
		Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -jumpForce, true);
		anims.SetCurrent("jump");
		isJumping = true;
	}
}

void Player::ApplyPhysics() {
	// Preserve vertical speed while jumping
	if (isJumping == true) {
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
	
	const SDL_Rect& animFrame = anims.GetCurrentFrame();
	Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2,&animFrame, 1.0f, 0.0, INT_MAX, INT_MAX, flipState);
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures->UnLoad(texture);
	return true;
}
void Player::Die()
{
	if (IsDead)return;
	LOG("Player Died!");

	// 1. Obtén el cuerpo de físicas
	IsDead = true;
	anims.SetCurrent("death");

	b2BodyId body = pbody->body;
	// 2. Resetea la velocidad a cero
	b2Vec2 vel =b2Body_GetLinearVelocity(body);

	// 3. Resetea la posición a la de inicio (convirtiendo de píxeles a metros)
	vel.x = 0.0f;
	b2Body_SetLinearVelocity(body, vel);
	
	
}
// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		//reset the jump flag when touching the ground
		isJumping = false;
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		Engine::GetInstance().audio->PlayFx(pickCoinFxId);
		physB->listener->Destroy();
		break;
	case ColliderType::DEATH: // <-- AÑADE ESTE CASO
		LOG("Collision DEATH");
		Die(); // Llama a la función de muerte
		break;
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

