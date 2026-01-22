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
	collectedIDs.clear();
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	srand(time(NULL));
	collectedIDs.clear();
	Engine::GetInstance().entityManager->CleanUp();
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

		// 1. We draw the background of Settings
		SDL_Rect bg = { w / 2 - 150, h / 2 - 200, 300, 400 };
		Engine::GetInstance().render->DrawRectangle(bg, 50, 50, 50, 240, true, false);
		Engine::GetInstance().render->DrawRectangle(bg, 255, 255, 255, 255, false, false);

		// If we are NOT in the credits, we show the normal title and volume
		if (!showCreditsUI) {
			Engine::GetInstance().render->DrawText("SETTINGS", w / 2 - 40, h / 2 - 180, 80, 30, { 255,255,255,255 });

			float vol = Engine::GetInstance().audio->GetMusicVolume();
			std::string volText = "Vol: " + std::to_string((int)(vol * 100)) + "%";
			Engine::GetInstance().render->DrawText(volText.c_str(), w / 2 - 40, h / 2 + 20, 80, 20, { 255,255,255,255 });
		}

		// 2. CREDIT LOGIC
		else {
			// Dark background (a little bigger so everything fits)
			SDL_Rect creditsBg = { w / 2 + 200, h / 2 - 200, 320, 420 };
			Engine::GetInstance().render->DrawRectangle(creditsBg, 20, 20, 20, 255, true, false);
			Engine::GetInstance().render->DrawRectangle(creditsBg, 255, 255, 255, 255, false, false); // Borde blanco

			int centerX = w / 2;
			int centerY = h / 2;

			int creditsX = centerX + 350;

			// --- TITLE ---
			Engine::GetInstance().render->DrawText("CREDITS", creditsX - 50, centerY - 170, 100, 30, { 255, 255, 0, 255 });

			// --- SECTION 1: Game Designer ---
			Engine::GetInstance().render->DrawText("Game Designer", creditsX - 60, centerY - 120, 120, 20, { 200, 200, 200, 255 });
			Engine::GetInstance().render->DrawText("Yangfeng Xu and Bole Wu", creditsX - 95, centerY - 90, 220, 20, { 255, 255, 255, 255 });

			// --- SECTION 2: Programmer ---
			Engine::GetInstance().render->DrawText("Programmer", creditsX - 50, centerY - 40, 100, 20, { 200, 200, 200, 255 });
			Engine::GetInstance().render->DrawText("Yangfeng Xu and Bole Wu", creditsX - 95, centerY - 10, 220, 20, { 255, 255, 255, 255 });

			// --- SECTION 3: Assets ---
			Engine::GetInstance().render->DrawText("Assets & Audio", creditsX - 60, centerY + 40, 120, 20, { 200, 200, 200, 255 });
			Engine::GetInstance().render->DrawText("Free License", creditsX - 50, centerY + 70, 100, 20, { 255, 255, 255, 255 });

			// --- EXIT INSTRUCTION ---
			Engine::GetInstance().render->DrawText("[Click to Close]", creditsX - 70, centerY + 150, 140, 20, { 150, 150, 150, 255 });

			// Logic to close on click
			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
				showCreditsUI = false;
			}
		}

	}
	return true;
}

