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
#include "UIToggle.h"
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
	/*LoadScene(currentScene);*/
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//SDL_Texture* pauseTex = Engine::GetInstance().textures->Load("Assets/Textures/Pause.png");
	//SDL_Texture* resumeTex = Engine::GetInstance().textures->Load("Assets/Textures/Resume.png");
	LoadScene(currentScene);

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
	//if (currentScene == SceneID::MAIN_MENU && mainMenuBackground != nullptr) {
	//	
	//	Engine::GetInstance().render->DrawTexture(mainMenuBackground, 0, 0, nullptr, 0.0f);
	//}
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
		isGameOver = false;
		exitGameRequested = false;
	}

	if (exitGameRequested) {
		return false; // ???? false ??? Engine ???????????????
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
	}


	//----------------------------------------------------------------------------Currrent Scene-----------------------------------------------------------------------------------//


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
		PostUpdateLevel2();
		break;
	default:
		break;
	}
	//----------------------------------------------------------------------------F5-----------------------------------------------------------------------------------//
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
	if (player == nullptr) {
		return Vector2D(0, 0);
	}
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
	// ?? ID ???????? ?? Toggle ID (?? 100)
	if (uiElement->id == 100)
	{
		// ???????????
		Engine::GetInstance().window->ToggleFullsreen();
		LOG("Toggled Fullscreen Mode");
	}


	// --------------------------------------------------------------------------Pause UI-----------------------------------------------------------------------------------//
	if (uiElement->id == PAUSE_TOGGLE_ID)
	{
		// 这里的逻辑是：如果按钮是 On (显示播放图)，说明我们刚才按下了暂停，进入暂停状态
		// 我们强制转换一下来获取状态
		auto toggle = std::dynamic_pointer_cast<UIToggle>(Engine::GetInstance().uiManager->GetElement(PAUSE_TOGGLE_ID));
		if (toggle) {
			isGamePaused = toggle->IsOn();
		}

		LOG("Game Paused: %s", isGamePaused ? "YES" : "NO");

		// Pause music
		if (isGamePaused)
			Engine::GetInstance().audio->PauseMusic(); // 假设你有这个函数
		else
			Engine::GetInstance().audio->ResumeMusic();
	}
	
	//----------------------------------------------------------------------------Level 1-----------------------------------------------------------------------------------//
	if (currentScene == SceneID::LEVEL_1 && isGameOver) {

		if (uiElement->id == RESUME_BTN_ID) {
		
			LOG("Restarting Level 1...");	
			ChangeScene(SceneID::LEVEL_1);		
		}
		else if (uiElement->id == EXIT_BTN_ID) {
			// ?????????
			LOG("Exiting Game...");
			exitGameRequested = true;
		
		}
	}
	else if (currentScene == SceneID::LEVEL_2 && isGameOver) {
		if (uiElement->id == RESUME_BTN_ID) {
			LOG("Restarting Level 2...");
			ChangeScene(SceneID::LEVEL_2); // <--- 确保这里重启的是 Level 2
		}
		else if (uiElement->id == EXIT_BTN_ID) {
			LOG("Exiting Game...");
			exitGameRequested = true;
		}
	}


	//----------------------------------------------------------------------------Current Scene-----------------------------------------------------------------------------------//
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
	Engine::GetInstance().render->camera.x = 0;
	Engine::GetInstance().render->camera.y = 0;
	mainMenuBackground = Engine::GetInstance().textures->Load("Assets/Textures/menu_resized.png");
	Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, "Start", { 520,350,120,20 }, this);

}
void Scene::UnloadMainMenu() {
	Engine::GetInstance().uiManager->CleanUp();
	if (mainMenuBackground != nullptr) {
		Engine::GetInstance().textures->UnLoad(mainMenuBackground);
		mainMenuBackground = nullptr;
	}
	
}
void Scene::UpdateMainMenu(float dt) {
	if (mainMenuBackground != nullptr) {
		// speed = 0.0f 确保背景固定
		Engine::GetInstance().render->DrawTexture(mainMenuBackground, 0, 0, nullptr, 0.0f);
	}
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

	Engine::GetInstance().render->camera.x = 0;
	Engine::GetInstance().render->camera.y = 0;
	isGameOver = false;
	gameOverShown = false;
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");

	iconPause = Engine::GetInstance().textures->Load("Assets/Textures/Pause.png");
	iconPlay = Engine::GetInstance().textures->Load("Assets/Textures/Resume.png");

	// 2. 创建 Toggle
	// 位置 { 10, 50, 32, 32 } 根据你的图标大小调整
	auto pauseToggle = Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::TOGGLE,
		PAUSE_TOGGLE_ID,
		"",
		{ 10, 50, 32, 32 },
		this
	);

	// 3. 将其转换为 UIToggle 指针并设置图片
	// 注意：dynamic_pointer_cast 用于将基类 UIElement 转为子类 UIToggle
	auto togglePtr = std::dynamic_pointer_cast<UIToggle>(pauseToggle);
	if (togglePtr) {
		// 参数1(Off): 游戏没暂停时显示的图 -> 暂停键
		// 参数2(On): 游戏暂停时显示的图 -> 播放键
		togglePtr->SetTextures(iconPause, iconPlay);
	}

	Engine::GetInstance().map->LoadEntities(player);
	// Texture to highligh mouse position 
	mouseTileTex = Engine::GetInstance().textures->Load("Assets/Maps/MapMetadata.png");
	helpMenuTexture = Engine::GetInstance().textures->Load("Assets/Maps/menu.png");

	saveFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/autosave.wav");


	//// L16: TODO 2: Instantiate a new GuiControlButton in the Scene
	uiBt1 = std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, std::string("Start").c_str(), { 0,0,100,20 }, this));
	uiBt2 = std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, std::string("Setting").c_str(), { 0,50,100,20 }, this));
	//full sreen
	Engine::GetInstance().uiManager->CreateUIElement(UIElementType::TOGGLE, 100, "FullScreen", { 10, 10, 100, 30 }, this);


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
	//timer
	levelTimer = 60.0f * 1000.0f;
	
}
void Scene::UnloadLevel1() {//limpia la mapa y entity
	Engine::GetInstance().uiManager->CleanUp();
	player.reset();//eliminar player para verificar que todo esta eliminado//opcional
	Engine::GetInstance().entityManager->CleanUp();
	Engine::GetInstance().map->CleanUp();

	isGameOver = false;
	gameOverShown = false;
	levelTimer = 60.0f * 1000.0f; // ????
	
}
void Scene::UpdateLevel1(float dt) {//para poder cambiar la escena a nivell 2

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL_2);
	}

	// 1. 始终允许切换场景的输入（如按键 '2'）
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL_2);
	}

	// 2. 如果游戏暂停了，直接 return，不执行下面的逻辑
	if (isGamePaused) {
		return;
	}

	if (player != nullptr)
	{
		Vector2D pos = player->GetPosition();

		// 【修改这里】：设置你的终点范围
		// 假设终点平台在 X=3050, Y=200 左右
		float targetX_Min = 1950.0f; // 区域左边界
		float targetX_Max = 2000.0f; // 区域右边界
		float targetY_Max = 160.0f;  // 区域下边界 (Y越小越高，所以只要小于这个值就是在上面)

		// 同时满足：X在范围内 且 Y足够高
		if (pos.getX() > targetX_Min && pos.getX() < targetX_Max && pos.getY() < targetY_Max)
		{
			LOG("Reached the top platform! Loading Level 2...");
			ChangeScene(SceneID::LEVEL_2);
		}
	}

	if (!isGameOver) {
		levelTimer -= dt;
		if (levelTimer <= 0.0f) {
			levelTimer = 0.0f;
			isGameOver = true;
			LOG("Game Over!!! Time's Up");
		}
	}

	// ??? isGameOver ?????????????
	if (isGameOver && !gameOverShown) {
		int w, h;
		Engine::GetInstance().window->GetWindowSize(w, h);
		int centerX = w / 2;
		int centerY = h / 2;

		Engine::GetInstance().uiManager->CreateUIElement(
			UIElementType::BUTTON,
			RESUME_BTN_ID,
			"RESTART",
			{ centerX - 70, centerY - 50, 140, 40 },
			this
		);

		Engine::GetInstance().uiManager->CreateUIElement(
			UIElementType::BUTTON,
			EXIT_BTN_ID,
			"EXIT",
			{ centerX - 70, centerY + 10, 140, 40 },
			this
		);
		gameOverShown = true;
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

	// ???????
	int secondsLeft = (int)(levelTimer / 1000.0f);

	// ?????
	std::string timeText = "Time: " + std::to_string(secondsLeft);

	// ?????????? (?? x=10, y=10)
	// ????? {255, 255, 255, 255}
	// ???? DrawText ????
	Engine::GetInstance().render->DrawText(timeText.c_str(), 50, 50, 100, 30, { 255, 255, 255, 255 });

	// ????????????????? "GAME OVER" ???
	if (isGameOver) {
		Engine::GetInstance().render->DrawText("GAME OVER", 550, 200, 200, 50, { 255, 0, 0, 255 });
	}
}
// *********************************************
// Level 2 functions
// *********************************************

