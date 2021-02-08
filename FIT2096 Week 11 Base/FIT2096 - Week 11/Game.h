/*	FIT2096 - Example Code
*	Game.h
*	Created by Elliott Wilson & Mike Yeates - 2016 - Monash University
*	This class is the heart of our game and is also where our game logic will reside
*	It contains the overall Update and Render method for the whole game
*	You should eventually split some game logic into other objects which Game will manage
*	Watch the size of this class - it can grow out of control very fast!
*/

#ifndef GAME_H
#define GAME_H

#include "Direct3D.h"
#include "Camera.h"
#include "InputController.h"
#include "MeshManager.h"
#include "TextureManager.h"
#include "CollisionManager.h"
#include "AudioSystem.h"
#include "SceneLighting.h"
#include "StateMachine.h"
#include "Button.h"
#include "GameObject.h"
#include "StaticObject.h"
#include "MovingObject.h"
#include "Agent.h"
#include "Bullet.h"
#include "Shockwave.h"
#include "Hellbomb.h"
#include "Explosion.h"
#include "Tower.h"
#include "PlayerBase.h"
#include "Kart.h"
#include "ItemBox.h"

#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"

#include <vector>

#include <windows.h>

class Game
{
private:
	// All the states our game can be in. The STATE_COUNT down the bottom is just an easy way
	// for us to count how many states this enum has (cast this to an int)
	enum class GameStates
	{
		MENU_STATE,
		GAMEPLAY_STATE,
		PAUSE_STATE,
		GAMEOVER_STATE,
		STATE_COUNT
	};

	Direct3D* m_renderer;
	AudioSystem* m_audio;
	InputController* m_input;
	MeshManager* m_meshManager;
	TextureManager* m_textureManager;
	CollisionManager* m_collisionManager;
	SceneLighting* m_sceneLighting;
	Camera* m_currentCam;

	// Our state machine is a generic class (we need to tell it what types it manages).
	// It knows about two things - our states, and also who ownes those states.
	StateMachine<GameStates, Game>* m_stateMachine;

	// Sprites / Fonts
	SpriteBatch* m_spriteBatch;
	SpriteFont* m_arialFont12;
	SpriteFont* m_arialFont18;
	Texture* m_lightning;
	Texture* temptexture1;
	Texture* temptexture2;
	Texture* temptexture3;
	Texture* temptexture4;

	// Our menu screen will have a "start" and "quit" button
	Button* m_startButton;
	Button* m_quitButton;

	//Our pause screen will have a "Quit to main menu" button
	Button* m_quitToMainMenuButton;

	//Our pause screen will have a "Quit to main menu" button
	Button* m_retryButton;

	//Buttons for special attacks
	Button* m_InstaKillBulletButton;
	Button* m_ShockwaveButton;
	Button* m_HellbombButton;

	//booleans for special attacks
	bool m_SpecialLevel1 = false;
	bool m_SpecialLevel2 = false;
	bool m_SpecialLevel3 = false;

	//Purchase Tower
	Button* m_BuyTowerButton;
	bool m_BuyTowerAvailable = false;
	bool m_placingTower = false;

	//Buttons to upgrade weapons
	Button* m_UpgradeDamageButton;
	Button* m_UpgradeFireRateButton;
	Button* m_UpgradeRangeButton;
	Button* m_UpgradeBulletSpeedButton;

	Texture* blueButtonTexture;
	Texture* redButtonTexture;
	Texture* greenButtonTexture;
	Texture* darkGreyButtonTexture;
	Texture* darkGreyButtonLongTexture;

	//booleans for upgrades
	bool m_UpgradeDamageAvailable = false;
	bool m_UpgradeDamageLevel1 = false;
	bool m_UpgradeDamageLevel2 = false;
	bool m_UpgradeDamageLevel3 = false;
	bool m_UpgradeFireRateAvailable = false;
	bool m_UpgradeFireRateLevel1 = false;
	bool m_UpgradeFireRateLevel2 = false;
	bool m_UpgradeFireRateLevel3 = false;
	bool m_UpgradeRangeAvailable = false;
	bool m_UpgradeRangeLevel1 = false;
	bool m_UpgradeRangeLevel2 = false;
	bool m_UpgradeRangeLevel3 = false;
	bool m_UpgradeBulletSpeedAvailable = false;
	bool m_UpgradeBulletSpeedLevel1 = false;
	bool m_UpgradeBulletSpeedLevel2 = false;
	bool m_UpgradeBulletSpeedLevel3 = false;

