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
#include <time.h>
#include"UIButton.h"
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
	srand(time(NULL));
	collectedIDs.clear();
	Engine::GetInstance().entityManager->CleanUp();
	/*Engine::GetInstance().map->Load("Assets/Maps/MapTemplate.tmx");*/
	LoadScene(currentScene);
	Engine::GetInstance().entityManager->Start();

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

	if (showSettingsUI) {
		int w, h;
		Engine::GetInstance().window->GetWindowSize(w, h);

		// 1. Dibujamos el fondo de Settings (Tu código existente)
		SDL_Rect bg = { w / 2 - 150, h / 2 - 200, 300, 400 };
		Engine::GetInstance().render->DrawRectangle(bg, 50, 50, 50, 240, true, false);
		Engine::GetInstance().render->DrawRectangle(bg, 255, 255, 255, 255, false, false);

		// Si NO estamos en créditos, mostramos el título normal y volumen
		if (!showCreditsUI) {
			Engine::GetInstance().render->DrawText("SETTINGS", w / 2 - 40, h / 2 - 180, 80, 30, { 255,255,255,255 });

			float vol = Engine::GetInstance().audio->GetMusicVolume();
			std::string volText = "Vol: " + std::to_string((int)(vol * 100)) + "%";
			Engine::GetInstance().render->DrawText(volText.c_str(), w / 2 - 40, h / 2 + 20, 80, 20, { 255,255,255,255 });
		}

		// 2. LÓGICA DE CRÉDITOS (SE DIBUJA ENCIMA)
		else {
			// Fondo oscuro (un poco más grande para que quepa todo)
			SDL_Rect creditsBg = { w / 2 + 200, h / 2 - 200, 320, 420 };
			Engine::GetInstance().render->DrawRectangle(creditsBg, 20, 20, 20, 255, true, false);
			Engine::GetInstance().render->DrawRectangle(creditsBg, 255, 255, 255, 255, false, false); // Borde blanco

			int centerX = w / 2;
			int centerY = h / 2;

			int creditsX = centerX + 350;

			// --- TÍTULO ---
			Engine::GetInstance().render->DrawText("CREDITS", creditsX - 50, centerY - 170, 100, 30, { 255, 255, 0, 255 });

			// --- SECCIÓN 1: Game Designer ---
			Engine::GetInstance().render->DrawText("Game Designer", creditsX - 60, centerY - 120, 120, 20, { 200, 200, 200, 255 });
			Engine::GetInstance().render->DrawText("Yangfeng Xu and Bole Wu", creditsX - 95, centerY - 90, 220, 20, { 255, 255, 255, 255 });

			// --- SECCIÓN 2: Programmer ---
			Engine::GetInstance().render->DrawText("Programmer", creditsX - 50, centerY - 40, 100, 20, { 200, 200, 200, 255 });
			Engine::GetInstance().render->DrawText("Yangfeng Xu and Bole Wu", creditsX - 95, centerY - 10, 220, 20, { 255, 255, 255, 255 });

			// --- SECCIÓN 3: Assets ---
			Engine::GetInstance().render->DrawText("Assets & Audio", creditsX - 60, centerY + 40, 120, 20, { 200, 200, 200, 255 });
			Engine::GetInstance().render->DrawText("Free License", creditsX - 50, centerY + 70, 100, 20, { 255, 255, 255, 255 });

			// --- INSTRUCCIÓN SALIDA ---
			Engine::GetInstance().render->DrawText("[Click to Close]", creditsX - 70, centerY + 150, 140, 20, { 150, 150, 150, 255 });

			// Lógica para cerrar al hacer clic
			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
				showCreditsUI = false;
			}
		}

	}
	return true;
}

