#include <iostream>
using namespace std;

#include "OptionsState.h"
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

OptionsState::OptionsState()
{
}

OptionsState::~OptionsState()
{
}

void OptionsState::Init()
{
	//Create and attach the camera to the scene
	camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	//Load all the meshes
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

	//Button Border
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "Resolution");
	MeshBuilder::GetInstance()->GenerateQuad("ButtonBorder", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("ButtonBorder")->textureID = LoadTGA("Image//buttonborder.tga");
	ButtonBorder = Create::Sprite2DObject("ButtonBorder",
		Vector3(halfWindowWidth - 10.f, CLuaInterface::GetInstance()->GetField("posY"), 1.f),
		Vector3(450, 160, 0.f));
	
	//Buttons
	//Change res button
	MeshBuilder::GetInstance()->GenerateQuad("Resolution", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("Resolution")->textureID = LoadTGA("Image//ChangeResButton.tga");
	Create::Sprite2DObject("Resolution",
		Vector3(halfWindowWidth, CLuaInterface::GetInstance()->GetField("posY"), 1.f),
		Vector3(CLuaInterface::GetInstance()->GetField("scaleX"), CLuaInterface::GetInstance()->GetField("scaleY"), 0.f));

	//Save & Exit
	MeshBuilder::GetInstance()->GenerateQuad("SaveExitButton", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("SaveExitButton")->textureID = LoadTGA("Image//SaveExitButton.tga");

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "SaveExitButton");
	Create::Sprite2DObject("SaveExitButton",
		Vector3(halfWindowWidth, CLuaInterface::GetInstance()->GetField("posY"), 1.f),
		Vector3(CLuaInterface::GetInstance()->GetField("scaleX"), CLuaInterface::GetInstance()->GetField("scaleY"), 0.f));

	//Cancel & Exit
	MeshBuilder::GetInstance()->GenerateQuad("CancelExitButton", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("CancelExitButton")->textureID = LoadTGA("Image//CancelExitButton.tga");

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "CancelExitButton");
	Create::Sprite2DObject("CancelExitButton",
		Vector3(halfWindowWidth, CLuaInterface::GetInstance()->GetField("posY"), 1.f),
		Vector3(CLuaInterface::GetInstance()->GetField("scaleX"), CLuaInterface::GetInstance()->GetField("scaleY"), 0.f));

	std::string resolutionString = std::to_string(Application::GetInstance().GetWindowWidth()) + 'x' + std::to_string(Application::GetInstance().GetWindowHeight());

	if (resolutionString == "800x600")
	{
		resolutionState = Res_800x600;
		tempWidth = 800;
		tempHeight = 600;
	}
	else if (resolutionString == "1280x720")
	{
		resolutionState = Res_1280x720;
		tempWidth = 1280;
		tempHeight = 720;
	}
	else if (resolutionString == "1366x768")
	{
		resolutionState = Res_1366x768;
		tempWidth = 1366;
		tempHeight = 768;
	}
	else if (resolutionString == "1920x1080")
	{
		resolutionState = Res_1920x1080;
		tempWidth = 1920;
		tempHeight = 1080;
	}
	else if (resolutionString == "3840x2160")
	{
		resolutionState = Res_3840x2160;
		tempWidth = 3840;
		tempHeight = 2160;
	}
	else
	{
		//If resolution is not one og the standard ones, reset to 1280x720
		resolutionState = Res_1280x720;

		CLuaInterface::GetInstance()->saveIntValue("width", 1280, "SaveResolution", true);
		CLuaInterface::GetInstance()->saveIntValue("height", 720, "SaveResolution", false);

		Application::GetInstance().SetWindowSize(1280, 720);
		tempWidth = 1280;
		tempHeight = 720;
	}

	cout << "OptionsState Loaded\n";
	buttonState = STATE_RESOLUTION;
	MoveUp = CLuaInterface::GetInstance()->getCharValue("MoveUp");
	MoveDown = CLuaInterface::GetInstance()->getCharValue("MoveDown");
}

void OptionsState::Update(double dt)
{
	if (KeyboardController::GetInstance()->IsKeyReleased(MoveUp))
	{
		buttonState = static_cast<ButtonState>(buttonState - 1);

		if (buttonState < 0)
		{
			buttonState = static_cast<ButtonState>(STATES_TOTAL - 1);
		}

		ButtonborderPosSnap();
	}

	if (KeyboardController::GetInstance()->IsKeyReleased(MoveDown))
	{
		buttonState = static_cast<ButtonState>(buttonState + 1);

		if (buttonState >= STATES_TOTAL)
		{
			buttonState = static_cast<ButtonState>(0);
		}

		ButtonborderPosSnap();
	}

	if (KeyboardController::GetInstance()->IsKeyReleased(VK_RETURN))
	{
		switch (buttonState)
		{
		case STATE_RESOLUTION:
		{
			resolutionState = static_cast<Resolutions>(resolutionState + 1);

			if (resolutionState >= TOTAL_RESOLUTIONS)
			{
				resolutionState = static_cast<Resolutions>(0);
			}

			switch (resolutionState)
			{
			case Res_800x600:
			{
				Application::GetInstance().SetWindowSize(800, 600);
				break;
			}
			case Res_1280x720:
			{
				Application::GetInstance().SetWindowSize(1280, 720);
				break;
			}
			case Res_1366x768:
			{
				Application::GetInstance().SetWindowSize(1366, 768);
				break;
			}
			case Res_1920x1080:
			{
				Application::GetInstance().SetWindowSize(1920, 1080);
				break;
			}
			case Res_3840x2160:
			{
				Application::GetInstance().SetWindowSize(3840, 2160);
				break;
			}

			default:
				break;
			}
			break;
		}
		case STATE_SAVEEXIT:
		{
			CLuaInterface::GetInstance()->saveIntValue("width", Application::GetInstance().GetWindowWidth(), "SaveResolution", true);
			CLuaInterface::GetInstance()->saveIntValue("height", Application::GetInstance().GetWindowHeight(), "SaveResolution", false);
			SceneManager::GetInstance()->SetActiveScene("MenuState");
			break;
		}
		case STATE_CANCELEXIT:
		{
			Application::GetInstance().SetWindowSize(tempWidth, tempHeight);
			SceneManager::GetInstance()->SetActiveScene("MenuState");
			break;
		}
		default:
			break;
		}
	}
}

void OptionsState::Render()
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

void OptionsState::Exit()
{
	//Remove the entity from EntitManager
	EntityManager::GetInstance()->RemoveEntity(MenuStateBackground);
	EntityManager::GetInstance()->RemoveEntity(ButtonBorder);

	//Remove the meshes which are specific to CIntroState
	MeshBuilder::GetInstance()->RemoveMesh("MENUSTATE_BKGROUND");
	MeshBuilder::GetInstance()->RemoveMesh("ButtonBorder");
	MeshBuilder::GetInstance()->RemoveMesh("SaveExitButton");
	MeshBuilder::GetInstance()->RemoveMesh("CancelExitButton");
	MeshBuilder::GetInstance()->RemoveMesh("Resolution");

	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();

	glEnable(GL_DEPTH_TEST);
}

void OptionsState::ButtonborderPosSnap()
{
	//Set button border position
	switch (buttonState)
	{
	case STATE_RESOLUTION:
	{
		lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "Resolution");
		ButtonBorder->SetPosition(Vector3(halfWindowWidth, CLuaInterface::GetInstance()->GetField("posY"), 1.f));
		break;
	}
	case STATE_SAVEEXIT:
	{
		lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "SaveExitButton");
		ButtonBorder->SetPosition(Vector3(halfWindowWidth, CLuaInterface::GetInstance()->GetField("posY"), 1.f));
		break;
	}
	case STATE_CANCELEXIT:
	{
		lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "CancelExitButton");
		ButtonBorder->SetPosition(Vector3(halfWindowWidth, CLuaInterface::GetInstance()->GetField("posY"), 1.f));
		break;
	}

	default:
		break;
	}
}