bool Scene::PostUpdate()
{
	// 1. Close Settings if prompted
	if (settingsCloseRequested) {
		DestroySettingsUI();
		settingsCloseRequested = false;
		showSettingsUI = false;
	}

	// 2. Scene change management
	if (sceneChangeRequested)
	{
		UnloadCurrentScene();
		currentScene = nextScene;
		LoadScene(currentScene);
		sceneChangeRequested = false;
		isGameOver = false;
		exitGameRequested = false;
	}

	// 3. Exiting the game requested by UI button
	if (exitGameRequested) {
		return false;
	}

	bool ret = true;

	// 4. Draw Help Menu (without logic to exit from here)
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
	//Scene Management
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
	// Function keys (F5, F6, F11)
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

	// ESC
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		// Case A: We are playing (Level 1 or 2) -> Back to Menu
		if (currentScene == SceneID::LEVEL_1 || currentScene == SceneID::LEVEL_2) {
			LOG("Escape presionado en nivel. Volviendo al Menú Principal...");
			collectedIDs.clear();
			ChangeScene(SceneID::MAIN_MENU);
		}
		// Case B: We are on the Main Menu or Enter screen -> Close the game
		else if (currentScene == SceneID::MAIN_MENU) {
			LOG("Escape presionado en Menú. Cerrando juego.");
			ret = false;
		}
		// By default, close if it's another scene
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

		// 2. We load the entities (this will create new ones and move the Player)
		Engine::GetInstance().map->LoadEntities(player);

		reloadCooldown = 500.0f;
	}

	//----------------------------------------------------------------------------F6-----------------------------------------------------------------------------------//
	//L15 TODO 5: Call the function to save entities from the map
	// I press F6 to save where I want the player to turn, then I press F5 to save that point
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
	if (uiElement->id == 1) {
		LOG("Start game");
	}

	if (uiElement->id == 1) {
		LOG("Start New Game");
		loadFromSave = false; // Importante: resetear bandera
		collectedIDs.clear(); // Limpiar items recogidos
		deadEnemyIDs.clear(); // <--- ¡AÑADIR ESTO! Limpiar enemigos muertos
		ChangeScene(SceneID::LEVEL_1);
	}

	// --- AÑADIR ESTO: Lógica Botón Continuar ---
	if (uiElement->id == BTN_MAIN_MENU_CONTINUE) {
		LOG("Continue Game Requested");
		collectedIDs.clear();
		LoadGame(); // Esto lee el archivo y cambia de escena
		return true;
	}

	// --- AÑADIR ESTO: Lógica Botón Guardar (en Pausa) ---
	if (uiElement->id == BTN_SETTINGS_SAVE) {
		LOG("Saving Game...");
		SaveGame();
		Engine::GetInstance().audio->PlayFx(saveFxId); // Sonido de guardado
		return true;
	}

	if (uiElement->id == BTN_MAIN_MENU_EXIT) {
		LOG("Exiting Application from Main Menu");
		exitGameRequested = true; 
	}

	if (uiElement->id == BTN_SETTINGS_CREDITS) {
	LOG("Opening Credits UI");
	showCreditsUI = true; 
	}

	if (uiElement->id == BTN_SETTINGS_RETURN_MENU) {
		LOG("Returning to Main Menu from Pause Settings");

		// 1. It is important to unpause the game so that the logic of the next scene works properly
		isGamePaused = false;
		Engine::GetInstance().audio->ResumeMusic();

		// 2. We close the settings UI (internal cleaning)
		settingsCloseRequested = true;
		showSettingsUI = false;

		// 3. We clean up collected items if necessary
		collectedIDs.clear();

		// 4. We switch to the menu scene
		ChangeScene(SceneID::MAIN_MENU);

		return true;
	}

	if (uiElement->id == BTN_SETTINGS_CLOSE) {
	settingsCloseRequested = true;
	showCreditsUI = false; 
	}
	if (uiElement->id == BTN_MAIN_MENU_SETTINGS) { 
		LOG("Opening Settings from Main Menu");
		if (!showSettingsUI) {
			CreateSettingsUI();
			showSettingsUI = true;
		}
	}
	// Logic for the Exit button on the Victoria screen
	if (uiElement->id == BTN_WIN_EXIT) {
		LOG("Victory! Returning to Main Menu...");

		// We cleaned the list of collected IDs
		collectedIDs.clear();

		// We reset the victory state so that it doesn't get "stuck".
		isGameWon = false;

		// Volvemos al menú principal
		ChangeScene(SceneID::MAIN_MENU);
		return true;
	}

	// ---------------------- SETTINGS PANEL EVENTS ---------------------- //
	if (uiElement->id == BTN_SETTINGS_CLOSE) {
		LOG("Closing Settings");
		settingsCloseRequested = true; 
	}

	if (uiElement->id == TOGGLE_FULLSCREEN_ID) {
		Engine::GetInstance().window->ToggleFullsreen();
		LOG("Toggled Fullscreen Mode");
	}

	if (uiElement->id == TOGGLE_MUSIC_ID) {
		// Simple music switch (mute/restore)
		float currentVol = Engine::GetInstance().audio->GetMusicVolume();
		if (currentVol > 0.0f) {
			Engine::GetInstance().audio->SetMusicVolume(0.0f); // off
			LOG("Music OFF");
		}
		else {
			Engine::GetInstance().audio->SetMusicVolume(1.0f); // on
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
				// Automatically close the settings interface upon restoration
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
			LOG("Returning to Main Menu...");
			Engine::GetInstance().scene->collectedIDs.clear();
			ChangeScene(SceneID::MAIN_MENU);
		
		}
	}
	else if (currentScene == SceneID::LEVEL_2 && isGameOver) {
		if (uiElement->id == RESUME_BTN_ID) {
			LOG("Restarting Level 2...");
			ChangeScene(SceneID::LEVEL_2); 
		}
		else if (uiElement->id == EXIT_BTN_ID) {
			LOG("Returning to Main Menu...");
			ChangeScene(SceneID::MAIN_MENU);
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
void Scene::LoadScene(SceneID newScene) {
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

	sceneChangeRequested = true;
	nextScene = newScene;
}

// *********************************************
// MAIN MENU functions
// *********************************************

// L17 TODO 3: Define specific function for main menu scene: Load, Unload, Handle UI events
void Scene::LoadMainMenu() {
	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/retro-gaming-short-248416.wav");
	Engine::GetInstance().render->camera.x = 0;
	Engine::GetInstance().render->camera.y = 0;


	mainMenuBackground = Engine::GetInstance().textures->Load("Assets/Textures/menu_resized.png");
	mainMenuStartBtn = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, "Start", { 575,350,120,20 }, this);
	mainMenuSettingBtn = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, BTN_MAIN_MENU_SETTINGS, "Setting", { 575, 380, 120, 20 }, this);
	mainMenuExitBtn = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, BTN_MAIN_MENU_EXIT, "Exit Game", { 575, 410, 120, 20 }, this);
	mainMenuStartBtn = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, "New Game", { 575,350,120,20 }, this);

	std::ifstream file("savegame.txt");
	if (file.good()) {
		// Si existe, creamos el botón CONTINUE
		mainMenuContinueBtn = Engine::GetInstance().uiManager->CreateUIElement(
			UIElementType::BUTTON,
			BTN_MAIN_MENU_CONTINUE,
			"Continue",
			{ 575, 320, 120, 20 }, // Un poco más arriba que Start
			this
		);
		file.close();
	}
	else {
		mainMenuContinueBtn = nullptr;
	}

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
	mainMenuContinueBtn = nullptr;
}
void Scene::UpdateMainMenu(float dt) {
	if (mainMenuBackground != nullptr) {
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
			{ centerX - 60, centerY + 140, 120, 30 }, 
			this
		);
	}

	if (currentScene != SceneID::MAIN_MENU) {

		// Botón Return to Menu (que ya hiciste)
		Engine::GetInstance().uiManager->CreateUIElement(
			UIElementType::BUTTON,
			BTN_SETTINGS_RETURN_MENU,
			"Main Menu",
			{ centerX - 60, centerY + 140, 120, 30 },
			this
		);

		// Botón SAVE GAME (Nuevo) - Lo ponemos encima de "Main Menu" o donde quepa
		Engine::GetInstance().uiManager->CreateUIElement(
			UIElementType::BUTTON,
			BTN_SETTINGS_SAVE,
			"Save Game",
			{ centerX - 60, centerY + 180, 120, 30 }, // Ajusta la posición Y según necesites
			this
		);
	}

	// Close Button
	Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, BTN_SETTINGS_CLOSE, "Close", { centerX - 50, centerY + 100, 100, 30 }, this);
}