bool Scene::PostUpdate()
{
	// 1. Cerrar Settings si se ha solicitado
	if (settingsCloseRequested) {
		DestroySettingsUI();
		settingsCloseRequested = false;
		showSettingsUI = false;
	}

	// 2. Gestión de cambio de escena
	if (sceneChangeRequested)
	{
		UnloadCurrentScene();
		currentScene = nextScene;
		LoadScene(currentScene);
		sceneChangeRequested = false;
		isGameOver = false;
		exitGameRequested = false;
	}

	// 3. Salida del juego solicitada por botón UI
	if (exitGameRequested) {
		return false;
	}

	bool ret = true;

	// 4. Dibujar Menú de Ayuda (sin lógica de salir aquí dentro)
	if (showHelpMenu)
	{
		int windowWidth, windowHeight;
		Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);

		int menuWidth = windowWidth * 2 / 3;
		int menuHeight = windowHeight * 2 / 3;
		int menuX = (windowWidth - menuWidth) / 2;
		int menuY = (windowHeight - menuHeight) / 2;

		SDL_Rect backgroundRect = { menuX, menuY, menuWidth, menuHeight };
		Engine::GetInstance().render->DrawRectangle(backgroundRect, 0, 0, 0, 200, true, false);

		if (helpMenuTexture != nullptr)
		{
			Engine::GetInstance().render->DrawTexture(helpMenuTexture, menuX, menuY, nullptr, 0.0f, 0.0, INT_MAX, INT_MAX);
			if (currentScene == SceneID::LEVEL_1) PostUpdateLevel1();
		}
		Engine::GetInstance().render->DrawRectangle(backgroundRect, 255, 255, 255, 255, false, false);
	}

	// ---------------------------------------------------------
	// Gestión de Escenas
	// ---------------------------------------------------------
	switch (currentScene) {
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

	// ---------------------------------------------------------
	// Teclas de función (F5, F6, F11)
	// ---------------------------------------------------------
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		Engine::GetInstance().map->LoadEntities(player);
		reloadCooldown = 500.0f;
	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		Engine::GetInstance().map->SaveEntities(player);
		Engine::GetInstance().audio->PlayFx(saveFxId);
	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {
		int currentFps = Engine::GetInstance().GetTargetFrameRate();
		if (currentFps == 60) {
			Engine::GetInstance().SetTargetFrameRate(30);
			LOG("FPS limit changed to 30");
		}
		else {
			Engine::GetInstance().SetTargetFrameRate(60);
			LOG("FPS limit changed to 60");
		}
	}

	// ---------------------------------------------------------
	// LÓGICA CORREGIDA DE LA TECLA ESCAPE
	// (Debe estar al final y fuera de otros ifs)
	// ---------------------------------------------------------
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		// Caso A: Estamos jugando (Nivel 1 o 2) -> Volver al Menú
		if (currentScene == SceneID::LEVEL_1 || currentScene == SceneID::LEVEL_2) {
			LOG("Escape presionado en nivel. Volviendo al Menú Principal...");
			collectedIDs.clear();
			ChangeScene(SceneID::MAIN_MENU);
		}
		// Caso B: Estamos en el Menú Principal o Intro -> Cerrar el juego
		else if (currentScene == SceneID::MAIN_MENU) {
			LOG("Escape presionado en Menú. Cerrando juego.");
			ret = false;
		}
		// Por defecto, cerrar si es otra escena
		else {
			ret = false;
		}
	}

	return ret;



	//----------------------------------------------------------------------------Currrent Scene-----------------------------------------------------------------------------------//


