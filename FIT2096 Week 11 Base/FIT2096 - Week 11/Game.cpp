/*	FIT2096 - Example Code
*	Game.cpp
*	Created by Elliott Wilson & Mike Yeates - 2016 - Monash University
*/

#include "Game.h"
#include "TexturedShader.h"
#include "StaticObject.h"
#include "ThirdPersonCamera.h"
#include "FlyingCamera.h"
#include "MathsHelper.h"
#include "DirectXTK/CommonStates.h"
#include <string>
#include <stdio.h>
#define MAX 100 

#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>

typedef uint32_t tDWORD;

wchar_t* Game::Convert1(int data)
{
	wchar_t *result = new wchar_t[10];
	swprintf(result, 10, L"%d", data);
	wprintf(L"[%ls]\n", result);
	return result;
}

Game::Game()
{
	m_renderer = NULL;
	m_audio = NULL;
	m_currentCam = NULL;
	m_input = NULL;
	m_meshManager = NULL;
	m_textureManager = NULL;
	m_stateMachine = NULL;
	m_sceneLighting = NULL;
	m_diffuseTexturedShader = NULL;
	m_unlitVertexColouredShader = NULL;
	m_spriteBatch = NULL;
	m_arialFont12 = NULL;
	m_arialFont18 = NULL;
	m_startButton = NULL;
	m_quitButton = NULL;
	m_player = NULL;
}

Game::~Game() {}

bool Game::Initialise(Direct3D* renderer, AudioSystem* audio, InputController* input)
{
	m_renderer = renderer;	
	m_audio = audio;
	m_input = input;
	m_meshManager = new MeshManager();
	m_textureManager = new TextureManager();
	InitLighting();
	
	if (!InitShaders())
		return false;

	if (!LoadMeshes())
		return false;

	if (!LoadTextures())
		return false;

	if (!LoadAudio())
		return false;

	LoadFonts();
	InitUI();
	InitGameWorld();
	InitStates();

	m_collisionManager = new CollisionManager(&m_agents, &m_bullets, &m_shockwave, &m_hellbombs, &m_explosions, m_playerBase);
	//m_currentCam = new ThirdPersonCamera(m_player, Vector3(0, 10, -25), true, 2.0f);
	//m_currentCam = new FlyingCamera(m_input, Vector3(0.0f, 10.0f, -50.0f));
	m_currentCam = new Camera(Vector3(-1.0f, 20.0f, -40.0f), Vector3(-1.0f, 0.0f, -2.5f), Vector3::Backward, 1280.0f / 720.0f, (float)ToRadians(45.0f), 0.1f, 100.0f);

	return true;
}

void Game::InitLighting()
{
	m_sceneLighting = new SceneLighting(Vector3(0.5, -0.5, 0.5), // Light direction
										Color(0.9f, 0.8f, 0.5f, 1.0f), // Light colour
										Color(0.4f, 0.4f, 0.4f, 1.0f), // Ambient colour
										Color(0.8f, 1.0f, 0.9f, 1.0f), // Fog colour
										10.0f, // Fog start distance
										600.0f); // Fog end distance
}

