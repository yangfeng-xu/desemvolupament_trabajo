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

	collectedIDs.clear();
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	/*collectedIDs.clear();*/
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
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//L03 TODO 3: Make the camera movement independent of framerate

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
	// 绘制 Settings 背景 (如果在显示状态)
if (showSettingsUI) {
	int w, h;
	Engine::GetInstance().window->GetWindowSize(w, h);
	// 简单的半透明黑色背景遮罩
	SDL_Rect bg = { w / 2 - 150, h / 2 - 200, 300, 400 };
	Engine::GetInstance().render->DrawRectangle(bg, 50, 50, 50, 240, true, false); // 深灰色背景
	Engine::GetInstance().render->DrawRectangle(bg, 255, 255, 255, 255, false, false); // 白色边框

	// 绘制标题
	Engine::GetInstance().render->DrawText("SETTINGS", w / 2 - 40, h / 2 - 180, 80, 30, { 255,255,255,255 });

	// 绘制音量数值
	float vol = Engine::GetInstance().audio->GetMusicVolume();
	std::string volText = "Vol: " + std::to_string((int)(vol * 100)) + "%";
	Engine::GetInstance().render->DrawText(volText.c_str(), w / 2 - 40, h / 2 + 20, 80, 20, { 255,255,255,255 });
}
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	// 安全关闭设置界面（防止在 Update 循环中删除 UI 导致崩溃）
	if (settingsCloseRequested) {
		DestroySettingsUI();
		settingsCloseRequested = false;
		showSettingsUI = false;
	}

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

	//----------------------------------------------------------------------------F6-----------------------------------------------------------------------------------//
	//L15 TODO 5: Call the function to save entities from the map
	//pulso f6 para guardar donde quiero que el player se vuelve,luego pulsamos f5 para valve ese punto
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		Engine::GetInstance().map->SaveEntities(player);
		Engine::GetInstance().audio->PlayFx(saveFxId);
	}
	//----------------------------------------------------------------------------F11-----------------------------------------------------------------------------------//
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
	//if (uiElement->id == 2) {
	//	LOG("Go to settings");
	//}
	// ID 2: Settings 按钮 (Main Menu)
	// 【修正点】：这里之前只 Log 了，现在加上打开界面的逻辑
	if (uiElement->id == BTN_MAIN_MENU_SETTINGS) { // 也就是 ID 2
		LOG("Opening Settings from Main Menu");
		if (!showSettingsUI) {
			CreateSettingsUI();
			showSettingsUI = true;
		}
	}


	// ---------------------- SETTINGS PANEL EVENTS ---------------------- //
	if (uiElement->id == BTN_SETTINGS_CLOSE) {
		LOG("Closing Settings");
		settingsCloseRequested = true; // 在 PostUpdate 中销毁
		// 如果是在游戏中暂停打开的，关闭设置同时是否要取消暂停？通常保持暂停比较好。
	}

	if (uiElement->id == TOGGLE_FULLSCREEN_ID) {
		Engine::GetInstance().window->ToggleFullsreen();
		LOG("Toggled Fullscreen Mode");
	}

	if (uiElement->id == TOGGLE_MUSIC_ID) {
		// 简单的音乐开关 (静音/恢复)
		float currentVol = Engine::GetInstance().audio->GetMusicVolume();
		if (currentVol > 0.0f) {
			Engine::GetInstance().audio->SetMusicVolume(0.0f); // 关
			LOG("Music OFF");
		}
		else {
			Engine::GetInstance().audio->SetMusicVolume(1.0f); // 开（恢复最大）
			LOG("Music ON");
		}
	}

	if (uiElement->id == BTN_VOL_PLUS) {
		float vol = Engine::GetInstance().audio->GetMusicVolume();
		vol += 0.1f;
		Engine::GetInstance().audio->SetMusicVolume(vol);
		LOG("Volume Up: %f", vol);
	}

	if (uiElement->id == BTN_VOL_MINUS) {
		float vol = Engine::GetInstance().audio->GetMusicVolume();
		vol -= 0.1f;
		Engine::GetInstance().audio->SetMusicVolume(vol);
		LOG("Volume Down: %f", vol);
	}

	// --------------------------------------------------------------------------Pause UI-----------------------------------------------------------------------------------//


	if (uiElement->id == PAUSE_TOGGLE_ID)
	{
		// 使用 dynamic_cast 将基类指针转为子类指针
		UIToggle* toggle = dynamic_cast<UIToggle*>(uiElement);

		if (toggle != nullptr) {
			isGamePaused = toggle->IsOn();

			LOG("Game Paused: %s", isGamePaused ? "YES" : "NO");

			if (isGamePaused) {
				Engine::GetInstance().audio->PauseMusic();
				if (!showSettingsUI) {
					CreateSettingsUI();
					showSettingsUI = true;
				}
			}
			else {
				Engine::GetInstance().audio->ResumeMusic();
				// 恢复时自动关闭设置界面
				if (showSettingsUI) {
					settingsCloseRequested = true;
				}
			}
		}
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
	//Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, "Start", { 520,350,120,20 }, this);
	//// 新增：Settings Button (在 Start 下方)
	//Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, BTN_MAIN_MENU_SETTINGS, "Setting", { 520, 380, 120, 20 }, this);

	// 【修改】创建并保存 Start 按钮
	mainMenuStartBtn = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, "Start", { 520,350,120,20 }, this);

	// 【修改】创建并保存 Settings 按钮
	mainMenuSettingBtn = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, BTN_MAIN_MENU_SETTINGS, "Setting", { 520, 380, 120, 20 }, this);
	showSettingsUI = false;

}
void Scene::UnloadMainMenu() {
	Engine::GetInstance().uiManager->CleanUp();
	if (mainMenuBackground != nullptr) {
		Engine::GetInstance().textures->UnLoad(mainMenuBackground);
		mainMenuBackground = nullptr;
	}
	//
	mainMenuStartBtn = nullptr;
	mainMenuSettingBtn = nullptr;
	showSettingsUI = false;
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
		collectedIDs.clear();
		ChangeScene(SceneID::LEVEL_1);
		break;
	default:
		LOG("UIElement not handled");
		break;
	}
}