switch (currentScene) {
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
	if (uiElement->id == BTN_MAIN_MENU_EXIT) {
		LOG("Exiting Application from Main Menu");
		exitGameRequested = true; // Aquí SÍ cerramos la aplicación
	}

	if (uiElement->id == BTN_SETTINGS_CREDITS) {
	LOG("Opening Credits UI");
	showCreditsUI = true; // Activamos la pantalla de créditos
	}

	if (uiElement->id == BTN_SETTINGS_RETURN_MENU) {
		LOG("Returning to Main Menu from Pause Settings");

		// 1. Es importante despausar el juego para que la lógica de la siguiente escena funcione bien
		isGamePaused = false;
		Engine::GetInstance().audio->ResumeMusic();

		// 2. Cerramos la UI de settings (limpieza interna)
		settingsCloseRequested = true;
		showSettingsUI = false;

		// 3. Limpiamos items recolectados si es necesario
		collectedIDs.clear();

		// 4. Cambiamos a la escena del menú
		ChangeScene(SceneID::MAIN_MENU);

		return true;
	}

// --- AÑADIR ESTO PARA PODER CERRAR CRÉDITOS AL CERRAR SETTINGS ---
	if (uiElement->id == BTN_SETTINGS_CLOSE) {
	settingsCloseRequested = true;
	showCreditsUI = false; // Aseguramos que se cierren también
	}
	// ID 2: Settings 按钮 (Main Menu)
	// 【修正点】：这里之前只 Log 了，现在加上打开界面的逻辑
	if (uiElement->id == BTN_MAIN_MENU_SETTINGS) { // 也就是 ID 2
		LOG("Opening Settings from Main Menu");
		if (!showSettingsUI) {
			CreateSettingsUI();
			showSettingsUI = true;
		}
	}
	// 【NUEVO】Lógica para el botón de Salir en la pantalla de Victoria
	if (uiElement->id == BTN_WIN_EXIT) {
		LOG("Victory! Returning to Main Menu...");

		// Limpiamos la lista de IDs recolectados
		collectedIDs.clear();

		// Reseteamos el estado de victoria para que no se quede "pegado"
		isGameWon = false;

		// Volvemos al menú principal
		ChangeScene(SceneID::MAIN_MENU);
		return true;
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
			Engine::GetInstance().scene->collectedIDs.clear();
			ChangeScene(SceneID::LEVEL_1);		
		}
		else if (uiElement->id == EXIT_BTN_ID) {
			// ?????????
			LOG("Returning to Main Menu...");
			Engine::GetInstance().scene->collectedIDs.clear();
			ChangeScene(SceneID::MAIN_MENU);
		
		}
	}
	else if (currentScene == SceneID::LEVEL_2 && isGameOver) {
		if (uiElement->id == RESUME_BTN_ID) {
			LOG("Restarting Level 2...");
			ChangeScene(SceneID::LEVEL_2); // <--- 确保这里重启的是 Level 2
		}
		else if (uiElement->id == EXIT_BTN_ID) {
			LOG("Returning to Main Menu...");
			ChangeScene(SceneID::MAIN_MENU);
			//exitGameRequested = true;
		}
	}


	//----------------------------------------------------------------------------Current Scene-----------------------------------------------------------------------------------//