	bool m_UpgradeDamageButtonCreated = false;
	bool m_UpgradeFireRateButtonCreated = false;
	bool m_UpgradeRangeButtonCreated = false;
	bool m_UpgradeBulletSpeedButtonCreated = false;

	//attributes for Special attacks
	bool m_InstaKill = false;
	float m_InstaKillCounter = 0;
	bool m_Hellbomb = false;
	float m_HellbombSpawnRate = 0;
	float m_HellbombCounter = 0;

	//Special Attack Bar
	StaticObject* m_finisherBarLevel1;
	StaticObject* m_finisherBarLevel2;
	StaticObject* m_finisherBarLevel3;
	bool m_finisherBarLevel1Available = false;
	bool m_finisherBarLevel2Available = false;
	bool m_finisherBarLevel3Available = false;

	// Shaders
	Shader* m_unlitVertexColouredShader;
	Shader* m_vertexColouredPixelShader;
	Shader* m_screenSpaceColourPixelShader;
	Shader* m_texturedPixelShader;
	Shader* m_unlitTexturedShader;
	Shader* m_diffuseTexturedShader;
	Shader* m_diffuseTexturedFogShader;
	Shader* m_cameraShader;
	
	Kart* m_player;

	enum Agents { Small, Medium, Large, BIGMOFO };		//agent types
	std::vector<StaticObject*> m_ground;	//vector of ground objects
	std::vector<StaticObject*> m_waypoints;	//vector of waypoints
	std::vector<Tower*> m_towers;
	std::vector<Shockwave*> m_shockwave;
	std::vector<Hellbomb*> m_hellbombs;
	std::vector<Explosion*> m_explosions;
	float spawnCounter = 0;
	float spawnRate = 5.0f;
	float lightningCounter = 0;
	float m_agentDamage = 0.365f;
	float m_towerFireRate = 1.0f;
	float m_towerRange = 4.0f;
	float m_bulletSpeed = 5.0f;
	float armourScale = 1.0f;
	float shockwaveCounter = 0;
	int m_money = 0;
	float roundTimer = 0;
	wchar_t* roundNotification;
	int towerPrice;
	int currentPrice;
	wchar_t* towerPriceString;
	bool inflationChange = false;
	bool displayRound = false;
	float displayRoundCounter = 0;
	bool finalRound = false;
	float finalRoundTimer = 0;
	bool isPlayerKO = false;
	int explosionIndex;

	// This contains everything for easy calls to update and render
	std::vector<GameObject*> m_gameObjects;

	// We also need more specific collections for easier collision checks
	std::vector<Agent*> m_agents;
	std::vector<Bullet*> m_bullets;
	PlayerBase* m_playerBase;

	// Initialisation Helpers
	void InitLighting();
	bool InitShaders();
	bool LoadMeshes();
	bool LoadTextures();
	bool LoadAudio();
	void LoadFonts();
	void InitGameWorld();
	void InitStates();
	void InitItemBoxes();

	// UI drawing helpers
	void InitUI();
	void DrawMenuUI();
	void DrawGameUI();
	void DrawPauseUI();
	void DrawGameoverUI();
	void BeginUI();
	void EndUI();

	// Every state in our game will have four callbacks
	// We register these with the StateMachine and it calls them for us
	void Menu_OnEnter();
	void Menu_OnUpdate(float timestep);
	void Menu_OnRender();
	void Menu_OnExit();

	void Gameplay_OnEnter();
	void Gameplay_OnUpdate(float timestep);
	void Gameplay_OnRender();
	void Gameplay_OnExit();

	void Pause_OnEnter();
	void Pause_OnUpdate(float timestep);
	void Pause_OnRender();
	void Pause_OnExit();

	void Gameover_OnEnter();
	void Gameover_OnUpdate(float timestep);
	void Gameover_OnRender();
	void Gameover_OnExit();

	static wchar_t* Convert1(int data);

public:
	Game();	
	~Game();

	bool Initialise(Direct3D* renderer, AudioSystem* audio, InputController* input); //The initialise method will load all of the content for the game (meshes, textures, etc.)

	void Update(float timestep);	//The overall Update method for the game. All gameplay logic will be done somewhere within this method
	void Render();					//The overall Render method for the game. Here all of the meshes that need to be drawn will be drawn
	void Reset();					//Reset everything to its default state

	void Shutdown(); //Cleanup everything we initialised
};

#endif