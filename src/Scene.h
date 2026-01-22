#pragma once

#include "Module.h"
#include "Player.h"
#include "UIButton.h"
#include "Enemy.h"
#include <fstream>
#include <list>
struct SDL_Texture;

// Enum to identify different game scenes
enum class SceneID
{
	MAIN_MENU,
	LEVEL_1,
	LEVEL_2

};
class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	int saveFxId = 0;

	void SaveGame();
	void LoadGame();
	bool loadFromSave = false;   // para saber si estamos cargando partida
	Vector2D savedPlayerPos;     // Posición cargada
	int savedScore = 0;
	int savedAmmo = 0;
	float savedLevelTimer = 0.0f;
	int savedBossHealth = 0;

	// Return the player position
	Vector2D GetPlayerPosition();

	// Get tilePosDebug value
	std::string GetTilePosDebug() {
		return tilePosDebug;
	}
	bool OnUIMouseClickEvent(UIElement* uiElement);

	// L17 TODO 2: Define functions to handle scene changes
	void LoadScene(SceneID newScene);
	void UnloadCurrentScene();
	void ChangeScene(SceneID newScene);
	
	// Game Paused State
	bool isGamePaused = false; 
	
	// Getter to check if the game has ended (for Player control lock)
	bool IsGameOver() const { return isGameOver; }

	// Items collected in the scene
	int bossAttackFxId = 0;
	int loseGameFxId = 0;
	int levelUpFxId = 0;

	// Bandera para asegurar que la música del boss solo empiece una vez
	bool isBossMusicPlaying = false;
	std::list<int> collectedIDs;
	std::list<int> deadEnemyIDs;
	std::shared_ptr<Player> player;
	
	// Reference to the Boss enemy to check health/death status
	std::shared_ptr<Enemy> bossReference = nullptr;
private:
//----------------------------------------------------------------------Scene-Specific Logic Methods---------------------------------------------------------------------------------------------------//
	//main menu
	void LoadMainMenu();
	void UnloadMainMenu();
	void UpdateMainMenu(float dt);
	void HandleMainMenuUIVebets(UIElement* uiLement);
	
	//leve 1
	void LoadLevel1();
	void UnloadLevel1();
	void UpdateLevel1(float dt);
	void PostUpdateLevel1();
	
	// level 2
	void LoadLevel2();
	void UnloadLevel2();
	void UpdateLevel2(float dt);
	void PostUpdateLevel2();

	// Settings UI Management
	void CreateSettingsUI();
	void DestroySettingsUI();
private:

	// UI and Assets
	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";
	bool showHelpMenu = false;
	bool once = false;
	float reloadCooldown = 0.0f;
	SDL_Texture* helpMenuTexture = nullptr;
	
	// L16: TODO 2: Declare a UIButton 
	std::shared_ptr<UIButton>uiBt1;
	std::shared_ptr<UIButton>uiBt2;
	bool showCreditsUI;

	// L17 TODO 1: Current scene attribute with initial value
	bool sceneChangeRequested = false;
	SceneID nextScene = SceneID::MAIN_MENU;
	SceneID currentScene = SceneID::MAIN_MENU;
	SDL_Texture* mainMenuBackground = nullptr;
	
	// timer
	float levelTimer = 0.0f;
	bool isGameOver = false;
	
	// UI Buttons and IDs
	const int RESUME_BTN_ID = 101;
	const int EXIT_BTN_ID = 102;
	bool exitGameRequested = false;
	bool gameOverShown = false;
	const int BTN_SETTINGS_RETURN_MENU = 305;
	
	// Pause Menu Icons
	SDL_Texture* iconPause = nullptr; 
	SDL_Texture* iconPlay = nullptr; 
	const int PAUSE_TOGGLE_ID = 200;
	//------------------------------------
	const int BTN_MAIN_MENU_CONTINUE = 5; // Botón "Continuar" en el menú principal
	const int BTN_SETTINGS_SAVE = 306;    // Botón "Guardar" en el menú de pausa
	//-------------------------------------------
	std::shared_ptr<UIElement> mainMenuContinueBtn;
	
	// Main Menu Buttons
	const int BTN_MAIN_MENU_EXIT = 3;
	std::shared_ptr<UIElement> mainMenuExitBtn;
	std::shared_ptr<UIElement> winButton = nullptr;
	std::shared_ptr<UIElement> mainMenuStartBtn;
	std::shared_ptr<UIElement> mainMenuSettingBtn;
	
	// Settings Menu IDs
	const int BTN_MAIN_MENU_SETTINGS = 2;
	const int SETTINGS_PANEL_BG_ID = 300; 
	const int BTN_SETTINGS_CLOSE = 301;
	const int TOGGLE_FULLSCREEN_ID = 100; 
	const int TOGGLE_MUSIC_ID = 302;
	const int BTN_VOL_PLUS = 303;
	const int BTN_VOL_MINUS = 304;
	const int BTN_SETTINGS_CREDITS = 9;

	// Flag to render settings overlay
	bool showSettingsUI = false;
	bool settingsCloseRequested = false; 


	// Victory State
	bool isGameWon = false;
	SDL_Texture* winScreenTexture = nullptr;
	const int BTN_WIN_EXIT = 400;
	int winFxId = 0;

};