// *********************************************
// SETTINGS UI functions
// *********************************************
void Scene::CreateSettingsUI() {
	int w, h;
	Engine::GetInstance().window->GetWindowSize(w, h);
	int centerX = w / 2;
	int centerY = h / 2;

	// 【新增】隐藏主菜单按钮
	if (mainMenuStartBtn != nullptr) mainMenuStartBtn->visible = false;
	if (mainMenuSettingBtn != nullptr) mainMenuSettingBtn->visible = false;

	// Fullscreen Toggle
	Engine::GetInstance().uiManager->CreateUIElement(UIElementType::TOGGLE, TOGGLE_FULLSCREEN_ID, "Fullscreen", { centerX - 50, centerY - 120, 100, 30 }, this);

	// Music Toggle (On/Off)
	Engine::GetInstance().uiManager->CreateUIElement(UIElementType::TOGGLE, TOGGLE_MUSIC_ID, "Music", { centerX - 50, centerY - 70, 100, 30 }, this);

	// Volume Control Buttons
	Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, BTN_VOL_MINUS, "Vol -", { centerX - 80, centerY - 20, 60, 30 }, this);
	Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, BTN_VOL_PLUS, "Vol +", { centerX + 20, centerY - 20, 60, 30 }, this);

	// Close Button
	Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, BTN_SETTINGS_CLOSE, "Close", { centerX - 50, centerY + 100, 100, 30 }, this);
}

