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
	
	glDisable(GL_DEPTH_TEST);

	//Init the lua state interface to MenuData.lua
	CLuaInterface::GetInstance()->SetLuaFile("Image//MenuData.lua", CLuaInterface::GetInstance()->theLuaState);

	halfWindowWidth = Application::GetInstance().GetWindowWidth() * .5f;
	halfWindowHeight = Application::GetInstance().GetWindowHeight() * .5f;

	//Load all the meshes
	//Main menu Background
	MeshBuilder::GetInstance()->GenerateQuad("MENUSTATE_BKGROUND", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("MENUSTATE_BKGROUND")->textureID = LoadTGA("Image//MenuState.tga");
	MenuStateBackground = Create::Sprite2DObject("MENUSTATE_BKGROUND",
		Vector3(halfWindowWidth, halfWindowHeight, 0.f),
		Vector3(Application::GetInstance().GetWindowWidth(), Application::GetInstance().GetWindowHeight(), 0.f)
	);

	//Buttons
	//Play Game Button
	MeshBuilder::GetInstance()->GenerateQuad("PlayGameButton", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("PlayGameButton")->textureID = LoadTGA("Image//PlayGameButton.tga");

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "PlayGameButton");
	Create::Sprite2DObject("PlayGameButton",
		Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f),
		Vector3(CLuaInterface::GetInstance()->getFieldFloat("scaleX"), CLuaInterface::GetInstance()->getFieldFloat("scaleY"), 0.f));

	//Button Border
	MeshBuilder::GetInstance()->GenerateQuad("ButtonBorder", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("ButtonBorder")->textureID = LoadTGA("Image//buttonborder.tga");
	ButtonBorder = Create::Sprite2DObject("ButtonBorder",
		Vector3(halfWindowWidth - 10.f, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f),
		Vector3(450, 160, 0.f));

	//Options Button
	MeshBuilder::GetInstance()->GenerateQuad("OptionsButton", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("OptionsButton")->textureID = LoadTGA("Image//OptionsButton.tga");

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "OptionsButton");
	Create::Sprite2DObject("OptionsButton",
		Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f),
		Vector3(CLuaInterface::GetInstance()->getFieldFloat("scaleX"), CLuaInterface::GetInstance()->getFieldFloat("scaleY"), 0.f));

	//Highscore Button
	MeshBuilder::GetInstance()->GenerateQuad("HighscoreButton", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("HighscoreButton")->textureID = LoadTGA("Image//HighscoreButton.tga");

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "HighscoreButton");
	Create::Sprite2DObject("HighscoreButton",
		Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f),
		Vector3(CLuaInterface::GetInstance()->getFieldFloat("scaleX"), CLuaInterface::GetInstance()->getFieldFloat("scaleY"), 0.f));

	//Exit Button
	MeshBuilder::GetInstance()->GenerateQuad("ExitButton", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("ExitButton")->textureID = LoadTGA("Image//ExitButton.tga");

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "ExitButton");
	Create::Sprite2DObject("ExitButton",
		Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f),
		Vector3(CLuaInterface::GetInstance()->getFieldFloat("scaleX"), CLuaInterface::GetInstance()->getFieldFloat("scaleY"), 0.f));
			
	MoveUp = CLuaInterface::GetInstance()->getCharValue("MoveUp");
	MoveDown = CLuaInterface::GetInstance()->getCharValue("MoveDown");

	cout << "CMenuState Loaded\n";

	buttonState = STATE_PLAY_GAME;
}

void CMenuState::Update(double dt)
{
	if (KeyboardController::GetInstance()->IsKeyReleased(MoveUp))
	{
		buttonState = static_cast<ButtonState>(buttonState - 1);

		if (buttonState < 0)
		{
			buttonState = static_cast<ButtonState>(STATES_TOTAL - 1);
		}

		//Set button border position
		switch (buttonState)
		{
		case STATE_PLAY_GAME:
		{
			lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "PlayGameButton");
			ButtonBorder->SetPosition(Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f));
			break;
		}
		case STATE_OPTIONS:
		{
			lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "OptionsButton");
			ButtonBorder->SetPosition(Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f));
			break;
		}
		case STATE_HIGHSCORES:
		{
			lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "HighscoreButton");
			ButtonBorder->SetPosition(Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f));
			break;
		}
		case STATE_EXIT:
		{
			lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "ExitButton");
			ButtonBorder->SetPosition(Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f));
			break;
		}

		default:
			break;
		}
	}

	if (KeyboardController::GetInstance()->IsKeyReleased(MoveDown))
	{
		buttonState = static_cast<ButtonState>(buttonState + 1);

		if (buttonState >= STATES_TOTAL)
		{
			buttonState = static_cast<ButtonState>(0);
		}

		//Set button border position
		switch (buttonState)
		{
		case STATE_PLAY_GAME:
		{
			lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "PlayGameButton");
			ButtonBorder->SetPosition(Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f));
			break;
		}
		case STATE_OPTIONS:
		{
			lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "OptionsButton");
			ButtonBorder->SetPosition(Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f));
			break;
		}
		case STATE_HIGHSCORES:
		{
			lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "HighscoreButton");
			ButtonBorder->SetPosition(Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f));
			break;
		}
		case STATE_EXIT:
		{
			lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "ExitButton");
			ButtonBorder->SetPosition(Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f));
			break;
		}

		default:
			break;
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
			SceneManager::GetInstance()->SetActiveScene("OptionsState");
			break;
		}
		case STATE_HIGHSCORES:
		{
			cout << "Loading HighscoreState\n";
			SceneManager::GetInstance()->SetActiveScene("HighscoreState");
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
	EntityManager::GetInstance()->RemoveEntity(ButtonBorder);

	//Remove the meshes which are specific to CIntroState
	MeshBuilder::GetInstance()->RemoveMesh("MENUSTATE_BKGROUND");

	//Remove Button Meshes
	MeshBuilder::GetInstance()->RemoveMesh("PlayGameButton");
	MeshBuilder::GetInstance()->RemoveMesh("OptionsButton");
	MeshBuilder::GetInstance()->RemoveMesh("HighscoreButton");
	MeshBuilder::GetInstance()->RemoveMesh("ExitButton");
	//Remove Button Border Mesh
	MeshBuilder::GetInstance()->RemoveMesh("ButtonBorder");

	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();
	
	glEnable(GL_DEPTH_TEST);
}
