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
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	//L04: TODO 3b: Instantiate the player using the entity manager
	/*player = std::dynamic_pointer_cast<Player>(Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER));*/
	
	//L08: TODO 4: Create a new item using the entity manager and set the position to (200, 672) to test
	std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
	item->startPosition = Vector2D(200, 672);


    std::shared_ptr<Enemy> enemy1 = std::dynamic_pointer_cast<Enemy>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));

	if (enemy1 != nullptr) {
		enemy1->SetEnemyType(EnemyType::FLYING); // <--- IMPORTANTE: Definir tipo
		enemy1->position = Vector2D(220, 672);   // Se usará esta posición cuando se cree el cuerpo físico en Enemy::Start()
	}

	// --- Enemigo 2: TERRESTRE ---
	std::shared_ptr<Enemy> enemy2 = std::dynamic_pointer_cast<Enemy>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));

	if (enemy2 != nullptr) {
		enemy2->SetEnemyType(EnemyType::GROUND); // Opcional si GROUND es el default
		enemy2->position = Vector2D(400, 672);
	}
	
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	
	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Level.wav");
	
	//L06 TODO 3: Call the function to load the map. 
	/*Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");*/

	//L15 TODO 3: Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player);
	// Texture to highligh mouse position 
	mouseTileTex = Engine::GetInstance().textures->Load("Assets/Maps/MapMetadata.png");
	
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

	if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		Engine::GetInstance().render->camera.y -= (int)ceil(camSpeed * dt);

	if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		Engine::GetInstance().render->camera.y += (int)ceil(camSpeed * dt);

	if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		Engine::GetInstance().render->camera.x -= (int)ceil(camSpeed * dt);
	
	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
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

        // 1. Define el área para el menú de ayuda (centrado y dos tercios de la pantalla)
        int menuWidth = windowWidth * 2 / 3;
        int menuHeight = windowHeight * 2 / 3;

        SDL_Rect helpRect = {
            (windowWidth - menuWidth) / 2,
            (windowHeight - menuHeight) / 2,
            menuWidth,
            menuHeight
        };

        // 2. Dibuja el fondo del menú (rectángulo semi-transparente: R, G, B, Alpha)
        Engine::GetInstance().render->DrawRectangle(helpRect, 0, 0, 0, 200, true, false);

        // 3. Dibuja el borde blanco
        Engine::GetInstance().render->DrawRectangle(helpRect, 255, 255, 255, 255, false, false);

        // 4. Dibuja un rectángulo para simular el TÍTULO (Blanco)
        int paddingX = 40;
        int startX = helpRect.x + paddingX;
        int startY = helpRect.y + 40;
        int textWidth = menuWidth - (paddingX * 2);

        SDL_Rect titleRect = { startX, startY - 20, textWidth, 15 };
        // Usamos blanco para el título (R:255, G:255, B:255)
        Engine::GetInstance().render->DrawRectangle(titleRect, 255, 255, 255, 255, true, false);

        // NOTA IMPORTANTE:
        // ELIMINAMOS TODOS LOS DEMÁS RECTÁNGULOS DE LÍNEA.
        // Si su motor tiene una función de dibujo de texto (DrawText),
        // el código para dibujar las teclas debería ir aquí, justo encima del fondo.

        // Mantenemos los LOGs para que la ayuda aparezca en la consola, que es
        // el único lugar donde podemos confirmar la salida de texto:
        LOG("--- H KEY PRESSED - TEXT OUTPUT VIA LOG ONLY ---");
        LOG("H: WSAD / Arrows: Walk / Fly (God Mode)");
        LOG("H: Spacebar: Jump");
        LOG("H: Esc: Exit the game");
        LOG("H: F1: Show / Hide Collisions (Debug)");
        LOG("H: F10: God Mode (Fly/Invincible)");
        LOG("H: H: Show / Hide this Help Menu (TITLE LINE IS WHITE)");
        LOG("-----------------------------------------------");
    }

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		Engine::GetInstance().map->LoadEntities(player);
	}
	//L15 TODO 5: Call the function to save entities from the map
	//pulso f6 para guardar donde quiero que el player se vuelve,luego pulsamos f5 para valve ese punto
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		Engine::GetInstance().map->SaveEntities(player);

	}
	if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
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