// L17 TODO 5: Define specific functions for level2 scene: Load, Unload, Update
void Scene::LoadLevel2() {

	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/that-8-bit-music-322062.wav");
	Engine::GetInstance().render->camera.x = 0;
	Engine::GetInstance().render->camera.y = 0;

	isGameOver = false;
	gameOverShown = false;

	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate2.tmx");
	//L15 TODO 3: Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player);
	Engine::GetInstance().uiManager->CreateUIElement(UIElementType::TOGGLE, 100, "FullScreen", { 10, 10, 100, 30 }, this);
	levelTimer = 60.0f * 1000.0f;

	
	
}
void Scene::UnloadLevel2() {
	Engine::GetInstance().uiManager->CleanUp();
	player.reset();//eliminar player para verificar que todo esta eliminado//opcional
	Engine::GetInstance().entityManager->CleanUp();
	Engine::GetInstance().map->CleanUp();
	isGameOver = false;
	gameOverShown = false;
	levelTimer = 60.0f * 1000.0f; // ????
}
void Scene::UpdateLevel2(float dt) {//cambiar a nivell 1
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL_1);
	}
	if (!isGameOver) {
		levelTimer -= dt;
		if (levelTimer <= 0.0f) {
			levelTimer = 0.0f;
			isGameOver = true;
			LOG("Game Over!!! Time's Up");
		}
	}

	// ??? isGameOver ?????????????
	if (isGameOver && !gameOverShown) {
		int w, h;
		Engine::GetInstance().window->GetWindowSize(w, h);
		int centerX = w / 2;
		int centerY = h / 2;

		Engine::GetInstance().uiManager->CreateUIElement(
			UIElementType::BUTTON,
			RESUME_BTN_ID,
			"RESTART",
			{ centerX - 70, centerY - 50, 140, 40 },
			this
		);

		Engine::GetInstance().uiManager->CreateUIElement(
			UIElementType::BUTTON,
			EXIT_BTN_ID,
			"EXIT",
			{ centerX - 70, centerY + 10, 140, 40 },
			this
		);
		gameOverShown = true;
	}
}
void Scene::PostUpdateLevel2() {
	//L15 TODO 3: Call the function to load entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		Engine::GetInstance().map->LoadEntities(player);
	}

	//L15 TODO 4: Call the function to save entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		Engine::GetInstance().map->SaveEntities(player);
	}

	// ???????
	int secondsLeft = (int)(levelTimer / 1000.0f);

	// ?????
	std::string timeText = "Time: " + std::to_string(secondsLeft);

	// ?????????? (?? x=10, y=10)
	// ????? {255, 255, 255, 255}
	// ???? DrawText ????
	Engine::GetInstance().render->DrawText(timeText.c_str(), 50, 50, 100, 30, { 255, 255, 255, 255 });

	// ????????????????? "GAME OVER" ???
	if (isGameOver) {
		Engine::GetInstance().render->DrawText("GAME OVER", 550, 200, 200, 50, { 255, 0, 0, 255 });
	}
}

