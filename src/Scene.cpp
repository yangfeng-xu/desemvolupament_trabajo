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
#include "UIManager.h"
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

	////L08: TODO 4: Create a new item using the entity manager and set the position to (200, 672) to test
	//std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
	//item->startPosition = Vector2D(200, 672);

	//// L16: TODO 2: Instantiate a new GuiControlButton in the Scene
	//uiBt1 = std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, std::string("Start").c_str(), { 0,0,100,20 }, this));
	//uiBt2 = std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, std::string("Setting").c_str(), { 0,50,100,20 }, this));
	
	LoadScene(currentScene);
	return ret;
}

// Called before the first frame
bool Scene::Start()
{

	/*Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Level.wav");*/

	//L06 TODO 3: Call the function to load the map. 
	/*Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");*/

	////L15 TODO 3: Call the function to load entities from the map
	//Engine::GetInstance().map->LoadEntities(player);
	//// Texture to highligh mouse position 
	//mouseTileTex = Engine::GetInstance().textures->Load("Assets/Maps/MapMetadata.png");
	//helpMenuTexture = Engine::GetInstance().textures->Load("Assets/Maps/menu.png");

	//saveFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/autosave.wav");

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
	/*float camSpeed = 1;*/

	if (reloadCooldown > 0.0f) {
		reloadCooldown -= dt;
	}
	/*if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		Engine::GetInstance().render->camera.y -= (int)ceil(camSpeed * dt);

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		Engine::GetInstance().render->camera.y += (int)ceil(camSpeed * dt);

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		Engine::GetInstance().render->camera.x -= (int)ceil(camSpeed * dt);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.x += (int)ceil(camSpeed * dt);*/

	////Get mouse position and obtain the map coordinate
	//Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();
	//Vector2D mouseTile = Engine::GetInstance().map->WorldToMap((int)(mousePos.getX() - Engine::GetInstance().render->camera.x),
	//	(int)(mousePos.getY() - Engine::GetInstance().render->camera.y));

	////Render a texture where the mouse is over to highlight the tile, use the texture 'mouseTileTex'
	//Vector2D highlightTile = Engine::GetInstance().map.get()->MapToWorld((int)mouseTile.getX(), (int)mouseTile.getY());
	//SDL_Rect rect = { 0,0,Engine::GetInstance().map->GetTileWidth(),Engine::GetInstance().map->GetTileHeight() };
	//Engine::GetInstance().render.get()->DrawTexture(mouseTileTex, (int)highlightTile.getX(), (int)highlightTile.getY(), &rect);

	//// saves the tile pos for debugging purposes
	//if (mouseTile.getX() >= 0 && mouseTile.getY() >= 0 || once) {
	//	tilePosDebug = "[" + std::to_string((int)mouseTile.getX()) + "," + std::to_string((int)mouseTile.getY()) + "] ";
	//	once = true;
	//}

	////If mouse button is pressed modify player position
	//if (Engine::GetInstance().input.get()->GetMouseButtonDown(1) == KEY_DOWN) {
	//	player->SetPosition(Vector2D(highlightTile.getX(), highlightTile.getY()));
	//}

	switch (currentScene) {
	case SceneID::INTRO_SCR:
		//loadIntroScren();
		break;

	case SceneID::MAIN_MENU:
		UpdateMainMenu(dt);
		break;
	case SceneID::LEVEL_1:
		UpdateLevel1(dt);
		break;
	case SceneID::LEVEL_2:
		UpdateLevel2(dt);
		break;
	default:
		break;
	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{

	if (sceneChangeRequested)
	{
		UnloadCurrentScene();
		currentScene = nextScene;
		LoadScene(currentScene);
		sceneChangeRequested = false;
	}
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

		//Dibuja un fondo oscuro para asegurar que el área est?definida.
		SDL_Rect backgroundRect = { menuX, menuY, menuWidth, menuHeight };
		Engine::GetInstance().render->DrawRectangle(backgroundRect, 0, 0, 0, 200, true, false);

		// 2. DIBUJA LA IMAGEN DEL MEN?("menu.png")
		if (helpMenuTexture != nullptr)
		{
			// Usamos las coordenadas de inicio del men?(menuX, menuY)
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

			if (currentScene == SceneID::LEVEL_1) PostUpdateLevel1();
		}

		//Dibuja un borde blanco.
		Engine::GetInstance().render->DrawRectangle(backgroundRect, 255, 255, 255, 255, false, false);

		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)//para salir el juego
			ret = false;
		switch (currentScene) {

		case SceneID::INTRO_SCR:
			//loadIntroScren();
			break;

		case SceneID::MAIN_MENU:
			break;
		case SceneID::LEVEL_1:
			PostUpdateLevel1();
			break;
		case SceneID::LEVEL_2:
			break;
		default:
			break;
		}




	}
	

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {

		
		

		// 2. Cargamos las entidades (esto crear?nuevas y mover?al Player)
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
	UnloadCurrentScene();
	return true;
}
Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();
}

bool Scene::OnUIMouseClickEvent(UIElement* uiElement)
{
	// L16: TODO 5: Implement the OnGuiMouseClickEvent method
	/*LOG("Click:%d",uiElement->id);*/
	if (uiElement->id == 1) {
		LOG("Start game");
	}
	if (uiElement->id == 2) {
		LOG("Go to settings");
	}

	switch (currentScene) {
	case SceneID::INTRO_SCR:
		//loadIntroScren();
		break;

	case SceneID::MAIN_MENU:
		HandleMainMenuUIVebets(uiElement);
		break;
	case SceneID::LEVEL_1:
		break;
	case SceneID::LEVEL_2:
		break;
	default:
		break;

	}
	return true;
}