switch (currentScene) {
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

	// 【修改】创建并保存 Start 按钮
	mainMenuStartBtn = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, "Start", { 575,350,120,20 }, this);

	// 【修改】创建并保存 Settings 按钮
	mainMenuSettingBtn = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, BTN_MAIN_MENU_SETTINGS, "Setting", { 575, 380, 120, 20 }, this);

	mainMenuExitBtn = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, BTN_MAIN_MENU_EXIT, "Exit Game", { 575, 410, 120, 20 }, this);

	showSettingsUI = false;
	showCreditsUI = false;

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
	mainMenuExitBtn = nullptr;
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
	if (mainMenuExitBtn != nullptr) mainMenuExitBtn->visible = false;

	// Fullscreen Toggle
	Engine::GetInstance().uiManager->CreateUIElement(UIElementType::TOGGLE, TOGGLE_FULLSCREEN_ID, "Fullscreen", { centerX - 50, centerY - 120, 100, 30 }, this);

	// Music Toggle (On/Off)
	Engine::GetInstance().uiManager->CreateUIElement(UIElementType::TOGGLE, TOGGLE_MUSIC_ID, "Music", { centerX - 50, centerY - 70, 100, 30 }, this);

	// Volume Control Buttons
	Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, BTN_VOL_MINUS, "Vol -", { centerX - 80, centerY - 20, 60, 30 }, this);
	Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, BTN_VOL_PLUS, "Vol +", { centerX + 20, centerY - 20, 60, 30 }, this);

	Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::BUTTON,
		BTN_SETTINGS_CREDITS,
		"Credits",
		{ centerX - 50, centerY + 40, 100, 30 },
		this
	);

	if (currentScene != SceneID::MAIN_MENU) {
		Engine::GetInstance().uiManager->CreateUIElement(
			UIElementType::BUTTON,
			BTN_SETTINGS_RETURN_MENU,
			"Main Menu",
			{ centerX - 60, centerY + 140, 120, 30 }, // Un poco más abajo del botón Close
			this
		);
	}

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
	Engine::GetInstance().uiManager->DestroyUIElement(BTN_SETTINGS_CREDITS);
	Engine::GetInstance().uiManager->DestroyUIElement(BTN_SETTINGS_RETURN_MENU);

	// 【新增】重新显示主菜单按钮
	if (mainMenuStartBtn != nullptr) mainMenuStartBtn->visible = true;
	if (mainMenuSettingBtn != nullptr) mainMenuSettingBtn->visible = true;
	if (mainMenuExitBtn != nullptr) mainMenuExitBtn->visible = true;
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

	levelUpFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/level_up.wav");
	loseGameFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/lose_game.wav");
	saveFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/autosave.wav");
	levelTimer = 60.0f * 1000.0f;
	//Engine::GetInstance().entityManager->Start();
	
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

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
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
			Engine::GetInstance().audio->PlayFx(levelUpFxId);
			ChangeScene(SceneID::LEVEL_2);
		}
	}

	if (!isGameOver) {
		levelTimer -= dt;
		if (levelTimer <= 0.0f) {
			levelTimer = 0.0f;
			isGameOver = true;
			Engine::GetInstance().audio->PlayFx(loseGameFxId);
			LOG("Game Over!!! Time's Up");
		}
	}


	// 【修改】如果游戏结束，显示 UI 并停止本函数后续逻辑
	if (isGameOver) {
		if (!gameOverShown) {
			// ... (保留原本创建 UI 的代码: RESUME_BTN_ID, EXIT_BTN_ID) ...
			int w, h;
			Engine::GetInstance().window->GetWindowSize(w, h);
			int centerX = w / 2;
			int centerY = h / 2;

			Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, RESUME_BTN_ID, "RESTART", { centerX - 70, centerY - 50, 140, 40 }, this);
			Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, EXIT_BTN_ID, "EXIT", { centerX - 70, centerY + 10, 140, 40 }, this);

			gameOverShown = true;
		}
		return; // 【关键】直接返回，不再执行后续代码
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

	if (player != nullptr) {
		// Mostrar Puntuación (Debajo del tiempo)
		std::string scoreStr = "Score: " + std::to_string(player->score);
		Engine::GetInstance().render->DrawText(scoreStr.c_str(), 20, 80, 100, 30, { 255, 255, 0, 255 }); // Color amarillo

		// Mostrar Munición (Debajo de la puntuación)
		std::string ammoStr = "Ammo: " + std::to_string(player->ammo);
		Engine::GetInstance().render->DrawText(ammoStr.c_str(), 20, 110, 100, 30, { 0, 255, 255, 255 }); // Color cian
	}

	
	int secondsLeft = (int)(levelTimer / 1000.0f);

	std::string timeText = "Time: " + std::to_string(secondsLeft);

	
	Engine::GetInstance().render->DrawText(timeText.c_str(), 20, 50, 100, 30, { 255, 255, 255, 255 });

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

	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Level.wav");
	Engine::GetInstance().render->camera.x = 0;
	Engine::GetInstance().render->camera.y = 0;

	isGameOver = false;
	gameOverShown = false;
	showSettingsUI = false;
	winButton = nullptr;
	isBossMusicPlaying = false;
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate2.tmx");
	//L15 TODO 3: Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player);
	//Engine::GetInstance().uiManager->CreateUIElement(UIElementType::TOGGLE, 100, "FullScreen", { 10, 10, 100, 30 }, this);
	levelTimer = 60.0f * 1000.0f;

	//--------------------------------------------------------------------------------win-----------------------------------------------------------------------------------//
	// 1. 初始化变量
	isGameWon = false;
	bossReference = nullptr;
	// ------------------------------------------------------------------------
	// 【关键修改】遍历实体列表，找到地图生成的 Boss 并赋值给 bossReference
	// ------------------------------------------------------------------------
	// 注意：这里假设 EntityManager 中的 list 叫 entities，如果是其他名字请修改 (如 entitiesList)
	for (const auto& entity : Engine::GetInstance().entityManager->entities) {
		// 判断是否是敌人
		if (entity->type == EntityType::ENEMY) {
			// 尝试转换为 Enemy 指针
			std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(entity);

			// 如果转换成功，并且类型是 BOSS
			if (enemy != nullptr && enemy->enemyType == EnemyType::BOSS) {
				bossReference = enemy; // 【成功连接引用】
				LOG("Boss loaded from Map and connected to Logic!");
				break; // 找到了就退出循环
			}
		}
	}


	// 2. 加载胜利画面资源 (确保你有一张图片叫 win_screen.png 或者你可以暂时用 menu.png 代替)
	winScreenTexture = Engine::GetInstance().textures->Load("Assets/Textures/menu_resized.png"); // 这里替换成你的胜利图片路径
	winFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/game_win.wav"); // 加载你文件列表中有的胜利音效
	bossAttackFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/boss_atack.wav");
	loseGameFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/lose_game.wav");

	// ... 地图加载代码 ...


	// --- AÑADIR ESTO: Lógica del Botón de Pausa ---
	int windowW, windowH;
	Engine::GetInstance().window->GetWindowSize(windowW, windowH);

	int iconSize = 32;
	int margin = 20;
	int posX = windowW - iconSize - margin;
	int posY = margin;

	// Cargar texturas (reutilizamos las variables de la clase Scene)
	iconPause = Engine::GetInstance().textures->Load("Assets/Textures/Pause.png");
	iconPlay = Engine::GetInstance().textures->Load("Assets/Textures/Resume.png");

	// Crear el Toggle
	auto pauseToggle = Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::TOGGLE,
		PAUSE_TOGGLE_ID,
		"",
		{ posX, posY, iconSize, iconSize },
		this
	);

	// Configurar texturas del Toggle
	auto togglePtr = std::dynamic_pointer_cast<UIToggle>(pauseToggle);
	if (togglePtr) {
		togglePtr->SetTextures(iconPause, iconPlay);
	}
	
	
}
void Scene::UnloadLevel2() {
	Engine::GetInstance().uiManager->CleanUp();
	showSettingsUI = false;
	player.reset();//eliminar player para verificar que todo esta eliminado//opcional
	Engine::GetInstance().entityManager->CleanUp();
	Engine::GetInstance().map->CleanUp();
	isGameOver = false;
	gameOverShown = false;
	levelTimer = 60.0f * 1000.0f; 
	if (iconPause != nullptr) {
		Engine::GetInstance().textures->UnLoad(iconPause);
		iconPause = nullptr;
	}
	if (iconPlay != nullptr) {
		Engine::GetInstance().textures->UnLoad(iconPlay);
		iconPlay = nullptr;
	}

	if (winScreenTexture != nullptr) {
		Engine::GetInstance().textures->UnLoad(winScreenTexture);
		winScreenTexture = nullptr;
	}
	winButton = nullptr;
	bossReference = nullptr; 
	isGameWon = false;
}
void Scene::UpdateLevel2(float dt) {//cambiar a nivell 1
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL_1);
	}

	// ------------------------------------------------------------------------
	// 1. 胜利状态逻辑 (游戏静止)
	// ------------------------------------------------------------------------
	if (isGameWon) {
		return; // 直接返回，停止后续所有逻辑 (计时器、移动等)
	}

	// 2. 暂停状态逻辑
	if (isGamePaused) {
		return;
	}

	if (player != nullptr && bossReference != nullptr && !isBossMusicPlaying) {

		// Calculamos distancia
		float distanceX = abs(bossReference->position.getX() - player->GetPosition().getX());

		// Si estamos a menos de 800 píxeles...
		if (distanceX < 800.0f) {
			LOG("Entering Boss Battle Zone!");

			// CAMBIO AQUÍ: Reproducir "boss_battle.wav"
			Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/boss_battle.wav");

			// Marcamos como true para que no se reinicie en cada frame
			isBossMusicPlaying = true;
		}
	}
	
	if (!isGameOver) {
		levelTimer -= dt;
		if (levelTimer <= 0.0f) {
			levelTimer = 0.0f;
			isGameOver = true;
			Engine::GetInstance().audio->PlayFx(loseGameFxId);
			LOG("Game Over!!! Time's Up");
		}
	}
	//// 1. 如果游戏暂停 OR 已经胜利，直接 return，实现画面静止
	// 2. 检测 Boss 是否死亡
	if (bossReference != nullptr && !bossReference->active) { // 假设 active 为 false 代表死亡/消失，或者用 bossReference->isDead
		// 注意：你需要确保 Enemy 类中有 isDead 属性且在死亡时设为 true
		// 如果 Enemy.h 中 isDead 是 private，你需要加一个 Getter，或者直接判断 active

		// 触发胜利
		isGameWon = true;
		LOG("BOSS DEFEATED! YOU WIN!");

		// 播放胜利音效
		Engine::GetInstance().audio->PlayFx(winFxId);
		Engine::GetInstance().audio->PauseMusic(); // 停止背景音乐

		// 创建 UI (显示 "Exit Game" 按钮)
		int w, h;
		Engine::GetInstance().window->GetWindowSize(w, h);

		// 在屏幕下方创建退出按钮 (复用 BTN_WIN_EXIT ID)
		winButton=Engine::GetInstance().uiManager->CreateUIElement(
			UIElementType::BUTTON,
			BTN_WIN_EXIT,
			"EXIT GAME",
			{ w / 2 - 60, h / 2 + 50, 120, 30 },
			this
		);

		return; // 立即停止本帧后续逻辑
	}

	// 【修改】如果游戏结束，显示 UI 并停止本函数后续逻辑
	if (isGameOver) {
		if (!gameOverShown) {
			// ... (保留原本创建 UI 的代码: RESUME_BTN_ID, EXIT_BTN_ID) ...
			int w, h;
			Engine::GetInstance().window->GetWindowSize(w, h);
			int centerX = w / 2;
			int centerY = h / 2;

			Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, RESUME_BTN_ID, "RESTART", { centerX - 70, centerY - 50, 140, 40 }, this);
			Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, EXIT_BTN_ID, "EXIT", { centerX - 70, centerY + 10, 140, 40 }, this);

			gameOverShown = true;
		}
		return; // 【关键】直接返回，不再执行后续代码
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
	// ------------------------------------------------------------------------
	// 【新增】绘制胜利画面 (这一层会盖住地图和Player)
	// ------------------------------------------------------------------------
	if (isGameWon) {
		int w, h;
		Engine::GetInstance().window->GetWindowSize(w, h);

		// 1. 绘制半透明黑色遮罩 (全屏)
		SDL_Rect screenRect = { 0, 0, w, h };
		Engine::GetInstance().render->DrawRectangle(screenRect, 0, 0, 0, 150, true, false);

		// 2. 绘制胜利图片 (全屏覆盖或居中)
		if (winScreenTexture != nullptr) {
			Engine::GetInstance().render->DrawTexture(winScreenTexture, 0, 0, nullptr, 0.0f);
		}

		// 3. 绘制胜利文字
		Engine::GetInstance().render->DrawText("MISSION ACCOMPLISHED", w / 2 - 150, h / 2 - 50, 300, 50, { 0, 255, 0, 255 });
		Engine::GetInstance().render->DrawText("Boss Defeated!", w / 2 - 80, h / 2, 160, 30, { 255, 255, 255, 255 });

		if (winButton != nullptr) {
			winButton->Render(); // ¡Ahora sí funciona!
		}
	}

	if (player != nullptr) {
		// Mostrar Puntuación (Debajo del tiempo)
		std::string scoreStr = "Score: " + std::to_string(player->score);
		// Posición Y=80 (debajo del Time que está en 50)
		Engine::GetInstance().render->DrawText(scoreStr.c_str(), 20, 80, 100, 30, { 255, 255, 0, 255 }); // Amarillo

		// Mostrar Munición (Debajo de la puntuación)
		std::string ammoStr = "Ammo: " + std::to_string(player->ammo);
		// Posición Y=110
		Engine::GetInstance().render->DrawText(ammoStr.c_str(), 20, 110, 100, 30, { 0, 255, 255, 255 }); // Cian
	}

	
	int secondsLeft = (int)(levelTimer / 1000.0f);

	
	std::string timeText = "Time: " + std::to_string(secondsLeft);

	//  DrawText 
	Engine::GetInstance().render->DrawText(timeText.c_str(), 20, 50, 100, 30, { 255, 255, 255, 255 });

	// GAME OVER
	if (isGameOver) {
		Engine::GetInstance().render->DrawText("GAME OVER", 550, 200, 200, 50, { 255, 0, 0, 255 });
	}
}