bool Game::InitShaders()
{
	m_unlitVertexColouredShader = new Shader();
	if (!m_unlitVertexColouredShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/UnlitVertexColourPixelShader.ps"))
		return false;

	m_vertexColouredPixelShader = new Shader();
	if (!m_vertexColouredPixelShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/VertexColourPixelShader.ps"))
		return false;

	m_screenSpaceColourPixelShader = new Shader();
	if (!m_screenSpaceColourPixelShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/ScreenSpaceColourPixelShader.ps"))
		return false;

	m_texturedPixelShader = new TexturedShader();
	if (!m_texturedPixelShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/TexturedPixelShader.ps"))
		return false;

	m_unlitTexturedShader = new TexturedShader();
	if (!m_unlitTexturedShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/UnlitTexturedPixelShader.ps"))
		return false;

	m_diffuseTexturedShader = new TexturedShader(m_sceneLighting);
	if (!m_diffuseTexturedShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/DiffuseTexturedPixelShader.ps"))
		return false;

	m_diffuseTexturedFogShader = new TexturedShader(m_sceneLighting);
	if (!m_diffuseTexturedFogShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/DiffuseTexturedFogPixelShader.ps"))
		return false;

	m_cameraShader = new TexturedShader(m_sceneLighting);
	if (!m_cameraShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/CameraShader.ps"))
		return false;


	return true;
}

bool Game::LoadMeshes()
{
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/kart.obj"))
		return false;

	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/ground.obj"))
		return false;

	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/wall.obj"))
		return false;

	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/rumble_strip.obj"))
		return false;

	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/item_box.obj"))
		return false;

	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/enemy.obj"))
		return false;

	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/ruby.obj"))
		return false;
	
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/bullet.obj"))
		return false;

	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/progress_cube.obj"))
		return false;

	//Obtained from https://github.com/SaschaWillems/Vulkan/blob/master/data/models/sphere.obj
	//File was modified in Brackets to make it readable by the engine
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/sphere.obj"))
		return false;

	//Created in Microsoft 3D Paint, imported and exported through Microsoft 3D Builder
	//to add texture coordinates and convert to .obj file, inported and exported through Blender to add vertex normals,
	//then file was modified in Brackets to make the file readable by the engine
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/lightning1.obj"))
		return false;

	//Created in Microsoft 3D Paint, imported and exported through Microsoft 3D Builder
	//to add texture coordinates and convert to .obj file, inported and exported through Blender to add vertex normals,
	//then file was modified in Brackets to make the file readable by the engine
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/lightning2.obj"))
		return false;

	//Created in Microsoft 3D Paint, imported and exported through Microsoft 3D Builder
	//to add texture coordinates and convert to .obj file, inported and exported through Blender to add vertex normals,
	//then file was modified in Brackets to make the file readable by the engine
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/lightning3.obj"))
		return false;

	//Created in Microsoft 3D Paint, imported and exported through Microsoft 3D Builder
	//to add texture coordinates and convert to .obj file, inported and exported through Blender to add vertex normals,
	//then file was modified in Brackets to make the file readable by the engine
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/lightning4.obj"))
		return false;

	return true;
}

bool Game::LoadTextures()
{
	if (!m_textureManager->Load(m_renderer, "Assets/Textures/kart_red.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/grass.jpg"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/rumble_strip.jpg"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/wall.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/item_box.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/grey_button.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/dark_grey_button.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/blue_button.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/green_button_long.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/red_button_long.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/grey_button_long.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/dark_grey_button_long.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/ground_green.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/ground_beige.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/ground_violet.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_blue.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_green.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_yellow.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_orange.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_red.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/mellow_blue.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/lightning_blue.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/fire_orange.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/black.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/grey.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/green.png"))
		return false;

	//Drawn in Microsoft Paint, with the background made transparent through GIMP
	if (!m_textureManager->Load(m_renderer, "Assets/Textures/lightning_effect_level_1_1.png"))
		return false;

	//Drawn in Microsoft Paint, with the background made transparent through GIMP
	if (!m_textureManager->Load(m_renderer, "Assets/Textures/lightning_effect_level_1_2.png"))
		return false;

	//Drawn in Microsoft Paint, with the background made transparent through GIMP
	if (!m_textureManager->Load(m_renderer, "Assets/Textures/lightning_effect_level_1_3.png"))
		return false;

	//Drawn in Microsoft Paint, with the background made transparent through GIMP
	if (!m_textureManager->Load(m_renderer, "Assets/Textures/lightning_effect_level_1_4.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/lightning_effect_level_2_1.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/lightning_effect_level_2_2.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/lightning_effect_level_2_3.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/lightning_effect_level_2_4.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/lightning_effect_level_3_1.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/lightning_effect_level_3_2.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/lightning_effect_level_3_3.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/lightning_effect_level_3_4.png"))
		return false;

	return true;
}

bool Game::LoadAudio()
{
	if (!m_audio->Load("Assets/Sounds/drumloop.wav"))
		return false;

	if (!m_audio->Load("Assets/Sounds/swish.wav"))
		return false;

	//Obtained from http://soundbible.com/538-Blast.html
	if (!m_audio->Load("Assets/Sounds/explosion.mp3"))
		return false;

	//Obtained from http://soundbible.com/2121-380-Gunshot.html
	if (!m_audio->Load("Assets/Sounds/gunshot.mp3"))
		return false;

	//Obtained from https://www.freesoundeffects.com/free-track/electriccurrent-426765/
	if (!m_audio->Load("Assets/Sounds/electricity.mp3"))
		return false;
	
	return true;
}

void Game::LoadFonts()
{
	// There's a few different size fonts in there, you know
	m_arialFont12 = new SpriteFont(m_renderer->GetDevice(), L"Assets/Fonts/Arial-12pt.spritefont");
	m_arialFont18 = new SpriteFont(m_renderer->GetDevice(), L"Assets/Fonts/Arial-18pt.spritefont");
}

void Game::InitUI()
{
	m_spriteBatch = new SpriteBatch(m_renderer->GetDeviceContext());
	m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_1_1.png");
	temptexture1 = m_textureManager->GetTexture("Assets/Textures/black.png");
	temptexture2 = m_textureManager->GetTexture("Assets/Textures/black.png");
	temptexture3 = m_textureManager->GetTexture("Assets/Textures/black.png");
	temptexture4 = m_textureManager->GetTexture("Assets/Textures/black.png");

	Texture* greyButtonTexture = m_textureManager->GetTexture("Assets/Textures/grey_button.png");
	Texture* greyButtonLongTexture = m_textureManager->GetTexture("Assets/Textures/grey_button_long.png");

	darkGreyButtonTexture = m_textureManager->GetTexture("Assets/Textures/dark_grey_button.png");
	blueButtonTexture = m_textureManager->GetTexture("Assets/Textures/blue_button.png");
	redButtonTexture = m_textureManager->GetTexture("Assets/Textures/red_button_long.png");
	greenButtonTexture = m_textureManager->GetTexture("Assets/Textures/green_button_long.png");
	darkGreyButtonLongTexture = m_textureManager->GetTexture("Assets/Textures/dark_grey_button_long.png");
	
	Vector3 m_displacement = ((Vector3(-1.0f, 0.0f, -2.5f) - Vector3(-1.0f, 20.0f, -40.0f)) / 20);

	m_finisherBarLevel1 = new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ground.obj"), m_cameraShader, m_textureManager->GetTexture("Assets/Textures/mellow_blue.png"), m_displacement + Vector3(-0.7271f, 20.15f, -41.0f), 0, 0.00001, 0.00001);
	m_finisherBarLevel1->SetXRotation((float)ToRadians(298.0724869358f));
	m_finisherBarLevel2 = new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ground.obj"), m_cameraShader, m_textureManager->GetTexture("Assets/Textures/mellow_blue.png"), m_displacement + Vector3(-0.551f, 20.15f, -41.0f), 0, 0.00001, 0.00001);
	m_finisherBarLevel2->SetXRotation((float)ToRadians(298.0724869358f));
	m_finisherBarLevel3 = new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ground.obj"), m_cameraShader, m_textureManager->GetTexture("Assets/Textures/mellow_blue.png"), m_displacement + Vector3(-0.3749f, 20.15f, -41.0f), 0, 0.00001, 0.00001);
	m_finisherBarLevel3->SetXRotation((float)ToRadians(298.0724869358f));
	
	towerPrice = 500;
	towerPriceString = L"Buy Tower: Cost $500";

	// Also init any buttons here
	m_startButton = new Button(128, 64, greyButtonTexture, L"Start Game", Vector2(574, 385), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		// Transition into the Gameplay state (these buttons are only used on the menu screen)
		m_stateMachine->ChangeState(GameStates::GAMEPLAY_STATE);
	});

	m_quitButton = new Button(128, 64, greyButtonTexture, L"Quit", Vector2(706, 385), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		// Tell windows to send a WM_QUIT message into our message pump
		PostQuitMessage(0);
	});

	m_quitToMainMenuButton = new Button(256, 64, greyButtonLongTexture, L"Quit To Main Menu", Vector2(640, 360), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		Reset();
		m_stateMachine->ChangeState(GameStates::MENU_STATE);
	});


	m_retryButton = new Button(256, 64, greyButtonLongTexture, L"Retry", Vector2(640, 428), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		Reset();
		m_stateMachine->ChangeState(GameStates::GAMEPLAY_STATE);
	});

	m_InstaKillBulletButton = new Button(128, 64, darkGreyButtonTexture, L"InstaKill Bullet", Vector2(906, 680), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		
	});

	m_ShockwaveButton = new Button(128, 64, darkGreyButtonTexture, L"Shockwave", Vector2(1038, 680), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		
	});

	m_HellbombButton = new Button(128, 64, darkGreyButtonTexture, L"Hellbomb", Vector2(1170, 680), m_spriteBatch, m_arialFont12, m_input, [this]
	{

	});

	m_BuyTowerButton = new Button(262, 64, redButtonTexture, towerPriceString, Vector2(139, 240), m_spriteBatch, m_arialFont12, m_input, [this]
	{

	});

	m_UpgradeDamageButton = new Button(262, 64, redButtonTexture, L"Upgrade Damage: Costs $500", Vector2(139, 612), m_spriteBatch, m_arialFont12, m_input, [this]
	{

	});

	m_UpgradeDamageButtonCreated = true;

	m_UpgradeFireRateButton = new Button(262, 64, redButtonTexture, L"Upgrade Fire Rate: Costs $400", Vector2(139, 680), m_spriteBatch, m_arialFont12, m_input, [this]
	{

	});

	m_UpgradeFireRateButtonCreated = true;

	m_UpgradeRangeButton = new Button(262, 64, redButtonTexture, L"Upgrade Range: Costs $1000", Vector2(405, 612), m_spriteBatch, m_arialFont12, m_input, [this]
	{

	});

	m_UpgradeRangeButtonCreated = true;

	m_UpgradeBulletSpeedButton = new Button(262, 64, redButtonTexture, L"Upgrade Bullet Speed: Costs $200", Vector2(405, 680), m_spriteBatch, m_arialFont12, m_input, [this]
	{

	});

	m_UpgradeBulletSpeedButtonCreated = true;
}

void Game::InitStates()
{
	// Our state machine needs to know its owner (so it only runs the callbacks while its owner exists)
	m_stateMachine = new StateMachine<GameStates, Game>(this, GameStates::MENU_STATE);

	// Let's match some states with with their OnEnter, OnUpdate, OnRender, OnExit callbacks.
	// Have a look in StateMachine.h to see how this works internally.
	m_stateMachine->RegisterState(GameStates::MENU_STATE, &Game::Menu_OnEnter,
		&Game::Menu_OnUpdate, &Game::Menu_OnRender, &Game::Menu_OnExit);

	m_stateMachine->RegisterState(GameStates::GAMEPLAY_STATE, &Game::Gameplay_OnEnter,
		&Game::Gameplay_OnUpdate, &Game::Gameplay_OnRender, &Game::Gameplay_OnExit);

	m_stateMachine->RegisterState(GameStates::PAUSE_STATE, &Game::Pause_OnEnter,
		&Game::Pause_OnUpdate, &Game::Pause_OnRender, &Game::Pause_OnExit);
	
	m_stateMachine->RegisterState(GameStates::GAMEOVER_STATE, &Game::Gameover_OnEnter,
		&Game::Gameover_OnUpdate, &Game::Gameover_OnRender, &Game::Gameover_OnExit);

}

void Game::InitGameWorld()
{
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(-12.6f, 0, 11.8f), 2.0));
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(-11.6f, 0, 5.4f), 2.0));
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(-8.2f, 0, 8.4f), 2.0));
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(-3.8f, 0, 7.0f), 2.0));
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(0.4f, 0, 4.4f), 2.0));
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(5.4f, 0, 12.6f), 2.0));
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(10.2f, 0, 8.4f), 2.0));
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(13.4f, 0, 9.6f), 2.0));
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(-13.4f, 0, -4.2f), 2.0));
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(-7.8f, 0, -2.8f), 2.0));
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(-4.8f, 0, 0.4f), 2.0));
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(-2.6f, 0, -4.0f), 2.0));
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(2.4f, 0, -6.2f), 2.0));
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(5.4f, 0, -11.0f), 2.0));
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(10.2f, 0, -13.2f), 2.0));
	m_waypoints.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ruby.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(11.8f, 0, -8.2f), 2.0));
	
	m_towers.push_back(new Tower(m_meshManager->GetMesh("Assets/Meshes/item_box.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/grey.png"), Vector3(-5.0f, 0.0f, -1.2f), 0.25, m_towerFireRate, m_towerRange, &m_agents, m_audio));
	m_towers.push_back(new Tower(m_meshManager->GetMesh("Assets/Meshes/item_box.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/grey.png"), Vector3(0.5f, 0.0f, 5.2f), 0.25, m_towerFireRate, m_towerRange, &m_agents, m_audio));
	m_playerBase = new PlayerBase(m_meshManager->GetMesh("Assets/Meshes/item_box.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/grey.png"), Vector3(17.0f, 0.0f, 0.0f), 1.0f);
	m_playerBase->GenerateHealthBar(m_meshManager->GetMesh("Assets/Meshes/progress_cube.obj"), m_diffuseTexturedShader, m_textureManager->GetTexture("Assets/Textures/green.png"));
	
	//The ground (Spawn area is green, centre is beige, and goal is purple)
	m_ground.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ground.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/ground_green.png"), Vector3(-17.31923322f, 0, 0.0f), 0.01f, 1.0f, 0.05f));
	m_ground.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ground.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/ground_violet.png"), Vector3(17.31923322f, 0, 0.0f), 0.01f, 1.0f, 0.05f));
	m_ground.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ground.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/ground_beige.png"), Vector3(0.0f, 0, 0.0f), 0.05f, 1.0f, 0.05f));
	

	/*InitItemBoxes();

	/*m_player = new Kart(m_meshManager->GetMesh("Assets/Meshes/kart.obj"),
		m_diffuseTexturedFogShader,
		m_textureManager->GetTexture("Assets/Textures/kart_red.png"),
		Vector3(0, 0, -10),
		m_input, m_audio);

	m_gameObjects.push_back(m_player);
	m_karts.push_back(m_player);

	// Static scenery objects
	/*m_gameObjects.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ground.obj"),
		m_diffuseTexturedFogShader,
		m_textureManager->GetTexture("Assets/Textures/grass.jpg")));

	m_gameObjects.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/wall.obj"),
		m_diffuseTexturedFogShader,
		m_textureManager->GetTexture("Assets/Textures/wall.png")));

	m_gameObjects.push_back(new StaticObject(m_meshManager->GetMesh("Assets/Meshes/rumble_strip.obj"),
		m_diffuseTexturedFogShader,
		m_textureManager->GetTexture("Assets/Textures/rumble_strip.jpg")));
	//*/
}

void Game::InitItemBoxes()
{
	/*for (int i = 0; i < 20; i++)
	{
		Vector3 position = Vector3(MathsHelper::RandomRange(-200.0f, 200.0f), 0.0f, MathsHelper::RandomRange(-200.0f, 200.0f));

		ItemBox* itemBox = new ItemBox(m_meshManager->GetMesh("Assets/Meshes/item_box.obj"),
			m_diffuseTexturedFogShader,
			m_textureManager->GetTexture("Assets/Textures/item_box.png"),
			position);
		
		m_itemBoxes.push_back(itemBox);
		m_gameObjects.push_back(itemBox);
	}*/
}

void Game::Update(float timestep)
{
	m_input->BeginUpdate();

	// Assuming audio will be needed across multiple states
	m_audio->Update();

	// The state machine knows what state we're in, so things are nice and simple out here
	m_stateMachine->Update(timestep);

	// If something needs to be done in all states, put it in here.
	// We're only updating the camera in the Gameplay state as that's
	// the only state where it can move.

	m_input->EndUpdate();
}