void Scene::DestroySettingsUI() {
	// 销毁所有设置界面相关的元素
	Engine::GetInstance().uiManager->DestroyUIElement(TOGGLE_FULLSCREEN_ID);
	Engine::GetInstance().uiManager->DestroyUIElement(TOGGLE_MUSIC_ID);
	Engine::GetInstance().uiManager->DestroyUIElement(BTN_VOL_PLUS);
	Engine::GetInstance().uiManager->DestroyUIElement(BTN_VOL_MINUS);
	Engine::GetInstance().uiManager->DestroyUIElement(BTN_SETTINGS_CLOSE);

	// 【新增】重新显示主菜单按钮
	if (mainMenuStartBtn != nullptr) mainMenuStartBtn->visible = true;
	if (mainMenuSettingBtn != nullptr) mainMenuSettingBtn->visible = true;
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
	showSettingsUI = false; // 确保重置
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");

	int windowW, windowH;
	Engine::GetInstance().window->GetWindowSize(windowW, windowH);

	int iconSize = 32; // 你设置的图标大小
	int margin = 20;   // 离边缘的距离

	// 计算坐标: X = 屏幕宽 - 图标宽 - 边距
	int posX = windowW - iconSize - margin;
	int posY = margin; // 离顶部一点距离

	iconPause = Engine::GetInstance().textures->Load("Assets/Textures/Pause.png");
	iconPlay = Engine::GetInstance().textures->Load("Assets/Textures/Resume.png");

	// 2. 创建 Toggle
	// 位置 { 10, 50, 32, 32 } 根据你的图标大小调整
	auto pauseToggle = Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::TOGGLE,
		PAUSE_TOGGLE_ID,
		"",
		{ posX, posY, iconSize, iconSize }, // 使用计算好的右上角坐标
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
	////full sreen
	//Engine::GetInstance().uiManager->CreateUIElement(UIElementType::TOGGLE, 100, "FullScreen", { 10, 10, 100, 30 }, this);


	//L08: TODO 4: Create a new item using the entity manager and set the position to (200, 672) to test
	//std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
	//item->position = Vector2D(200, 672);
	//item->Start();//!!!
	////Create a new enemy 
	//std::shared_ptr<Enemy> enemy1 = std::dynamic_pointer_cast<Enemy>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
	//enemy1->position = Vector2D(384, 672);
	//enemy1->Start();//importante!!!, tenemos que credar nosotros 
	//este escena no vaterner el botton por lo tanto podemos quitar
	// ================== 【新增：生成测试 Boss】 ==================
	//std::shared_ptr<Enemy> boss = std::dynamic_pointer_cast<Enemy>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));

	//if (boss != nullptr) {
	//	// 1. 【重要】先设置类型为 BOSS，这样 Start() 里才会跑 Boss 的加载逻辑
	//	boss->SetEnemyType(EnemyType::BOSS);

	//	// 2. 设置位置 (建议设置在主角附近，比如 X=500, Y=600)
	//	// 注意：确保这个坐标不在墙里，而在你的地图可视范围内
	//	boss->position = Vector2D(500, 600);

	//	// 3. 启动！(这会调用你刚才写的加载图片和动画的代码)
	//	boss->Start();

	//	LOG("Testing Boss Created!");
	//}
	// ==========================================================






	levelTimer = 60.0f * 1000.0f;
	
}
void Scene::UnloadLevel1() {//limpia la mapa y entity
	Engine::GetInstance().uiManager->CleanUp();
	showSettingsUI = false;
	player.reset();//eliminar player para verificar que todo esta eliminado//opcional
	Engine::GetInstance().entityManager->CleanUp();
	Engine::GetInstance().map->CleanUp();

	// 卸载暂停/播放图标
	if (iconPause != nullptr) {
		Engine::GetInstance().textures->UnLoad(iconPause);
		iconPause = nullptr;
	}
	if (iconPlay != nullptr) {
		Engine::GetInstance().textures->UnLoad(iconPlay);
		iconPlay = nullptr;
	}
	isGameOver = false;
	gameOverShown = false;
	levelTimer = 60.0f * 1000.0f; // ????
	
}
void Scene::UpdateLevel1(float dt) {//para poder cambiar la escena a nivell 2

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL_2);
	}
	/*Engine::GetInstance().uiManager->Update(dt);*/
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
	showSettingsUI = false;
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate2.tmx");
	//L15 TODO 3: Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player);
	//Engine::GetInstance().uiManager->CreateUIElement(UIElementType::TOGGLE, 100, "FullScreen", { 10, 10, 100, 30 }, this);
	levelTimer = 60.0f * 1000.0f;

	
	
}
void Scene::UnloadLevel2() {
	Engine::GetInstance().uiManager->CleanUp();
	showSettingsUI = false;
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

	//  DrawText 
	Engine::GetInstance().render->DrawText(timeText.c_str(), 50, 50, 100, 30, { 255, 255, 255, 255 });

	// GAME OVER
	if (isGameOver) {
		Engine::GetInstance().render->DrawText("GAME OVER", 550, 200, 200, 50, { 255, 0, 0, 255 });
	}
}

