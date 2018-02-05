#include <iostream>
#include <sstream>
using namespace std;

#include "HighscoreState.h"
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

HighscoreState::HighscoreState()
{
}

HighscoreState::~HighscoreState()
{
}

void HighscoreState::Init()
{
	//Create and attach the camera to the scene
	camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	srand(0);
	isHighscoreFull = true;
	ScoreToInput = 0;

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

	MoveUp = CLuaInterface::GetInstance()->getCharValue("MoveUp");
	MoveDown = CLuaInterface::GetInstance()->getCharValue("MoveDown");

	float fontSize = 25.0f;
	float halfFontSize = fontSize * .5f;

	MeshBuilder::GetInstance()->GenerateText("text", 16, 16);
	MeshBuilder::GetInstance()->GetMesh("text")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GetMesh("text")->material.kAmbient.Set(1, 0, 0);
	for (int i = 0; i < 10; ++i)
	{
		textObj[i] = Create::Text2DObject("text", Vector3(halfWindowWidth - (halfWindowWidth * 0.5f), Application::GetInstance().GetWindowHeight() - 100 - fontSize * i + halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(.0f, 1.0f, .0f));
	}

	//textObj[0]->SetText("Helo World");
	CLuaInterface::GetInstance()->SetLuaFile("Image//DM2240_Highscore.lua", CLuaInterface::GetInstance()->theLuaState);
	std::string PlayerStats;

	std::vector<unsigned int> PlayerScoreVector;
	std::vector<std::string> PlayerNameVector;

	for (size_t i = 0; i < 10; ++i)
	{
		std::string PlayerNum = "Player0";
		PlayerNum[6] = static_cast<char>(i) + 48;
		
		std::string PlayerName = CLuaInterface::GetInstance()->getStringValue(PlayerNum);
		PlayerStats = PlayerName;

		PlayerStats.append(" --> ");
		PlayerNum.push_back('N');
		unsigned int PlayerScore = CLuaInterface::GetInstance()->getIntValue(PlayerNum);
		PlayerStats.append(std::to_string(PlayerScore));

		if (PlayerScore <= 0)
		{
			isHighscoreFull = false;
		}

		textObj[i]->SetText(PlayerStats);

		PlayerNameVector.push_back(PlayerName);
		PlayerScoreVector.push_back(PlayerScore);

		PlayerStats.clear();
	}

	textObj[11] = Create::Text2DObject("text", Vector3(halfWindowWidth - (halfWindowWidth * 0.5f), Application::GetInstance().GetWindowHeight() - 100 - fontSize * 16 + halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(.0f, 1.0f, .0f));
}

void HighscoreState::Update(double dt)
{

	std::ostringstream ss;
	ss.precision(5);
	ss << "Score To Input: " << ScoreToInput;
	textObj[11]->SetText(ss.str());
}

void HighscoreState::Render()
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

void HighscoreState::Exit()
{
	//Remove the entity from EntitManager
	EntityManager::GetInstance()->RemoveEntity(MenuStateBackground);

	//Remove the meshes which are specific to CIntroState
	MeshBuilder::GetInstance()->RemoveMesh("MENUSTATE_BKGROUND");

	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();

	glEnable(GL_DEPTH_TEST);
}