void Scene::DestroySettingsUI() {
	// Destroy all elements related to the settings interface
	Engine::GetInstance().uiManager->DestroyUIElement(TOGGLE_FULLSCREEN_ID);
	Engine::GetInstance().uiManager->DestroyUIElement(TOGGLE_MUSIC_ID);
	Engine::GetInstance().uiManager->DestroyUIElement(BTN_VOL_PLUS);
	Engine::GetInstance().uiManager->DestroyUIElement(BTN_VOL_MINUS);
	Engine::GetInstance().uiManager->DestroyUIElement(BTN_SETTINGS_CLOSE);
	Engine::GetInstance().uiManager->DestroyUIElement(BTN_SETTINGS_CREDITS);
	Engine::GetInstance().uiManager->DestroyUIElement(BTN_SETTINGS_RETURN_MENU);
	Engine::GetInstance().uiManager->DestroyUIElement(BTN_SETTINGS_SAVE);

	
	if (mainMenuStartBtn != nullptr) mainMenuStartBtn->visible = true;
	if (mainMenuSettingBtn != nullptr) mainMenuSettingBtn->visible = true;
	if (mainMenuExitBtn != nullptr) mainMenuExitBtn->visible = true;
}
// *********************************************
// Level 1 functions
// *********************************************

// L17 TODO 4: Define specific functions for level1 scene: Load, Unload, Update, PostUpdate
void Scene::LoadLevel1() {//load map, texture, audio
	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Level.wav");

	Engine::GetInstance().render->camera.x = 0;
	Engine::GetInstance().render->camera.y = 0;
	isGameOver = false;
	gameOverShown = false;
	showSettingsUI = false;

	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");
	int windowW, windowH;
	Engine::GetInstance().window->GetWindowSize(windowW, windowH);
	int iconSize = 32; 
	int margin = 20;
	int posX = windowW - iconSize - margin;
	int posY = margin; 

	iconPause = Engine::GetInstance().textures->Load("Assets/Textures/Pause.png");
	iconPlay = Engine::GetInstance().textures->Load("Assets/Textures/Resume.png");
	
	// Create Toggle
	auto pauseToggle = Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::TOGGLE,
		PAUSE_TOGGLE_ID,
		"",
		{ posX, posY, iconSize, iconSize }, 
		this
	);

	auto togglePtr = std::dynamic_pointer_cast<UIToggle>(pauseToggle);
	if (togglePtr) {
		togglePtr->SetTextures(iconPause, iconPlay);
	}

	Engine::GetInstance().map->LoadEntities(player);

	if (loadFromSave) {
		LOG("Loading Game Data in Level 1...");

		// A) Restaurar posición y datos del Player
		if (player != nullptr) {
			player->SetPosition(savedPlayerPos);
			player->score = savedScore;
			player->ammo = savedAmmo;

			// Ajustar la cámara a la posición cargada
			int w, h;
			Engine::GetInstance().window->GetWindowSize(w, h);
			Engine::GetInstance().render->camera.x = -player->GetPosition().getX() + (w / 2);
		}

		// B) Eliminar entidades que ya no deberían existir (Items recogidos y Enemigos muertos)
		auto& entities = Engine::GetInstance().entityManager->entities;

		for (auto it = entities.begin(); it != entities.end(); ) {
			bool shouldRemove = false;

			// Verificar si es un ITEM recogido
			if ((*it)->type == EntityType::ITEM) {
				for (int id : collectedIDs) {
					if ((*it)->id == id) {
						shouldRemove = true;
						break;
					}
				}
			}
			// Verificar si es un ENEMIGO muerto
			else if ((*it)->type == EntityType::ENEMY) {
				for (int id : deadEnemyIDs) {
					if ((*it)->id == id) {
						shouldRemove = true;
						break;
					}
				}
			}

			if (shouldRemove) {
				// Borrar de la lista y actualizar el iterador
				it = entities.erase(it);
			}
			else {
				++it;
			}
		}

		// Resetear la bandera para que no afecte a futuros reinicios
		loadFromSave = false;
	}

	// Texture to highligh mouse position 
	mouseTileTex = Engine::GetInstance().textures->Load("Assets/Maps/MapMetadata.png");
	helpMenuTexture = Engine::GetInstance().textures->Load("Assets/Maps/menu.png");

	levelUpFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/level_up.wav");
	loseGameFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/lose_game.wav");
	saveFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/autosave.wav");
	levelTimer = 60.0f * 1000.0f;
}
void Scene::UnloadLevel1() {//cleans the map and entity
	Engine::GetInstance().uiManager->CleanUp();
	showSettingsUI = false;
	player.reset();
	Engine::GetInstance().entityManager->CleanUp();
	Engine::GetInstance().map->CleanUp();

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
	levelTimer = 60.0f * 1000.0f; 
	
}
void Scene::UpdateLevel1(float dt) {//to be able to change the scene to level 2

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL_2);
	}
	// If the game is paused, return directly without executing the following logic.
	if (isGamePaused) {
		return;
	}

	if (player != nullptr)
	{
		Vector2D pos = player->GetPosition();

		//Level 1 finish line
		float targetX_Min = 1950.0f; 
		float targetX_Max = 2000.0f; 
		float targetY_Max = 160.0f;  

		
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


	
	if (isGameOver) {
		if (!gameOverShown) {
			
			int w, h;
			Engine::GetInstance().window->GetWindowSize(w, h);
			int centerX = w / 2;
			int centerY = h / 2;

			Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, RESUME_BTN_ID, "RESTART", { centerX - 70, centerY - 50, 140, 40 }, this);
			Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, EXIT_BTN_ID, "EXIT", { centerX - 70, centerY + 10, 140, 40 }, this);

			gameOverShown = true;
		}
		return; 
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

		std::string scoreStr = "Score: " + std::to_string(player->score);
		Engine::GetInstance().render->DrawText(scoreStr.c_str(), 20, 80, 100, 30, { 255, 255, 0, 255 }); // Color amarillo

		std::string ammoStr = "Ammo: " + std::to_string(player->ammo);
		Engine::GetInstance().render->DrawText(ammoStr.c_str(), 20, 110, 100, 30, { 0, 255, 255, 255 }); // Color cian
	}

	
	int secondsLeft = (int)(levelTimer / 1000.0f);
	std::string timeText = "Time: " + std::to_string(secondsLeft);

	Engine::GetInstance().render->DrawText(timeText.c_str(), 20, 50, 100, 30, { 255, 255, 255, 255 });

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
	levelTimer = 60.0f * 1000.0f;

	//--------------------------------------------------------------------------------win-----------------------------------------------------------------------------------//
	isGameWon = false;
	bossReference = nullptr;
	for (const auto& entity : Engine::GetInstance().entityManager->entities) {
		
		if (entity->type == EntityType::ENEMY) {
			
			std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(entity);

			if (enemy != nullptr && enemy->enemyType == EnemyType::BOSS) {
				bossReference = enemy;
				LOG("Boss loaded from Map and connected to Logic!");
				break; 
			}
		}
	}

	winScreenTexture = Engine::GetInstance().textures->Load("Assets/Textures/menu_resized.png");
	winFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/game_win.wav"); 
	bossAttackFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/boss_atack.wav");
	loseGameFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Music/lose_game.wav");

	// --- Pause Button Logic ---
	int windowW, windowH;
	Engine::GetInstance().window->GetWindowSize(windowW, windowH);

	int iconSize = 32;
	int margin = 20;
	int posX = windowW - iconSize - margin;
	int posY = margin;

	// Load textures (we reuse the variables from the Scene class)
	iconPause = Engine::GetInstance().textures->Load("Assets/Textures/Pause.png");
	iconPlay = Engine::GetInstance().textures->Load("Assets/Textures/Resume.png");

	// Create the Toggle
	auto pauseToggle = Engine::GetInstance().uiManager->CreateUIElement(
		UIElementType::TOGGLE,
		PAUSE_TOGGLE_ID,
		"",
		{ posX, posY, iconSize, iconSize },
		this
	);

	// Configure toggle textures
	auto togglePtr = std::dynamic_pointer_cast<UIToggle>(pauseToggle);
	if (togglePtr) {
		togglePtr->SetTextures(iconPause, iconPlay);
	}

	if (loadFromSave) {
		LOG("Loading Game Data in Level 1...");

		// A) Restaurar posición y datos del Player
		if (player != nullptr) {
			player->SetPosition(savedPlayerPos);
			player->score = savedScore;
			player->ammo = savedAmmo;

			// Ajustar la cámara a la posición cargada
			int w, h;
			Engine::GetInstance().window->GetWindowSize(w, h);
			Engine::GetInstance().render->camera.x = -player->GetPosition().getX() + (w / 2);
		}

		// B) Eliminar entidades que ya no deberían existir (Items recogidos y Enemigos muertos)
		auto& entities = Engine::GetInstance().entityManager->entities;

		for (auto it = entities.begin(); it != entities.end(); ) {
			bool shouldRemove = false;

			// Verificar si es un ITEM recogido
			if ((*it)->type == EntityType::ITEM) {
				for (int id : collectedIDs) {
					if ((*it)->id == id) {
						shouldRemove = true;
						break;
					}
				}
			}
			// Verificar si es un ENEMIGO muerto
			else if ((*it)->type == EntityType::ENEMY) {
				for (int id : deadEnemyIDs) {
					if ((*it)->id == id) {
						shouldRemove = true;
						break;
					}
				}
			}

			if (shouldRemove) {
				// Borrar de la lista y actualizar el iterador
				it = entities.erase(it);
			}
			else {
				++it;
			}
		}

		// Resetear la bandera para que no afecte a futuros reinicios
		loadFromSave = false;
	}
	
	
}
void Scene::UnloadLevel2() {
	Engine::GetInstance().uiManager->CleanUp();
	showSettingsUI = false;
	player.reset();
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
void Scene::UpdateLevel2(float dt) {//change to level 1
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL_1);
	}


	// ------------------------------------------------------------------------
	// 1. Victory State Logic (Game at Rest)
	// ------------------------------------------------------------------------
	if (isGameWon) {
		return; // Return immediately, stopping all subsequent logic (timers, movement, etc.)
	}

	// 2. Pause State Logic
	if (isGamePaused) {
		return;
	}

	if (player != nullptr && bossReference != nullptr && !isBossMusicPlaying) {

		// We calculate distance
		float distanceX = abs(bossReference->position.getX() - player->GetPosition().getX());
		if (distanceX < 800.0f) {
			LOG("Entering Boss Battle Zone!");
			Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/boss_battle.wav");
			// We mark it as true so that it doesn't restart on every frame
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
	// 1. If the game is paused or a victory has been achieved, return immediately to freeze the screen.
	// 2. Detect if the Boss is dead.
	if (bossReference != nullptr && !bossReference->active) { 
		// Trigger victory
		isGameWon = true;
		LOG("BOSS DEFEATED! YOU WIN!");

		// Play victory sound effect
		Engine::GetInstance().audio->PlayFx(winFxId);
		Engine::GetInstance().audio->PauseMusic(); // Stop background music

		// Create UI (display the "Exit Game" button)
		int w, h;
		Engine::GetInstance().window->GetWindowSize(w, h);

		// Create an exit button at the bottom of the screen (reuse the BTN_WIN_EXIT ID)
		winButton=Engine::GetInstance().uiManager->CreateUIElement(
			UIElementType::BUTTON,
			BTN_WIN_EXIT,
			"EXIT GAME",
			{ w / 2 - 60, h / 2 + 50, 120, 30 },
			this
		);

		return; // Immediately stop subsequent logic in this frame
	}


	if (isGameOver) {
		if (!gameOverShown) {

			int w, h;
			Engine::GetInstance().window->GetWindowSize(w, h);
			int centerX = w / 2;
			int centerY = h / 2;

			Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, RESUME_BTN_ID, "RESTART", { centerX - 70, centerY - 50, 140, 40 }, this);
			Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, EXIT_BTN_ID, "EXIT", { centerX - 70, centerY + 10, 140, 40 }, this);

			gameOverShown = true;
		}
		return; 
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

	// Win scene
	if (isGameWon) {
		int w, h;
		Engine::GetInstance().window->GetWindowSize(w, h);

		SDL_Rect screenRect = { 0, 0, w, h };
		Engine::GetInstance().render->DrawRectangle(screenRect, 0, 0, 0, 150, true, false);
		if (winScreenTexture != nullptr) {
			Engine::GetInstance().render->DrawTexture(winScreenTexture, 0, 0, nullptr, 0.0f);
		}

		Engine::GetInstance().render->DrawText("MISSION ACCOMPLISHED", w / 2 - 150, h / 2 - 50, 300, 50, { 0, 255, 0, 255 });
		Engine::GetInstance().render->DrawText("Boss Defeated!", w / 2 - 80, h / 2, 160, 30, { 255, 255, 255, 255 });

		if (winButton != nullptr) {
			winButton->Render(); 
		}
	}

	if (player != nullptr) {
		// Show Score (Below the time)
		std::string scoreStr = "Score: " + std::to_string(player->score);
		// Position Y=80 (below the Time which is at 50)
		Engine::GetInstance().render->DrawText(scoreStr.c_str(), 20, 80, 100, 30, { 255, 255, 0, 255 }); // Amarillo
		// Show Ammunition (Below the score)
		std::string ammoStr = "Ammo: " + std::to_string(player->ammo);
		// Position Y=110
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

void Scene::SaveGame() {
	if (player != nullptr && (currentScene == SceneID::LEVEL_1 || currentScene == SceneID::LEVEL_2)) {
		std::ofstream file("savegame.txt");
		if (file.is_open()) {
			// 1. Datos básicos
			file << (int)currentScene << " "
				<< player->GetPosition().getX() << " "
				<< player->GetPosition().getY() << " "
				<< player->score << " "
				<< player->ammo << " "; // Añadir un espacio al final

			// 2. Guardar Items recogidos
			file << collectedIDs.size() << " ";
			for (int id : collectedIDs) file << id << " ";

			// 3. Guardar Enemigos muertos
			file << deadEnemyIDs.size() << " ";
			for (int id : deadEnemyIDs) file << id << " ";

			file.close();
			LOG("GAME SAVED SUCCESSFULLY with Items and Enemies!");
		}
	}
}

void Scene::LoadGame() {
	std::ifstream file("savegame.txt");
	if (file.is_open()) {
		int sceneInt;
		float pX, pY;

		// 1. Leer datos básicos
		file >> sceneInt >> pX >> pY >> savedScore >> savedAmmo;
		savedPlayerPos = Vector2D(pX, pY);

		// 2. Limpiar listas actuales para evitar duplicados
		collectedIDs.clear();
		deadEnemyIDs.clear();

		// 3. Leer Items recogidos
		int collectedCount = 0;
		file >> collectedCount;
		for (int i = 0; i < collectedCount; i++) {
			int id;
			file >> id;
			collectedIDs.push_back(id);
		}

		// 4. Leer Enemigos muertos
		int deadCount = 0;
		file >> deadCount;
		for (int i = 0; i < deadCount; i++) {
			int id; file >> id;
			deadEnemyIDs.push_back(id);
		}

		file.close();

		loadFromSave = true;
		LOG("GAME LOADED! Enemies and Items synced.");
		ChangeScene((SceneID)sceneInt);
	}
}