// *********************************************
// Scene change functions
// *********************************************

// L17 TODO 2: Define functions to handle scene changes
void Scene::LoadScene(SceneID newScene) {//despues de implementar hay que llamar en aqui
	switch (newScene) {
	case SceneID::INTRO_SCR:
		//loadIntroScren();
		break;

	case SceneID::MAIN_MENU:
		LoadMainMenu();
		break;
	case SceneID::LEVEL_1:
		LoadLevel1();
		break;
	case SceneID::LEVEL_2:
		LoadLevel2();
		break;
	default:
		break;
	}
}
void Scene::UnloadCurrentScene() {
	switch (currentScene) {
	case SceneID::INTRO_SCR:
		//loadIntroScren();
		break;

	case SceneID::MAIN_MENU:
		UnloadMainMenu();
		break;
	case SceneID::LEVEL_1:
		UnloadLevel1();
		break;
	case SceneID::LEVEL_2:
		UnloadLevel2();
		break;
	default:
		break;
	}
}
void Scene::ChangeScene(SceneID newScene) {
	/*UnloadCurrentScene();*/
	sceneChangeRequested = true;
	nextScene = newScene;
	/*currentScene = newScene;*/
	/*LoadScene(currentScene);*/
}

// *********************************************
// MAIN MENU functions
// *********************************************

// L17 TODO 3: Define specific function for main menu scene: Load, Unload, Handle UI events
void Scene::LoadMainMenu() {//cargar audio en aqui
	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/retro-gaming-short-248416.wav");
	Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, "Start", { 520,350,120,20 }, this);
}
void Scene::UnloadMainMenu() {
	Engine::GetInstance().uiManager->CleanUp();
}
void Scene::UpdateMainMenu(float dt) {

}
void Scene::HandleMainMenuUIVebets(UIElement* uiLement) {
	switch (uiLement->id)
	{
	case 1:
		ChangeScene(SceneID::LEVEL_1);
		break;
	default:
		LOG("UIElement not handled");
		break;
	}
}
// *********************************************
// Level 1 functions
// *********************************************

// L17 TODO 4: Define specific functions for level1 scene: Load, Unload, Update, PostUpdate
void Scene::LoadLevel1() {//cargar mapa ,textura,audio
	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Level.wav");

	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");

	Engine::GetInstance().map->LoadEntities(player);
	// Texture to highligh mouse position 
	mouseTileTex = Engine::GetInstance().textures->Load("Assets/Maps/MapMetadata.png");
	helpMenuTexture = Engine::GetInstance().textures->Load("Assets/Maps/menu.png");

	saveFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/autosave.wav");


	//// L16: TODO 2: Instantiate a new GuiControlButton in the Scene
	uiBt1 = std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, std::string("Start").c_str(), { 0,0,100,20 }, this));
	uiBt2 = std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, std::string("Setting").c_str(), { 0,50,100,20 }, this));



	//L08: TODO 4: Create a new item using the entity manager and set the position to (200, 672) to test
	std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
	item->position = Vector2D(200, 672);
	item->Start();//!!!
	//Create a new enemy 
	std::shared_ptr<Enemy> enemy1 = std::dynamic_pointer_cast<Enemy>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
	enemy1->position = Vector2D(384, 672);
	enemy1->Start();//importante!!!, tenemos que credar nosotros 
	//este escena no vaterner el botton por lo tanto podemos quitar
	//// L16: TODO 2: Instantiate a new GuiControlButton in the Scene
	//SDL_Rect btPos = { 520, 350, 120,20 };
	//uiBt = std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, "MyButton", btPos, this));
}
void Scene::UnloadLevel1() {//limpia la mapa y entity
	Engine::GetInstance().uiManager->CleanUp();
	player.reset();//eliminar player para verificar que todo esta eliminado//opcional
	Engine::GetInstance().map->CleanUp();
	Engine::GetInstance().entityManager->CleanUp();
}
void Scene::UpdateLevel1(float dt) {//para poder cambiar la escena a nivell 2
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL_2);
	}
}
void Scene::PostUpdateLevel1() {//code especifico de level 1
	//L15 TODO 3: Call the function to load entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		Engine::GetInstance().map->LoadEntities(player);
	}

	//L15 TODO 4: Call the function to save entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		Engine::GetInstance().map->SaveEntities(player);
	}
}
// *********************************************
// Level 2 functions
// *********************************************

// L17 TODO 5: Define specific functions for level2 scene: Load, Unload, Update
void Scene::LoadLevel2() {

	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/that-8-bit-music-322062.wav");

	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplateLevel2.tmx");

	//L15 TODO 3: Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player);


}
void Scene::UnloadLevel2() {
	Engine::GetInstance().uiManager->CleanUp();
	player.reset();//eliminar player para verificar que todo esta eliminado//opcional
	Engine::GetInstance().map->CleanUp();
	Engine::GetInstance().entityManager->CleanUp();
}
void Scene::UpdateLevel2(float dt) {//cambiar a nivell 1
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL_1);
	}
}