void Game::Render()
{
	m_renderer->BeginScene(1.0f, 0.3f, 0.3f, 1.0f);

	// Render whatever state we're in
	m_stateMachine->Render();

	m_renderer->EndScene();		
}

void Game::Reset()
{
	m_BuyTowerAvailable = false;
	m_placingTower = false;
	m_UpgradeDamageAvailable = false;
	m_UpgradeDamageLevel1 = false;
	m_UpgradeDamageLevel2 = false;
	m_UpgradeDamageLevel3 = false;
	m_UpgradeFireRateAvailable = false;
	m_UpgradeFireRateLevel1 = false;
	m_UpgradeFireRateLevel2 = false;
	m_UpgradeFireRateLevel3 = false;
	m_UpgradeRangeAvailable = false;
	m_UpgradeRangeLevel1 = false;
	m_UpgradeRangeLevel2 = false;
	m_UpgradeRangeLevel3 = false;
	m_UpgradeBulletSpeedAvailable = false;
	m_UpgradeBulletSpeedLevel1 = false;
	m_UpgradeBulletSpeedLevel2 = false;
	m_UpgradeBulletSpeedLevel3 = false;
	m_UpgradeDamageButtonCreated = false;
	m_UpgradeFireRateButtonCreated = false;
	m_UpgradeRangeButtonCreated = false;
	m_UpgradeBulletSpeedButtonCreated = false;
	m_InstaKill = false;
	m_InstaKillCounter = 0;
	m_Hellbomb = false;
	m_HellbombSpawnRate = 0;
	m_HellbombCounter = 0;
	m_finisherBarLevel1Available = false;
	m_finisherBarLevel2Available = false;
	m_finisherBarLevel3Available = false;

	for (unsigned int i = 0; i < m_ground.size(); i++)
	{
		delete m_ground[i];
	}
	m_ground.clear();

	for (unsigned int i = 0; i < m_waypoints.size(); i++)
	{
		delete m_waypoints[i];
	}
	m_waypoints.clear();

	for (unsigned int i = 0; i < m_towers.size(); i++)
	{
		delete m_towers[i];
	}
	m_towers.clear();

	for (unsigned int i = 0; i < m_shockwave.size(); i++)
	{
		delete m_shockwave[i];
	}
	m_shockwave.clear();

	for (unsigned int i = 0; i < m_hellbombs.size(); i++)
	{
		delete m_hellbombs[i];
	}
	m_hellbombs.clear();

	for (unsigned int i = 0; i < m_explosions.size(); i++)
	{
		delete m_explosions[i];
	}
	m_explosions.clear();

	spawnCounter = 0;
	spawnRate = 5.0f;
	lightningCounter = 0;
	m_agentDamage = 0.365f;
	m_towerFireRate = 1.0f;
	m_towerRange = 4.0f;
	m_bulletSpeed = 5.0f;
	armourScale = 1.0f;
	shockwaveCounter = 0;
	m_money = 0;
	roundTimer = 0;
	roundNotification = NULL;
	towerPrice;
	currentPrice;
	towerPriceString;
	inflationChange = false;
	displayRound = false;
	displayRoundCounter = 0;
	finalRound = false;
	finalRoundTimer = 0;
	isPlayerKO = false;
	explosionIndex = 0;

	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		delete m_gameObjects[i];
	}
	m_gameObjects.clear();

	for (unsigned int i = 0; i < m_agents.size(); i++)
	{
		delete m_agents[i];
	}
	m_agents.clear();

	for (unsigned int i = 0; i < m_bullets.size(); i++)
	{
		delete m_bullets[i];
	}
	m_bullets.clear();

	m_playerBase = NULL;

	InitUI();
	InitGameWorld();

}

void Game::Menu_OnEnter()
{
	OutputDebugString("Menu OnEnter\n");
}

void Game::Menu_OnUpdate(float timestep)
{
	// Button's need to update so they can check if the mouse is over them (they swap to a hover section of the image)
	m_startButton->Update();
	m_quitButton->Update();
}

void Game::Menu_OnRender()
{
	DrawMenuUI();
}

void Game::Menu_OnExit()
{
	OutputDebugString("Menu OnExit\n");
}

void Game::Gameplay_OnEnter()
{
	OutputDebugString("GamePlay OnEnter\n");
}

