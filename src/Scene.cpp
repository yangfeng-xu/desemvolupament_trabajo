#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"
#include "Enemy.h"

Scene::Scene() : Module()
{
	name = "scene";
}

// Destructor
Scene::~Scene()
{
}

// Called before render is available
//en awake si no creamos una capa llamando entities en el maptxt, decidimos el posición en aqui
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	//L04: TODO 3b: Instantiate the player using the entity manager

	//L08: TODO 4: Create a new item using the entity manager and set the position to (200, 672) to test
	std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
	item->startPosition = Vector2D(200, 672);

	return ret;
}

// Called before the first frame
bool Scene::Start()
{

	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Level.wav");

	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");

	//L15 TODO 3: Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player);
	// Texture to highligh mouse position 
	mouseTileTex = Engine::GetInstance().textures->Load("Assets/Maps/MapMetadata.png");
	helpMenuTexture = Engine::GetInstance().textures->Load("Assets/Maps/menu.png");

	saveFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/autosave.wav");

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
	{
		showHelpMenu = !showHelpMenu;
		LOG("Help Menu: %s", showHelpMenu ? "SHOWING" : "HIDING");
	}
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//L03 TODO 3: Make the camera movement independent of framerate
	float camSpeed = 1;

	if (reloadCooldown > 0.0f) {
		reloadCooldown -= dt;
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		Engine::GetInstance().render->camera.y -= (int)ceil(camSpeed * dt);

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		Engine::GetInstance().render->camera.y += (int)ceil(camSpeed * dt);

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		Engine::GetInstance().render->camera.x -= (int)ceil(camSpeed * dt);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.x += (int)ceil(camSpeed * dt);

	//Get mouse position and obtain the map coordinate
	Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();
	Vector2D mouseTile = Engine::GetInstance().map->WorldToMap((int)(mousePos.getX() - Engine::GetInstance().render->camera.x),
		(int)(mousePos.getY() - Engine::GetInstance().render->camera.y));

	//Render a texture where the mouse is over to highlight the tile, use the texture 'mouseTileTex'
	Vector2D highlightTile = Engine::GetInstance().map.get()->MapToWorld((int)mouseTile.getX(), (int)mouseTile.getY());
	SDL_Rect rect = { 0,0,Engine::GetInstance().map->GetTileWidth(),Engine::GetInstance().map->GetTileHeight() };
	Engine::GetInstance().render.get()->DrawTexture(mouseTileTex, (int)highlightTile.getX(), (int)highlightTile.getY(), &rect);

	// saves the tile pos for debugging purposes
	if (mouseTile.getX() >= 0 && mouseTile.getY() >= 0 || once) {
		tilePosDebug = "[" + std::to_string((int)mouseTile.getX()) + "," + std::to_string((int)mouseTile.getY()) + "] ";
		once = true;
	}

	//If mouse button is pressed modify player position
	if (Engine::GetInstance().input.get()->GetMouseButtonDown(1) == KEY_DOWN) {
		player->SetPosition(Vector2D(highlightTile.getX(), highlightTile.getY()));
	}
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;
	//L15 TODO 3: Call the function to load entities from the map
	//Engine::GetInstance().map->LoadEntities(player);
	// Texture to highligh mouse position 
	//mouseTileTex = Engine::GetInstance().textures->Load("Assets/Maps/MapMetadata.png");
	if (showHelpMenu)
	{
		int windowWidth, windowHeight;
		Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);

		// CALCULAR POSICIÓN Y TAMAÑO CENTRADO (2/3 de la pantalla)
		int menuWidth = windowWidth * 2 / 3;
		int menuHeight = windowHeight * 2 / 3;

		int menuX = (windowWidth - menuWidth) / 2;
		int menuY = (windowHeight - menuHeight) / 2;

		//Dibuja un fondo oscuro para asegurar que el área esté definida.
		SDL_Rect backgroundRect = { menuX, menuY, menuWidth, menuHeight };
		Engine::GetInstance().render->DrawRectangle(backgroundRect, 0, 0, 0, 200, true, false);

		// 2. DIBUJA LA IMAGEN DEL MENÚ ("menu.png")
		if (helpMenuTexture != nullptr)
		{
			// Usamos las coordenadas de inicio del menú (menuX, menuY)
			// speed = 0.0f asegura que la imagen no se mueva con el offset de la cámara.
			Engine::GetInstance().render->DrawTexture(
				helpMenuTexture,
				menuX,
				menuY,
				nullptr, // section: Dibuja la textura completa
				0.0f,    // speed: 0.0f (sin movimiento de cámara)
				0.0,     // angle
				INT_MAX, // pivotX 
				INT_MAX  // pivotY 
			);
		}

		//Dibuja un borde blanco.
		Engine::GetInstance().render->DrawRectangle(backgroundRect, 255, 255, 255, 255, false, false);
	}
	

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {

		
		

		// 2. Cargamos las entidades (esto creará nuevas y moverá al Player)
		Engine::GetInstance().map->LoadEntities(player);

		reloadCooldown = 500.0f;
	}
	//L15 TODO 5: Call the function to save entities from the map
	//pulso f6 para guardar donde quiero que el player se vuelve,luego pulsamos f5 para valve ese punto
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		Engine::GetInstance().map->SaveEntities(player);
		Engine::GetInstance().audio->PlayFx(saveFxId);
	}

	//f11 change fps
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {
		int currentFps = Engine::GetInstance().GetTargetFrameRate();
		if (currentFps == 60) {
			Engine::GetInstance().SetTargetFrameRate(30);
			LOG("FPS limid changed to 30");
		}
		else {
			Engine::GetInstance().SetTargetFrameRate(60);
			LOG("FPS limid changed to 60");

		}
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();
}