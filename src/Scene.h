#pragma once

#include "Module.h"
#include "Player.h"
#include "UIButton.h"
struct SDL_Texture;

enum class SceneID
{
	INTRO_SCR,
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
	bool isGamePaused = false; // 游戏是否暂停
private:
	// L17 TODO 3: Define specific function for main menu scene: Load, Unload, Handle UI events
	void LoadMainMenu();
	void UnloadMainMenu();
	void UpdateMainMenu(float dt);
	void HandleMainMenuUIVebets(UIElement* uiLement);
	// L17 TODO 4: Define specific functions for level1 scene: Load, Unload, Update, PostUpdate
	void LoadLevel1();
	void UnloadLevel1();
	void UpdateLevel1(float dt);
	void PostUpdateLevel1();
	// L17 TODO 5: Define specific functions for level2 scene: Load, Unload, Update
	void LoadLevel2();
	void UnloadLevel2();
	void UpdateLevel2(float dt);
	void PostUpdateLevel2();
private:

	//L03: TODO 3b: Declare a Player attribute
	std::shared_ptr<Player> player;
	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";
	bool showHelpMenu = false;
	bool once = false;
	float reloadCooldown = 0.0f;
	SDL_Texture* helpMenuTexture = nullptr;
	// L16: TODO 2: Declare a UIButton 
	std::shared_ptr<UIButton>uiBt1;
	std::shared_ptr<UIButton>uiBt2;

	
	// L17 TODO 1: Current scene attribute with initial value
	bool sceneChangeRequested = false;
	SceneID nextScene = SceneID::MAIN_MENU;
	SceneID currentScene = SceneID::MAIN_MENU;
	SDL_Texture* mainMenuBackground = nullptr;
	//timer
	float levelTimer = 0.0f;
	bool isGameOver = false;

	const int RESUME_BTN_ID = 101;
	const int EXIT_BTN_ID = 102;
	bool exitGameRequested = false;
	bool gameOverShown = false;

	
	SDL_Texture* iconPause = nullptr; // 暂停图标 (游戏进行时显示)
	SDL_Texture* iconPlay = nullptr;  // 播放/继续图标 (游戏暂停时显示)
	// 定义一个 ID 给暂停按钮
	const int PAUSE_TOGGLE_ID = 200;
};