#include <iostream>
using namespace std;

#include "MenuState.h"
#include "GL\glew.h"
#include "../Application.h"
#include "LoadTGA.h"
#include "GraphicsManager.h"
#include "MeshBuilder.h"
#include "../TextEntity.h"
#include "RenderHelper.h"
#include "../SpriteEntity.h"
#include "../EntityManager.h"

#include "KeyboardController.h"
#include "SceneManager.h"

CMenuState::CMenuState()
{
}

CMenuState::~CMenuState()
{
}

void CMenuState::Init()
{
	//Create and attach the camera to the scene
	camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	//Init the lua state interface to DM2240.lua
	CLuaInterface::GetInstance()->SetLuaFile("Image//DM2240.lua");
	
	//Load all the meshes
	//Main menu Background
	MeshBuilder::GetInstance()->GenerateQuad("MENUSTATE_BKGROUND", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("MENUSTATE_BKGROUND")->textureID = LoadTGA("Image//MenuState.tga");
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.f;
	MenuStateBackground = Create::Sprite2DObject("MENUSTATE_BKGROUND",
		Vector3(halfWindowWidth, halfWindowHeight, 0.f),
		Vector3(Application::GetInstance().GetWindowWidth(), Application::GetInstance().GetWindowHeight(), 0.f)
	);

	//Buttons
	//Play Game Button
	MeshBuilder::GetInstance()->GenerateQuad("PlayGameButton", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("PlayGameButton")->textureID = LoadTGA("Image//PlayGameButton.tga");

	/*CLuaInterface::GetInstance()->
        Create::Sprite2DObject("PlayGameButton",
		Vector3(halfWindowWidth, halfWindowHeight, 0.f),
		Vector3(Application::GetInstance().GetWindowWidth(), Application::GetInstance().GetWindowHeight(), 0.f)*/
	
			
	////Button Border
	//MeshBuilder::GetInstance()->GenerateQuad("ButtonBorder", Color(1, 1, 1), 1.f);
	//MeshBuilder::GetInstance()->GetMesh("ButtonBorder")->textureID = LoadTGA("Image//buttonborder.tga");
	//MenuStateBackground = Create::Sprite2DObject("ButtonBorder",
	//	Vector3(, , 0.f),
	//	Vector3(Application::GetInstance().GetWindowWidth(), Application::GetInstance().GetWindowHeight(), 0.f)

	cout << "CMenuState Loaded\n";

	buttonState = STATE_PLAY_GAME;
}

void CMenuState::Update(double dt)
{
	if (KeyboardController::GetInstance()->IsKeyReleased('W'))
	{
		buttonState = static_cast<ButtonState>(buttonState - 1);

		if (buttonState < 0)
		{
			buttonState = static_cast<ButtonState>(STATES_TOTAL - 1);
		}
	}
	else if (KeyboardController::GetInstance()->IsKeyReleased('S'))
	{
		buttonState = static_cast<ButtonState>(buttonState + 1);

		if (buttonState >= STATES_TOTAL)
		{
			buttonState = static_cast<ButtonState>(0);
		}
	}

	if (KeyboardController::GetInstance()->IsKeyReleased(VK_RETURN))
	{
		switch (buttonState)
		{
		case STATE_PLAY_GAME:
		{
			cout << "Loading GameState\n";
			SceneManager::GetInstance()->SetActiveScene("GameState");
			break;
		}
		case STATE_OPTIONS:
		{
			cout << "Loading OptionsState\n";
			break;
		}
		case STATE_HIGHSCORES:
		{
			cout << "Loading HighscoreState\n";
			break;
		}
		case STATE_EXIT:
		{
			cout << "Exiting Game...\n";
			Application::GetInstance().Exit();
			break;
		}

		default:
			break;
		}
	}
}

void CMenuState::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup 3D pipeline then render 3D
	GraphicsManager::GetInstance()->SetPerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	//Render the required entities
	EntityManager::GetInstance()->Render();

	// Setup 2D pipeline then render 2D
	GraphicsManager::GetInstance()->SetOrthographicProjection(0, Application::GetInstance().GetWindowWidth(),
		0, Application::GetInstance().GetWindowHeight(),
		-10, 10);
	GraphicsManager::GetInstance()->DetachCamera();

	//Render the required entities
	EntityManager::GetInstance()->RenderUI();
}

void CMenuState::Exit()
{
	//Remove the entity from EntitManager
	EntityManager::GetInstance()->RemoveEntity(MenuStateBackground);

	//Remove the meshes which are specific to CIntroState
	MeshBuilder::GetInstance()->RemoveMesh("MENUSTATE_BKGROUND");

	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();
}