void Game::Gameplay_OnUpdate(float timestep)
{
	// Update all our gameobjects. What they really are doesn't matter
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->Update(timestep);
	}

	//Round Notifications
	int roundSeconds = roundTimer;
	if (roundSeconds == 0) {
		roundNotification = L"Round 1";
		displayRound = true;
	}
	if (!finalRound) {
		roundTimer += timestep;
		roundSeconds = roundTimer;
		if (roundSeconds == 30) {
			inflationChange = true;
			roundNotification = L"Round 2";
			spawnRate = 4.5;
			displayRound = true;
			armourScale = 0.8;
			for (unsigned int i = 0; i < m_agents.size(); i++) {
				m_agents[i]->SetArmourScale(0.8);
			}
		}
		if (roundSeconds == 60) {
			towerPrice = 1000;
			towerPriceString = L"Buy Tower: Cost $1000";
			inflationChange = true;
			roundNotification = L"Round 3";
			spawnRate = 4.0;
			displayRound = true;
		}
		if (roundSeconds == 90) {
			armourScale = 0.8;
			inflationChange = true;
			roundNotification = L"Round 4";
			spawnRate = 3.5;
			displayRound = true;
			armourScale = 0.6;
			for (unsigned int i = 0; i < m_agents.size(); i++) {
				m_agents[i]->SetArmourScale(0.6);
			}
		}
		if (roundSeconds == 120) {
			towerPrice = 3000;
			towerPriceString = L"Buy Tower: Cost $3000";
			inflationChange = true;
			roundNotification = L"Round 5";
			spawnRate = 3.0;
			displayRound = true;
		}
		if (roundSeconds == 150) {
			inflationChange = true;
			roundNotification = L"Round 6";
			spawnRate = 2.5;
			displayRound = true;
			armourScale = 0.4;
			for (unsigned int i = 0; i < m_agents.size(); i++) {
				m_agents[i]->SetArmourScale(0.4);
			}
		}
		if (roundSeconds == 180) {
			towerPrice = 6000;
			towerPriceString = L"Buy Tower: Cost $6000";
			inflationChange = true;
			roundNotification = L"Round 7";
			spawnRate = 2.0;
			displayRound = true;
		}
		if (roundSeconds == 210) {
			inflationChange = true;
			roundNotification = L"Round 8";
			spawnRate = 1.5;
			displayRound = true;
			armourScale = 0.2;
			for (unsigned int i = 0; i < m_agents.size(); i++) {
				m_agents[i]->SetArmourScale(0.2);
			}
		}
		if (roundSeconds == 240) {
			towerPrice = 10000;
			towerPriceString = L"Buy Tower: Cost $10000";
			inflationChange = true;
			roundNotification = L"Round 9";
			spawnRate = 1.0;
			displayRound = true;
		}
		if (roundSeconds == 270) {
			towerPrice = 20000;
			towerPriceString = L"Buy Tower: Cost $20000";
			inflationChange = true;
			roundNotification = L"Final Round";
			spawnRate = 0.5;
			displayRound = true;
			finalRound = true;
		}
	}

	if (finalRound) {
		finalRoundTimer += timestep;
		if (finalRoundTimer >= 5.0) {
			spawnRate *= 0.99;
			finalRoundTimer = 0;
		}
	}

	//Display round notification
	if (displayRound) {
		displayRoundCounter += timestep;
		if (displayRoundCounter >= 5.0) {
			displayRound = false;
			displayRoundCounter = 0;
		}
	}

	//We spawn an agent every second
	spawnCounter += timestep;
	if (spawnCounter >= spawnRate) {

		//randomly spawn an enum of the two different agents
		Agents agents = Agents(rand() % 4);

		//if he's small, spaen a small agent
		if (agents == Small) {
			m_agents.push_back(new Agent(m_meshManager->GetMesh("Assets/Meshes/enemy.obj"), m_diffuseTexturedShader, m_textureManager->GetTexture("Assets/Textures/gradient_blue.png"), Vector3(((rand() % 51) / 10) - 19.81923322, 0, ((rand() % 281) / 10) - 14), 0.5f, m_agentDamage, m_InstaKill, armourScale, &m_waypoints, m_playerBase));
			m_agents.back()->GenerateHealthBar(m_meshManager->GetMesh("Assets/Meshes/progress_cube.obj"), m_diffuseTexturedShader, m_textureManager->GetTexture("Assets/Textures/green.png"));
		}
		else if (agents == Medium) {
			m_agents.push_back(new Agent(m_meshManager->GetMesh("Assets/Meshes/enemy.obj"), m_diffuseTexturedShader, m_textureManager->GetTexture("Assets/Textures/gradient_green.png"), Vector3(((rand() % 51) / 10) - 19.81923322, 0, ((rand() % 281) / 10) - 14), 1.0f, m_agentDamage, m_InstaKill, armourScale, &m_waypoints, m_playerBase));
			m_agents.back()->GenerateHealthBar(m_meshManager->GetMesh("Assets/Meshes/progress_cube.obj"), m_diffuseTexturedShader, m_textureManager->GetTexture("Assets/Textures/green.png"));
		}

		else if (agents == Large) {
			m_agents.push_back(new Agent(m_meshManager->GetMesh("Assets/Meshes/enemy.obj"), m_diffuseTexturedShader, m_textureManager->GetTexture("Assets/Textures/gradient_yellow.png"), Vector3(((rand() % 51) / 10) - 19.81923322, 0, ((rand() % 281) / 10) - 14), 1.5f, m_agentDamage, m_InstaKill, armourScale, &m_waypoints, m_playerBase));
			m_agents.back()->GenerateHealthBar(m_meshManager->GetMesh("Assets/Meshes/progress_cube.obj"), m_diffuseTexturedShader, m_textureManager->GetTexture("Assets/Textures/green.png"));
		}

		//otherwise spawn a large agent
		else {
			m_agents.push_back(new Agent(m_meshManager->GetMesh("Assets/Meshes/enemy.obj"), m_diffuseTexturedShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(((rand() % 51) / 10) - 19.81923322, 0, ((rand() % 281) / 10) - 14), 2.0f, m_agentDamage, m_InstaKill, armourScale, &m_waypoints, m_playerBase));
			m_agents.back()->GenerateHealthBar(m_meshManager->GetMesh("Assets/Meshes/progress_cube.obj"), m_diffuseTexturedShader, m_textureManager->GetTexture("Assets/Textures/green.png"));
		}

		//and finally reset our counter back to zero
		spawnCounter = 0;
	}

	//InstaKill Bullets
	if (m_InstaKill) {
		m_InstaKillCounter += timestep;
		if (m_InstaKillCounter >= 5.0f) {
			m_InstaKill = false;
			m_InstaKillCounter = 0;
			for (unsigned int i = 0; i < m_agents.size(); i++) {
				m_agents[i]->SetInstaKill(m_InstaKill);
			}
		}
	}

	//Shockwave
	for (unsigned int i = 0; i < m_shockwave.size(); i++) {
		shockwaveCounter += timestep;
		m_shockwave[i]->Update(timestep);
		
		if (shockwaveCounter >= 0.1f && shockwaveCounter < 0.12f) {
			m_shockwave[i]->SetMesh(m_meshManager->GetMesh("Assets/Meshes/lightning2.obj"));
		}
		else if (shockwaveCounter >= 0.2f && shockwaveCounter < 0.22f)
		{
			m_shockwave[i]->SetMesh(m_meshManager->GetMesh("Assets/Meshes/lightning3.obj"));
		}
		else if (shockwaveCounter >= 0.3f && shockwaveCounter < 0.33f) {
			m_shockwave[i]->SetMesh(m_meshManager->GetMesh("Assets/Meshes/lightning4.obj"));
		}
		else if (shockwaveCounter >= 0.4f) {
			m_shockwave[i]->SetMesh(m_meshManager->GetMesh("Assets/Meshes/lightning1.obj"));
			shockwaveCounter = 0;
		}
		if (m_shockwave[i]->GetPosition().x < -20.20577209f) {
			m_shockwave.erase(m_shockwave.begin() + i);
		}
	}

	//Hellbomb
	for (unsigned int i = 0; i < m_explosions.size(); i++) {
		m_explosions[i]->Update(timestep);
		if (m_explosions[i]->GetXScale() >= 0.5) {
			m_explosions.erase(m_explosions.begin() + i);
		}
	}
	for (unsigned int i = 0; i < m_hellbombs.size(); i++) {
		m_hellbombs[i]->Update(timestep);
		if ((m_hellbombs[i]->GetPosition() + m_hellbombs[i]->GetMesh()->GetMin()).y <= 0) {
			m_hellbombs[i]->Explode(m_meshManager->GetMesh("Assets/Meshes/sphere.obj"), m_textureManager->GetTexture("Assets/Textures/fire_orange.png"), &m_explosions, m_audio);
		}
		if (m_hellbombs[i]->GetPosition().y <= 0) {
			m_hellbombs.erase(m_hellbombs.begin() + i);
		}
	}
	if (m_Hellbomb) {
		m_HellbombSpawnRate += timestep;
		m_HellbombCounter += timestep;
		if (m_HellbombSpawnRate >= 0.25) {
			m_hellbombs.push_back(new Hellbomb(m_meshManager->GetMesh("Assets/Meshes/bullet.obj"), m_diffuseTexturedShader, m_textureManager->GetTexture("Assets/Textures/black.png"), Vector3(((rand() % 351) / 10) - 20, 20.0, ((rand() % 301) / 10) - 15)));
			m_HellbombSpawnRate = 0;
		}
		if (m_HellbombCounter >= 5.0) {
			m_Hellbomb = false;
			m_HellbombCounter = 0;
		}
	}

	//Towers shooting
	for (unsigned int i = 0; i < m_towers.size(); i++) {
		m_towers[i]->ShootBullet(timestep, m_meshManager->GetMesh("Assets/Meshes/bullet.obj"), &m_bullets, 1.0, m_bulletSpeed);
	}

	for (unsigned int i = 0; i < m_bullets.size(); i++) {
		m_bullets[i]->Update(timestep);
		if (m_bullets[i]->GetPosition().x > 20.20577209 || m_bullets[i]->GetPosition().x <-20.20577209 || m_bullets[i]->GetPosition().z>14.43269435 || m_bullets[i]->GetPosition().z < -14.43269435) {
			m_bullets.erase(m_bullets.begin() + i);
		}
	}

	//Agents Functions
	for (unsigned int i = 0; i < m_agents.size(); i++)
	{
		//first set our target if we need to, then update our position, rotation and status of our agents
		m_agents[i]->SetTarget();
		m_agents[i]->Update(timestep);

		//if any of our agents has reached the last waypoint within the goal area, we delete the agent
		if (m_agents[i]->GetGoalReachedStatus()) {
			m_agents[i]->AttackBase(timestep);
		}
	}

	// Only interested in collisions during the gameplay state
	m_collisionManager->CheckCollisions();

	//Killing Agents
	for (unsigned int i = 0; i < m_agents.size(); i++) {
		if (m_agents[i]->GetHealthBar().GetXScale() < 0.01f) {
			for (unsigned int j = 0; j < m_bullets.size(); j++) {
				if (CheckCollision(m_agents[i]->GetBounds(), m_bullets[j]->GetBounds())) {
					m_money += 50;
				}
			}
			m_agents.erase(m_agents.begin() + i);
			if (!m_InstaKill && m_shockwave.size() < 1 && m_hellbombs.size() < 1 && m_explosions.size() < 1) {
				if (m_finisherBarLevel1->GetXScale() < 0.000305) {
					m_finisherBarLevel1->SetXScale(m_finisherBarLevel1->GetXScale() + 0.000001525f);
					float xpos = -0.7271f + ((m_finisherBarLevel1->GetXScale()) * 288.653887);
					m_finisherBarLevel1->SetPosition(Vector3(xpos, m_finisherBarLevel1->GetPosition().y, m_finisherBarLevel1->GetPosition().z));
				}//*/
				else if (m_finisherBarLevel2->GetXScale() < 0.000305) {
					m_finisherBarLevel2->SetXScale(m_finisherBarLevel2->GetXScale() + 0.000001525f);
					float xpos = -0.551f + ((m_finisherBarLevel2->GetXScale()) * 288.653887);
					m_finisherBarLevel2->SetPosition(Vector3(xpos, m_finisherBarLevel2->GetPosition().y, m_finisherBarLevel2->GetPosition().z));
				}//*/
				else if (m_finisherBarLevel3->GetXScale() < 0.000305) {
					m_finisherBarLevel3->SetXScale(m_finisherBarLevel3->GetXScale() + 0.000001525f);
					float xpos = -0.3749f + ((m_finisherBarLevel3->GetXScale()) * 288.653887);
					m_finisherBarLevel3->SetPosition(Vector3(xpos, m_finisherBarLevel3->GetPosition().y, m_finisherBarLevel3->GetPosition().z));
				}//*/
			}
		}
	}

	//Deleting bullets
	for (unsigned int i = 0; i < m_bullets.size(); i++) {
		if (m_bullets[i]->getCollided()) {
			m_bullets.erase(m_bullets.begin() + i);
			if (!m_InstaKill) {
				m_money += 20;
			}
			if (m_finisherBarLevel1->GetXScale() < 0.000305) {
				m_finisherBarLevel1->SetXScale(m_finisherBarLevel1->GetXScale() + 0.000001525f);
				float xpos = -0.7271f + ((m_finisherBarLevel1->GetXScale()) * 288.653887);
				m_finisherBarLevel1->SetPosition(Vector3(xpos, m_finisherBarLevel1->GetPosition().y, m_finisherBarLevel1->GetPosition().z));
			}//*/
			else if (m_finisherBarLevel2->GetXScale() < 0.000305) {
				m_finisherBarLevel2->SetXScale(m_finisherBarLevel2->GetXScale() + 0.000001525f);
				float xpos = -0.551f + ((m_finisherBarLevel2->GetXScale()) * 288.653887);
				m_finisherBarLevel2->SetPosition(Vector3(xpos, m_finisherBarLevel2->GetPosition().y, m_finisherBarLevel2->GetPosition().z));
			}//*/
			else if (m_finisherBarLevel3->GetXScale() < 0.000305) {
				m_finisherBarLevel3->SetXScale(m_finisherBarLevel3->GetXScale() + 0.000001525f);
				float xpos = -0.3749f + ((m_finisherBarLevel3->GetXScale()) * 288.653887);
				m_finisherBarLevel3->SetPosition(Vector3(xpos, m_finisherBarLevel3->GetPosition().y, m_finisherBarLevel3->GetPosition().z));
			}//*/
		}
	}
	//Special Bar
	if (m_finisherBarLevel1->GetXScale() >= 0.00030499) {
		m_finisherBarLevel1->SetXScale(0.00030501);
		float xpos = -0.7271f + ((m_finisherBarLevel1->GetXScale()) * 288.653887);
		m_finisherBarLevel1->SetPosition(Vector3(xpos, m_finisherBarLevel1->GetPosition().y, m_finisherBarLevel1->GetPosition().z));
		m_finisherBarLevel1->SetTexture(m_textureManager->GetTexture("Assets/Textures/lightning_blue.png"));
	}
	if (m_finisherBarLevel2->GetXScale() >= 0.00030499) {
		m_finisherBarLevel2->SetXScale(0.00030501);
		float xpos = -0.551f + ((m_finisherBarLevel2->GetXScale()) * 288.653887);
		m_finisherBarLevel2->SetPosition(Vector3(xpos, m_finisherBarLevel2->GetPosition().y, m_finisherBarLevel2->GetPosition().z));
		m_finisherBarLevel2->SetTexture(m_textureManager->GetTexture("Assets/Textures/lightning_blue.png"));
	}
	if (m_finisherBarLevel3->GetXScale() >= 0.00030499) {
		m_finisherBarLevel3->SetXScale(0.00030501);
		float xpos = -0.3749f + ((m_finisherBarLevel3->GetXScale()) * 288.653887);
		m_finisherBarLevel3->SetPosition(Vector3(xpos, m_finisherBarLevel3->GetPosition().y, m_finisherBarLevel3->GetPosition().z));
		m_finisherBarLevel3->SetTexture(m_textureManager->GetTexture("Assets/Textures/lightning_blue.png"));
	}

	//Special Bar Lightning Animations
	if (m_finisherBarLevel3->GetXScale() >= 0.00030499) {
		lightningCounter += timestep;
		if (lightningCounter >= 0.1f && lightningCounter < 0.12f) {
			m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_3_2.png");
		}
		else if (lightningCounter >= 0.2f && lightningCounter < 0.22f) {
			m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_3_3.png");
		}
		else if (lightningCounter >= 0.3f && lightningCounter < 0.33f) {
			m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_3_4.png");
		}
		else if (lightningCounter >= 0.4f) {
			m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_3_1.png");
			lightningCounter = 0;
		}
	}
	else if (m_finisherBarLevel2->GetXScale() >= 0.00030499) {
		lightningCounter += timestep;
		if (lightningCounter >= 0.1f && lightningCounter < 0.12f) {
			m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_2_2.png");
		}
		else if (lightningCounter >= 0.2f && lightningCounter < 0.22f) {
			m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_2_3.png");
		}
		else if (lightningCounter >= 0.3f && lightningCounter < 0.33f) {
			m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_2_4.png");
		}
		else if (lightningCounter >= 0.4f) {
			m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_2_1.png");
			lightningCounter = 0;
		}
		
	}
	else if (m_finisherBarLevel1->GetXScale() >= 0.00030499) {
		lightningCounter += timestep;
		if (lightningCounter >= 0.1f && lightningCounter < 0.12f) {
			m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_1_2.png");
		}
		else if (lightningCounter >= 0.2f && lightningCounter < 0.22f) {
			m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_1_3.png");
		}
		else if (lightningCounter >= 0.3f && lightningCounter < 0.33f) {
			m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_1_4.png");
		}
		else if (lightningCounter >= 0.4f) {
			m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_1_1.png");
			lightningCounter = 0;
		}
	}
	//Level 1 Special
	if (!m_finisherBarLevel1Available) {
		if (m_finisherBarLevel1->GetXScale() >= 0.00030499) {
			m_finisherBarLevel1Available = true;
			m_InstaKillBulletButton = new Button(128, 64, blueButtonTexture, L"InstaKill Bullet", Vector2(906, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				if (m_finisherBarLevel3Available) {
					m_finisherBarLevel3->SetXScale(0);
					m_finisherBarLevel3->SetPosition(Vector3(-0.3749f, m_finisherBarLevel3->GetPosition().y, m_finisherBarLevel3->GetPosition().z));
					m_HellbombButton = new Button(128, 64, darkGreyButtonTexture, L"Hellbomb", Vector2(1170, 680), m_spriteBatch, m_arialFont12, m_input, [this]
					{

					});
					m_finisherBarLevel3Available = false;
					m_finisherBarLevel3->SetTexture(m_textureManager->GetTexture("Assets/Textures/mellow_blue.png"));
					m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_2_1.png");
					lightningCounter = 0;
				}
				else if (m_finisherBarLevel2Available && !m_finisherBarLevel3Available) {
					m_finisherBarLevel2->SetXScale(m_finisherBarLevel3->GetXScale());
					float xpos = -0.551f + ((m_finisherBarLevel2->GetXScale()) * 288.653887);
					m_finisherBarLevel2->SetPosition(Vector3(xpos, m_finisherBarLevel2->GetPosition().y, m_finisherBarLevel2->GetPosition().z));
					m_finisherBarLevel3->SetXScale(0);
					m_finisherBarLevel3->SetPosition(Vector3(-0.3749f, m_finisherBarLevel3->GetPosition().y, m_finisherBarLevel3->GetPosition().z));
					m_ShockwaveButton = new Button(128, 64, darkGreyButtonTexture, L"Shockwave", Vector2(1038, 680), m_spriteBatch, m_arialFont12, m_input, [this]
					{

					});
					m_finisherBarLevel2Available = false;
					m_finisherBarLevel2->SetTexture(m_textureManager->GetTexture("Assets/Textures/mellow_blue.png"));
					m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_1_1.png");
					lightningCounter = 0;
				}
				else if (m_finisherBarLevel1Available && !m_finisherBarLevel2Available && !m_finisherBarLevel3Available) {
					m_finisherBarLevel1Available = false;
					m_finisherBarLevel1->SetXScale(m_finisherBarLevel2->GetXScale());
					float xpos = -0.7271f + ((m_finisherBarLevel1->GetXScale()) * 288.653887);
					m_finisherBarLevel1->SetPosition(Vector3(xpos, m_finisherBarLevel1->GetPosition().y, m_finisherBarLevel1->GetPosition().z));
					m_finisherBarLevel2->SetXScale(0);
					m_finisherBarLevel2->SetPosition(Vector3(-0.551f, m_finisherBarLevel2->GetPosition().y, m_finisherBarLevel2->GetPosition().z));
					m_InstaKillBulletButton = new Button(128, 64, darkGreyButtonTexture, L"InstaKill Bullet", Vector2(906, 680), m_spriteBatch, m_arialFont12, m_input, [this]
					{

					});
					m_finisherBarLevel1Available = false;
					m_finisherBarLevel1->SetTexture(m_textureManager->GetTexture("Assets/Textures/mellow_blue.png"));
					m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_1_1.png");
					lightningCounter = 0;

				}
				m_InstaKill = true;
				m_InstaKillCounter = 0;
				for (unsigned int i = 0; i < m_agents.size(); i++) {
					m_agents[i]->SetInstaKill(m_InstaKill);
				}
			});
		}
	}

	//Level 2 Special
	if (!m_finisherBarLevel2Available) {
		if (m_finisherBarLevel2->GetXScale() >= 0.00030499) {
			m_finisherBarLevel2Available = true;
			m_ShockwaveButton = new Button(128, 64, blueButtonTexture, L"Shockwave", Vector2(1038, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				if (m_finisherBarLevel3Available) {
					m_finisherBarLevel2->SetXScale(0);
					m_finisherBarLevel2->SetPosition(Vector3(-0.551f, m_finisherBarLevel2->GetPosition().y, m_finisherBarLevel2->GetPosition().z));
					m_finisherBarLevel3->SetXScale(0);
					m_finisherBarLevel3->SetPosition(Vector3(-0.3749f, m_finisherBarLevel3->GetPosition().y, m_finisherBarLevel3->GetPosition().z));
					m_ShockwaveButton = new Button(128, 64, darkGreyButtonTexture, L"Shockwave", Vector2(1038, 680), m_spriteBatch, m_arialFont12, m_input, [this]
					{

					});
					m_HellbombButton = new Button(128, 64, darkGreyButtonTexture, L"Hellbomb", Vector2(1170, 680), m_spriteBatch, m_arialFont12, m_input, [this]
					{

					});
					m_finisherBarLevel2Available = false;
					m_finisherBarLevel2->SetTexture(m_textureManager->GetTexture("Assets/Textures/mellow_blue.png"));
					m_finisherBarLevel3Available = false;
					m_finisherBarLevel3->SetTexture(m_textureManager->GetTexture("Assets/Textures/mellow_blue.png"));
					m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_1_1.png");
					lightningCounter = 0;
					
				}
				else if (m_finisherBarLevel2Available && !m_finisherBarLevel3Available) {
					m_finisherBarLevel1->SetXScale(m_finisherBarLevel3->GetXScale());
					float xpos = -0.7271f + ((m_finisherBarLevel1->GetXScale()) * 288.653887);
					m_finisherBarLevel1->SetPosition(Vector3(xpos, m_finisherBarLevel1->GetPosition().y, m_finisherBarLevel1->GetPosition().z));
					m_finisherBarLevel2->SetXScale(0);
					m_finisherBarLevel2->SetPosition(Vector3(-0.551f, m_finisherBarLevel2->GetPosition().y, m_finisherBarLevel2->GetPosition().z));
					m_finisherBarLevel3->SetXScale(0);
					m_finisherBarLevel3->SetPosition(Vector3(-0.3749f, m_finisherBarLevel3->GetPosition().y, m_finisherBarLevel3->GetPosition().z));
					m_InstaKillBulletButton = new Button(128, 64, darkGreyButtonTexture, L"InstaKill Bullet", Vector2(906, 680), m_spriteBatch, m_arialFont12, m_input, [this]
					{

					});
					m_ShockwaveButton = new Button(128, 64, darkGreyButtonTexture, L"Shockwave", Vector2(1038, 680), m_spriteBatch, m_arialFont12, m_input, [this]
					{

					});
					m_finisherBarLevel1Available = false;
					m_finisherBarLevel1->SetTexture(m_textureManager->GetTexture("Assets/Textures/mellow_blue.png"));
					m_finisherBarLevel2Available = false;
					m_finisherBarLevel2->SetTexture(m_textureManager->GetTexture("Assets/Textures/mellow_blue.png"));
					m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_1_1.png");
					lightningCounter = 0;
				}
				m_shockwave.push_back(new Shockwave(m_meshManager->GetMesh("Assets/Meshes/lightning1.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/lightning_blue.png"), Vector3(15.0, 1.0, 0.0), m_audio));
			});
		}
	}
	//Level 3 Special
	if (!m_finisherBarLevel3Available) {
		if (m_finisherBarLevel3->GetXScale() >= 0.00030499) {
			m_finisherBarLevel3Available = true;
			m_HellbombButton = new Button(128, 64, blueButtonTexture, L"Hellbomb", Vector2(1170, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				m_finisherBarLevel1->SetXScale(0);
				m_finisherBarLevel1->SetPosition(Vector3(-0.7271f, m_finisherBarLevel1->GetPosition().y, m_finisherBarLevel1->GetPosition().z));
				m_finisherBarLevel2->SetXScale(0);
				m_finisherBarLevel2->SetPosition(Vector3(-0.551f, m_finisherBarLevel2->GetPosition().y, m_finisherBarLevel2->GetPosition().z));
				m_finisherBarLevel3->SetXScale(0);
				m_finisherBarLevel3->SetPosition(Vector3(-0.3749f, m_finisherBarLevel3->GetPosition().y, m_finisherBarLevel3->GetPosition().z));

				m_InstaKillBulletButton = new Button(128, 64, darkGreyButtonTexture, L"InstaKill Bullet", Vector2(906, 680), m_spriteBatch, m_arialFont12, m_input, [this]
				{

				});
				m_ShockwaveButton = new Button(128, 64, darkGreyButtonTexture, L"Shockwave", Vector2(1038, 680), m_spriteBatch, m_arialFont12, m_input, [this]
				{

				});
				m_HellbombButton = new Button(128, 64, darkGreyButtonTexture, L"Hellbomb", Vector2(1170, 680), m_spriteBatch, m_arialFont12, m_input, [this]
				{

				});
				m_finisherBarLevel1Available = false;
				m_finisherBarLevel1->SetTexture(m_textureManager->GetTexture("Assets/Textures/mellow_blue.png"));
				m_finisherBarLevel2Available = false;
				m_finisherBarLevel2->SetTexture(m_textureManager->GetTexture("Assets/Textures/mellow_blue.png"));
				m_finisherBarLevel3Available = false;
				m_finisherBarLevel3->SetTexture(m_textureManager->GetTexture("Assets/Textures/mellow_blue.png"));
				m_lightning = m_textureManager->GetTexture("Assets/Textures/lightning_effect_level_1_1.png");
				lightningCounter = 0;
				m_Hellbomb = true;
			});
		}
	}

	//Placing Tower
	if (currentPrice == towerPrice) {
		currentPrice = towerPrice;
	}
	if (m_placingTower) {
		if (m_input->GetMouseUp(LEFT_MOUSE)) {
			float y1 = m_input->GetMouseY();
			float x1 = m_input->GetMouseX();
			float y2 = -0.11188*y1 + 39.82976;
			float y3 = 0.024*(pow(y2, 2)) - 5;
			float y4 = y2 + y3;
			float x2 = (-0.000114435*y4 + 0.0927948)*x1 + (0.07245*y4 - 59.6773);
			float x3 = -0.4157 * x2;
			float x4 = -0.0288*x3*y4;
			float x5 = x2 + x3 + x4;
			if (x5 > -14.0 && x5 < 14.0 && y4 > -14.0 && y4 < 14.0) {
				m_towers.push_back(new Tower(m_meshManager->GetMesh("Assets/Meshes/item_box.obj"), m_texturedPixelShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png"), Vector3(x5, 0.0f, y4), 0.25, m_towerFireRate, m_towerRange, &m_agents, m_audio));
				m_money -= currentPrice;
				currentPrice = towerPrice;
				m_BuyTowerAvailable = false;
				m_placingTower = false;
				if (m_money < towerPrice) {
					m_BuyTowerAvailable = false;
					m_BuyTowerButton = new Button(262, 64, redButtonTexture, towerPriceString, Vector2(139, 240), m_spriteBatch, m_arialFont12, m_input, [this]
					{

					});
				}
			}
		}
	}

	//Buy Tower
	if (!m_BuyTowerAvailable || inflationChange) {
		if (m_money >= towerPrice && !m_placingTower) {
			inflationChange = false;
			m_BuyTowerAvailable = true;
			m_BuyTowerButton = new Button(262, 64, greenButtonTexture, towerPriceString, Vector2(139, 240), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				m_placingTower = true;
			});
		}
	}
	if (m_BuyTowerAvailable || inflationChange) {
		if (m_money < towerPrice) {
			inflationChange = false;
			m_BuyTowerAvailable = false;
			m_BuyTowerButton = new Button(262, 64, redButtonTexture, towerPriceString, Vector2(139, 240), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				
			});
		}
		if (m_money >= towerPrice && m_placingTower) {
			inflationChange = false;
			m_BuyTowerAvailable = false;
			m_BuyTowerButton = new Button(262, 64, darkGreyButtonLongTexture, L"Cancel Tower Placement", Vector2(139, 240), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				m_placingTower = false;
			});
		}
	}

	//Damage Buttons
	if (!m_UpgradeDamageAvailable) {
		if (m_money >= 500 && !m_UpgradeDamageLevel1) {
			m_UpgradeDamageAvailable = true;
			m_UpgradeDamageButton = new Button(262, 64, greenButtonTexture, L"Upgrade Damage: Costs $500", Vector2(139, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				m_money -= 500;
				m_agentDamage = 0.49f;
				for (unsigned int i = 0; i < m_agents.size(); i++) {
					m_agents[i]->SetDamage(0.49f);
				}
				m_UpgradeDamageLevel1 = true;
				m_UpgradeDamageAvailable = false;
				m_UpgradeDamageButtonCreated = false;
			});
		}
		if (m_money >= 1000 && m_UpgradeDamageLevel1 && !m_UpgradeDamageLevel2) {
			m_UpgradeDamageAvailable = true;
			m_UpgradeDamageButton = new Button(262, 64, greenButtonTexture, L"Upgrade Damage: Costs $1000", Vector2(139, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				m_money -= 1000;
				m_agentDamage = 0.7f;
				for (unsigned int i = 0; i < m_agents.size(); i++) {
					m_agents[i]->SetDamage(0.7f);
				}
				m_UpgradeDamageLevel2 = true;
				m_UpgradeDamageAvailable = false;
				m_UpgradeDamageButtonCreated = false;
			});
		}
		else if (m_UpgradeDamageLevel1 && !m_UpgradeDamageLevel2 && !m_UpgradeDamageButtonCreated) {
			m_UpgradeDamageButtonCreated = true;
			m_UpgradeDamageButton = new Button(262, 64, redButtonTexture, L"Upgrade Damage: Costs $1000", Vector2(139, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				
			});
		}
		if (m_money >= 1500 && m_UpgradeDamageLevel1 && m_UpgradeDamageLevel2 && !m_UpgradeDamageLevel3) {
			m_UpgradeDamageAvailable = true;
			m_UpgradeDamageButton = new Button(262, 64, greenButtonTexture, L"Upgrade Damage: Costs $1500", Vector2(139, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				m_money -= 1500;
				m_agentDamage = 0.8f;
				for (unsigned int i = 0; i < m_agents.size(); i++) {
					m_agents[i]->SetDamage(0.8f);
				}
				m_UpgradeDamageLevel3 = true;
				m_UpgradeDamageAvailable = false;
				m_UpgradeDamageButtonCreated = false;
			});
		}
		else if (m_UpgradeDamageLevel1 && m_UpgradeDamageLevel2 && !m_UpgradeDamageLevel3 && !m_UpgradeDamageButtonCreated) {
			m_UpgradeDamageButtonCreated = true;
			m_UpgradeDamageButton = new Button(262, 64, redButtonTexture, L"Upgrade Damage: Costs $1500", Vector2(139, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
		if (m_UpgradeDamageLevel1 && m_UpgradeDamageLevel2 && m_UpgradeDamageLevel3) {
			m_UpgradeDamageAvailable = true;
			m_UpgradeDamageButton = new Button(262, 64, darkGreyButtonLongTexture, L"Damage Maxed Out", Vector2(139, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				
			});
		}
	}
	else {
		if (m_money < 500 && !m_UpgradeDamageLevel1) {
			m_UpgradeDamageAvailable = false;
			m_UpgradeDamageButton = new Button(262, 64, redButtonTexture, L"Upgrade Damage: Costs $500", Vector2(139, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
		if (m_money < 1000 && m_UpgradeDamageLevel1 && !m_UpgradeDamageLevel2) {
			m_UpgradeDamageAvailable = false;
			m_UpgradeDamageButton = new Button(262, 64, redButtonTexture, L"Upgrade Damage: Costs $1000", Vector2(139, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
		if (m_money < 1500 && m_UpgradeDamageLevel1 && m_UpgradeDamageLevel2 && !m_UpgradeDamageLevel3) {
			m_UpgradeDamageAvailable = false;
			m_UpgradeDamageButton = new Button(262, 64, redButtonTexture, L"Upgrade Damage: Costs $1500", Vector2(139, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
	}//*/

	//Fire Rate Buttons
	if (!m_UpgradeFireRateAvailable) {
		if (m_money >= 400 && !m_UpgradeFireRateLevel1) {
			m_UpgradeFireRateAvailable = true;
			
			m_UpgradeFireRateButton = new Button(262, 64, greenButtonTexture, L"Upgrade Fire Rate: Costs $400", Vector2(139, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				m_money -= 400;
				m_towerFireRate = 0.8f;
				for (unsigned int i = 0; i < m_towers.size(); i++) {
					m_towers[i]->SetFireRate(0.8f);
				}
				m_UpgradeFireRateLevel1 = true;
				m_UpgradeFireRateAvailable = false;
				m_UpgradeFireRateButtonCreated = false;
			});
		}
		if (m_money >= 800 && m_UpgradeFireRateLevel1 && !m_UpgradeFireRateLevel2) {
			m_UpgradeFireRateAvailable = true;
			m_UpgradeFireRateButton = new Button(262, 64, greenButtonTexture, L"Upgrade Fire Rate: Costs $800", Vector2(139, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				m_money -= 800;
				m_towerFireRate = 0.6f;
				for (unsigned int i = 0; i < m_towers.size(); i++) {
					m_towers[i]->SetFireRate(0.6f);
				}
				m_UpgradeFireRateLevel2 = true;
				m_UpgradeFireRateAvailable = false;
				m_UpgradeFireRateButtonCreated = false;
			});
		}
		else if (m_UpgradeFireRateLevel1 && !m_UpgradeFireRateLevel2 && !m_UpgradeFireRateButtonCreated) {
			m_UpgradeFireRateButtonCreated = true;
			m_UpgradeFireRateButton = new Button(262, 64, redButtonTexture, L"Upgrade Fire Rate: Costs $800", Vector2(139, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
		if (m_money >= 1600 && m_UpgradeFireRateLevel1 && m_UpgradeFireRateLevel2 && !m_UpgradeFireRateLevel3) {
			m_UpgradeFireRateAvailable = true;
			m_UpgradeFireRateButton = new Button(262, 64, greenButtonTexture, L"Upgrade Fire Rate: Costs $1600", Vector2(139, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				m_money -= 1600;
				m_towerFireRate = 0.4f;
				for (unsigned int i = 0; i < m_towers.size(); i++) {
					m_towers[i]->SetFireRate(0.4f);
				}
				m_UpgradeFireRateLevel3 = true;
				m_UpgradeFireRateAvailable = false;
				m_UpgradeFireRateButtonCreated = false;
			});
		}
		else if (m_UpgradeFireRateLevel1 && m_UpgradeFireRateLevel2 && !m_UpgradeFireRateLevel3 && !m_UpgradeFireRateButtonCreated) {
			m_UpgradeFireRateButtonCreated = true;
			m_UpgradeFireRateButton = new Button(262, 64, redButtonTexture, L"Upgrade Fire Rate: Costs $1600", Vector2(139, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
		if (m_UpgradeFireRateLevel1 && m_UpgradeFireRateLevel2 && m_UpgradeFireRateLevel3) {
			m_UpgradeFireRateAvailable = true;
			m_UpgradeFireRateButton = new Button(262, 64, darkGreyButtonLongTexture, L"Fire Rate Maxed Out", Vector2(139, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
	}
	else {
		if (m_money < 400 && !m_UpgradeFireRateLevel1) {
			m_UpgradeFireRateAvailable = false;
			m_UpgradeFireRateButton = new Button(262, 64, redButtonTexture, L"Upgrade Fire Rate: Costs $400", Vector2(139, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
		if (m_money < 800 && m_UpgradeFireRateLevel1 && !m_UpgradeFireRateLevel2) {
			m_UpgradeFireRateAvailable = false;
			m_UpgradeFireRateButton = new Button(262, 64, redButtonTexture, L"Upgrade Fire Rate: Costs $800", Vector2(139, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
		if (m_money < 1600 && m_UpgradeFireRateLevel1 && m_UpgradeFireRateLevel2 && !m_UpgradeFireRateLevel3) {
			m_UpgradeFireRateAvailable = false;
			m_UpgradeFireRateButton = new Button(262, 64, redButtonTexture, L"Upgrade Fire Rate: Costs $1600", Vector2(139, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
	}//*/

	//Range Buttons
	if (!m_UpgradeRangeAvailable) {
		if (m_money >= 1000 && !m_UpgradeRangeLevel1) {
			m_UpgradeRangeAvailable = true;

			m_UpgradeRangeButton = new Button(262, 64, greenButtonTexture, L"Upgrade Range: Costs $1000", Vector2(405, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				m_money -= 1000;
				m_towerRange = 6.0f;
				for (unsigned int i = 0; i < m_towers.size(); i++) {
					m_towers[i]->SetRange(6.0f);
				}
				m_UpgradeRangeLevel1 = true;
				m_UpgradeRangeAvailable = false;
				m_UpgradeRangeButtonCreated = false;
			});
		}
		if (m_money >= 2000 && m_UpgradeRangeLevel1 && !m_UpgradeRangeLevel2) {
			m_UpgradeRangeAvailable = true;
			m_UpgradeRangeButton = new Button(262, 64, greenButtonTexture, L"Upgrade Range: Costs $2000", Vector2(405, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				m_money -= 2000;
				m_towerRange = 8.0f;
				for (unsigned int i = 0; i < m_towers.size(); i++) {
					m_towers[i]->SetRange(8.0f);
				}
				m_UpgradeRangeLevel2 = true;
				m_UpgradeRangeAvailable = false;
				m_UpgradeRangeButtonCreated = false;
			});
		}
		else if (m_UpgradeRangeLevel1 && !m_UpgradeRangeLevel2 && !m_UpgradeRangeButtonCreated) {
			m_UpgradeRangeButtonCreated = true;
			m_UpgradeRangeButton = new Button(262, 64, redButtonTexture, L"Upgrade Range: Costs $2000", Vector2(405, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
		if (m_money >= 3000 && m_UpgradeRangeLevel1 && m_UpgradeRangeLevel2 && !m_UpgradeRangeLevel3) {
			m_UpgradeRangeAvailable = true;
			m_UpgradeRangeButton = new Button(262, 64, greenButtonTexture, L"Upgrade Range: Costs $3000", Vector2(405, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				m_money -= 3000;
				m_towerRange = 10.0f;
				for (unsigned int i = 0; i < m_towers.size(); i++) {
					m_towers[i]->SetRange(10.0f);
				}
				m_UpgradeRangeLevel3 = true;
				m_UpgradeRangeAvailable = false;
				m_UpgradeRangeButtonCreated = false;
			});
		}
		else if (m_UpgradeRangeLevel1 && m_UpgradeRangeLevel2 && !m_UpgradeRangeLevel3 && !m_UpgradeRangeButtonCreated) {
			m_UpgradeRangeButtonCreated = true;
			m_UpgradeRangeButton = new Button(262, 64, redButtonTexture, L"Upgrade Range: Costs $3000", Vector2(405, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
		if (m_UpgradeRangeLevel1 && m_UpgradeRangeLevel2 && m_UpgradeRangeLevel3) {
			m_UpgradeRangeAvailable = true;
			m_UpgradeRangeButton = new Button(262, 64, darkGreyButtonLongTexture, L"Range Maxed Out", Vector2(405, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
	}
	else {
		if (m_money < 1000 && !m_UpgradeRangeLevel1) {
			m_UpgradeRangeAvailable = false;
			m_UpgradeRangeButton = new Button(262, 64, redButtonTexture, L"Upgrade Range: Costs $1000", Vector2(405, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
		if (m_money < 2000 && m_UpgradeRangeLevel1 && !m_UpgradeRangeLevel2) {
			m_UpgradeRangeAvailable = false;
			m_UpgradeRangeButton = new Button(262, 64, redButtonTexture, L"Upgrade Range: Costs $2000", Vector2(405, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
		if (m_money < 3000 && m_UpgradeRangeLevel1 && m_UpgradeRangeLevel2 && !m_UpgradeRangeLevel3) {
			m_UpgradeRangeAvailable = false;
			m_UpgradeRangeButton = new Button(262, 64, redButtonTexture, L"Upgrade Range: Costs $3000", Vector2(405, 612), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
	}//*/

	//Bullet Speed Buttons
	if (!m_UpgradeBulletSpeedAvailable) {
		if (m_money >= 200 && !m_UpgradeBulletSpeedLevel1) {
			m_UpgradeBulletSpeedAvailable = true;

			m_UpgradeBulletSpeedButton = new Button(262, 64, greenButtonTexture, L"Upgrade Bullet Speed: Costs $200", Vector2(405, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				m_money -= 200;
				m_bulletSpeed = 7.0f;
				m_UpgradeBulletSpeedLevel1 = true;
				m_UpgradeBulletSpeedAvailable = false;
				m_UpgradeBulletSpeedButtonCreated = false;
			});
		}
		if (m_money >= 400 && m_UpgradeBulletSpeedLevel1 && !m_UpgradeBulletSpeedLevel2) {
			m_UpgradeBulletSpeedAvailable = true;
			m_UpgradeBulletSpeedButton = new Button(262, 64, greenButtonTexture, L"Upgrade Bullet Speed: Costs $400", Vector2(405, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				m_money -= 400;
				m_bulletSpeed = 8.5f;
				m_UpgradeBulletSpeedLevel2 = true;
				m_UpgradeBulletSpeedAvailable = false;
				m_UpgradeBulletSpeedButtonCreated = false;
			});
		}
		else if (m_UpgradeBulletSpeedLevel1 && !m_UpgradeBulletSpeedLevel2 && !m_UpgradeBulletSpeedButtonCreated) {
			m_UpgradeBulletSpeedButtonCreated = true;
			m_UpgradeBulletSpeedButton = new Button(262, 64, redButtonTexture, L"Upgrade Bullet Speed: Costs $400", Vector2(405, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
		if (m_money >= 800 && m_UpgradeBulletSpeedLevel1 && m_UpgradeBulletSpeedLevel2 && !m_UpgradeBulletSpeedLevel3) {
			m_UpgradeBulletSpeedAvailable = true;
			m_UpgradeBulletSpeedButton = new Button(262, 64, greenButtonTexture, L"Upgrade Bullet Speed: Costs $800", Vector2(405, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{
				m_money -= 800;
				m_bulletSpeed = 10.0f;
				m_UpgradeBulletSpeedLevel3 = true;
				m_UpgradeBulletSpeedAvailable = false;
				m_UpgradeBulletSpeedButtonCreated = false;
			});
		}
		else if (m_UpgradeBulletSpeedLevel1 && m_UpgradeBulletSpeedLevel2 && !m_UpgradeBulletSpeedLevel3 && !m_UpgradeBulletSpeedButtonCreated) {
			m_UpgradeBulletSpeedButtonCreated = true;
			m_UpgradeBulletSpeedButton = new Button(262, 64, redButtonTexture, L"Upgrade Bullet Speed: Costs $800", Vector2(405, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
		if (m_UpgradeBulletSpeedLevel1 && m_UpgradeBulletSpeedLevel2 && m_UpgradeBulletSpeedLevel3) {
			m_UpgradeBulletSpeedAvailable = true;
			m_UpgradeBulletSpeedButton = new Button(262, 64, darkGreyButtonLongTexture, L"Bullet Speed Maxed Out", Vector2(405, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
	}
	else {
		if (m_money < 200 && !m_UpgradeBulletSpeedLevel1) {
			m_UpgradeBulletSpeedAvailable = false;
			m_UpgradeBulletSpeedButton = new Button(262, 64, redButtonTexture, L"Upgrade Bullet Speed: Costs $200", Vector2(405, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
		if (m_money < 400 && m_UpgradeBulletSpeedLevel1 && !m_UpgradeBulletSpeedLevel2) {
			m_UpgradeBulletSpeedAvailable = false;
			m_UpgradeBulletSpeedButton = new Button(262, 64, redButtonTexture, L"Upgrade Bullet Speed: Costs $400", Vector2(405, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
		if (m_money < 800 && m_UpgradeBulletSpeedLevel1 && m_UpgradeBulletSpeedLevel2 && !m_UpgradeBulletSpeedLevel3) {
			m_UpgradeBulletSpeedAvailable = false;
			m_UpgradeBulletSpeedButton = new Button(262, 64, redButtonTexture, L"Upgrade Bullet Speed: Costs $800", Vector2(405, 680), m_spriteBatch, m_arialFont12, m_input, [this]
			{

			});
		}
	}//*/

	//Update UI
	m_BuyTowerButton->Update();
	m_InstaKillBulletButton->Update();
	m_ShockwaveButton->Update();
	m_HellbombButton->Update();

	m_UpgradeDamageButton->Update();
	m_UpgradeFireRateButton->Update();
	m_UpgradeRangeButton->Update();
	m_UpgradeBulletSpeedButton->Update();

	m_currentCam->Update(timestep);

	// TODO Tell FMOD where the audio listener (camera) is 
	m_audio->SetListener3DAttributes(m_currentCam->GetPosition(), m_currentCam->GetForward(), m_currentCam->GetUp(), m_currentCam->GetVelocity());

	// Should we pause
	if (m_input->GetKeyDown('P'))
	{
		m_stateMachine->ChangeState(GameStates::PAUSE_STATE);
	}

	if (m_playerBase->GetHealthBar().GetXScale() < 0.01 && !isPlayerKO) {
		m_explosions.push_back(new Explosion(m_meshManager->GetMesh("Assets/Meshes/sphere.obj"), m_playerBase->GetShader(), m_textureManager->GetTexture("Assets/Textures/fire_orange.png"), m_playerBase->GetPosition(), m_audio));
		explosionIndex = m_explosions.size() - 1;
		isPlayerKO = true;
	}
	if (isPlayerKO) {
		if (m_explosions[explosionIndex]->GetXScale() >= 0.4) {
			m_stateMachine->ChangeState(GameStates::GAMEOVER_STATE);
		}
	}
}

void Game::Gameplay_OnRender()
{
	// Draw our gameobjects
	for (unsigned int i = 0; i < m_ground.size(); i++)
	{
		m_ground[i]->Render(m_renderer, m_currentCam);
	}
	
	for (unsigned int i = 0; i < m_towers.size(); i++)
	{
		m_towers[i]->Render(m_renderer, m_currentCam);
	}

	m_playerBase->Render(m_renderer, m_currentCam);
	m_playerBase->GetHealthBar().Render(m_renderer, m_currentCam);//*/

	for (unsigned int i = 0; i < m_bullets.size(); i++) {
		m_bullets[i]->Render(m_renderer, m_currentCam);
	}

	for (unsigned int i = 0; i < m_waypoints.size(); i++)
	{
		m_waypoints[i]->Render(m_renderer, m_currentCam);

	}

	for (unsigned int i = 0; i < m_agents.size(); i++) {
		m_agents[i]->Render(m_renderer, m_currentCam);
		m_agents[i]->GetHealthBar().Render(m_renderer, m_currentCam);
	}
	for (unsigned int i = 0; i < m_shockwave.size(); i++) {
		m_shockwave[i]->Render(m_renderer, m_currentCam);
	}

	for (unsigned int i = 0; i < m_hellbombs.size(); i++) {
		m_hellbombs[i]->Render(m_renderer, m_currentCam);
	}

	for (unsigned int i = 0; i < m_explosions.size(); i++) {
		m_explosions[i]->Render(m_renderer, m_currentCam);
	}

	m_finisherBarLevel1->Render(m_renderer, m_currentCam);
	m_finisherBarLevel2->Render(m_renderer, m_currentCam);
	m_finisherBarLevel3->Render(m_renderer, m_currentCam);

	DrawGameUI();
}

void Game::Gameplay_OnExit()
{
	OutputDebugString("GamePlay OnExit\n");
}

void Game::Pause_OnEnter()
{
	OutputDebugString("Pause OnEnter\n");
}

void Game::Pause_OnUpdate(float timestep)
{
	// Check if we should exit pause
	if (m_input->GetKeyDown('P'))
	{
		m_stateMachine->ChangeState(GameStates::GAMEPLAY_STATE);
	}
	m_quitToMainMenuButton->Update();
	m_retryButton->Update();
}

void Game::Pause_OnRender()
{
	// Keep drawing the game when paused (it's not being updated though which is what freezes it)
	Gameplay_OnRender();

	// In addition to the game, draw some UI to say we're paused
	DrawPauseUI();
}

void Game::Pause_OnExit()
{
	OutputDebugString("Pause OnExit\n");
}

void Game::Gameover_OnEnter()
{
	OutputDebugString("GameOver OnEnter\n");
}

void Game::Gameover_OnUpdate(float timestep)
{
	m_quitToMainMenuButton->Update();
	m_retryButton->Update();
}

void Game::Gameover_OnRender()
{
	Gameplay_OnRender();
	DrawGameoverUI();
}

void Game::Gameover_OnExit()
{
	OutputDebugString("GameOver OnExit\n");
}

void Game::DrawMenuUI()
{
	BeginUI();

	m_startButton->Render();
	m_quitButton->Render();

	m_arialFont18->DrawString(m_spriteBatch, L"D.B.Perera's Tower Defence", Vector2(488, 295), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));

	EndUI();
}

void Game::DrawGameUI()
{
	BeginUI();
	wchar_t* money = Convert1(m_money);
	m_arialFont18->DrawString(m_spriteBatch, L"Money: $", Vector2(10, 10), Color(1.0f, 1.0f, 0.0f), 0, Vector2(0, 0));
	m_arialFont18->DrawString(m_spriteBatch, money, Vector2(110, 10), Color(1.0f, 1.0f, 0.0f), 0, Vector2(0, 0));
	m_arialFont18->DrawString(m_spriteBatch, L"Upgrades", Vector2(210, 535), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));
	m_arialFont18->DrawString(m_spriteBatch, L"Special Abilities", Vector2(950, 550), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));

	if (displayRound) {
		m_arialFont18->DrawString(m_spriteBatch, roundNotification, Vector2(600, 50), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));
		if (finalRound) {
			m_arialFont18->DrawString(m_spriteBatch, L"The agents have destabilised the economy! Tower prices are now $20000!", Vector2(230, 70), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));
		}
	}

	m_BuyTowerButton->Render();

	m_InstaKillBulletButton->Render();
	m_ShockwaveButton->Render();
	m_HellbombButton->Render();

	m_UpgradeDamageButton->Render();
	m_UpgradeFireRateButton->Render();
	m_UpgradeRangeButton->Render();
	m_UpgradeBulletSpeedButton->Render();
	
	m_arialFont18->DrawString(m_spriteBatch, L"P to toggle pause", Vector2(540, 680), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));
	if (m_finisherBarLevel1Available) {
		m_spriteBatch->Draw(m_lightning->GetShaderResourceView(), Vector2(842, 600));
	}
	
	EndUI();
}

void Game::DrawPauseUI()
{
	BeginUI();
	m_arialFont18->DrawString(m_spriteBatch, L"Paused", Vector2(605, 10), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));
	m_quitToMainMenuButton->Render();
	m_retryButton->Render();
	EndUI();
}

void Game::DrawGameoverUI()
{
	BeginUI();
	m_arialFont18->DrawString(m_spriteBatch, L"Game Over", Vector2(600, 10), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));
	m_quitToMainMenuButton->Render();
	m_retryButton->Render();
	EndUI();
}

void Game::BeginUI()
{
	// Sprites don't use a shader 
	m_renderer->SetCurrentShader(NULL);

	CommonStates states(m_renderer->GetDevice());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());
}

void Game::EndUI()
{
	m_spriteBatch->End();
}

void Game::Shutdown()
{
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		delete m_gameObjects[i];
	}

	m_gameObjects.clear();

	if (m_currentCam)
	{
		delete m_currentCam;
		m_currentCam = NULL;
	}

	if (m_unlitVertexColouredShader)
	{
		m_unlitVertexColouredShader->Release();
		delete m_unlitVertexColouredShader;
		m_unlitVertexColouredShader = NULL;
	}

	if (m_unlitTexturedShader)
	{
		m_unlitTexturedShader->Release();
		delete m_unlitTexturedShader;
		m_unlitTexturedShader = NULL;
	}

	if(m_diffuseTexturedShader)
	{
		m_diffuseTexturedShader->Release();
		delete m_diffuseTexturedShader;
		m_diffuseTexturedShader = NULL;
	}

	if (m_diffuseTexturedFogShader)
	{
		m_diffuseTexturedFogShader->Release();
		delete m_diffuseTexturedFogShader;
		m_diffuseTexturedFogShader = NULL;
	}

	if (m_meshManager)
	{
		m_meshManager->Release();
		delete m_meshManager;
		m_meshManager = NULL;
	}

	if (m_textureManager)
	{
		m_textureManager->Release();
		delete m_textureManager;
		m_textureManager = NULL;
	}

	if (m_stateMachine)
	{
		delete m_stateMachine;
		m_stateMachine = NULL;
	}

	if (m_spriteBatch)
	{
		delete m_spriteBatch;
		m_spriteBatch = NULL;
	}

	if (m_arialFont12)
	{
		delete m_arialFont12;
		m_arialFont12 = NULL;
	}

	if (m_arialFont18)
	{
		delete m_arialFont18;
		m_arialFont18 = NULL;
	}

	if (m_sceneLighting)
	{
		delete m_sceneLighting;
		m_sceneLighting = NULL;
	}

	if (m_audio)
	{
		m_audio->Shutdown();
		delete m_audio;
		m_audio = NULL;
	}
}