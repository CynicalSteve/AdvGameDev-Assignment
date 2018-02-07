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

	//Buttons
	//SaveScore Button
	MeshBuilder::GetInstance()->GenerateQuad("SaveScoreButton", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("SaveScoreButton")->textureID = LoadTGA("Image//SaveScoreButton.tga");

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "SaveScoreButton");
	Create::Sprite2DObject("SaveScoreButton",
		Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f),
		Vector3(CLuaInterface::GetInstance()->getFieldFloat("scaleX"), CLuaInterface::GetInstance()->getFieldFloat("scaleY"), 0.f));

	//Button Border
	MeshBuilder::GetInstance()->GenerateQuad("ButtonBorder", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("ButtonBorder")->textureID = LoadTGA("Image//buttonborder.tga");
	ButtonBorder = Create::Sprite2DObject("ButtonBorder",
		Vector3(halfWindowWidth - 10.f, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f),
		Vector3(450, 160, 0.f));

	//ExitButton
	MeshBuilder::GetInstance()->GenerateQuad("ExitButton", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("ExitButton")->textureID = LoadTGA("Image//ExitButton.tga");

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "ExitMenuButton");
	Create::Sprite2DObject("ExitButton",
		Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f),
		Vector3(CLuaInterface::GetInstance()->getFieldFloat("scaleX"), CLuaInterface::GetInstance()->getFieldFloat("scaleY"), 0.f));

	ScoreIncrease = CLuaInterface::GetInstance()->getCharValue("ScoreIncrease");
	ScoreDecrease = CLuaInterface::GetInstance()->getCharValue("ScoreDecrease");
	MoveUp = CLuaInterface::GetInstance()->getCharValue("MoveUp");
	MoveDown = CLuaInterface::GetInstance()->getCharValue("MoveDown");

	buttonState = STATE_SAVESCORE;

	float fontSize = 25.0f;
	float halfFontSize = fontSize * .5f;

	MeshBuilder::GetInstance()->GenerateText("text", 16, 16);
	MeshBuilder::GetInstance()->GetMesh("text")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GetMesh("text")->material.kAmbient.Set(1, 0, 0);
	for (int i = 0; i < 10; ++i)
	{
		textObj[i] = Create::Text2DObject("text", Vector3(halfWindowWidth - (halfWindowWidth * 0.5f), Application::GetInstance().GetWindowHeight() - 100 - fontSize * i + halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(.0f, 1.0f, .0f));
	}

	CLuaInterface::GetInstance()->SetLuaFile("Image//DM2240_Highscore.lua", CLuaInterface::GetInstance()->theLuaState);
	std::string PlayerStats;

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

	textObj[10] = Create::Text2DObject("text", Vector3(halfWindowWidth - (halfWindowWidth * 0.5f), Application::GetInstance().GetWindowHeight() - 50), "", Vector3(fontSize, fontSize, fontSize), Color(.0f, 1.0f, .0f));

	CLuaInterface::GetInstance()->SetLuaFile("Image//MenuData.lua", CLuaInterface::GetInstance()->theLuaState);
}

void HighscoreState::Update(double dt)
{
	if (KeyboardController::GetInstance()->IsKeyReleased(ScoreIncrease))
	{
		ScoreToInput += 1000;
	}

	if (KeyboardController::GetInstance()->IsKeyReleased(ScoreDecrease))
	{
		if (ScoreToInput <= 999)
		{
			ScoreToInput = 0;
		}
		else
		{
			ScoreToInput -= 1000;
		}
	}

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
		case STATE_SAVESCORE:
		{	
			if (ScoreToInput == 0)
			{
				break;
			}

			bool EligibleForHighscore = false;
			std::string tempPlayerName = "Player" + std::to_string(rand() % 100 + 1);

			//Check if score should be part of highscore
			for (size_t i = 0; i < PlayerNameVector.size(); ++i)
			{
				if (tempPlayerName == PlayerNameVector[i])
				{
					if (ScoreToInput > PlayerScoreVector[i])
					{
						PlayerScoreVector[i] = ScoreToInput;
						EligibleForHighscore = true;
					}
					else
					{
						break;
					}

				}
				else if (ScoreToInput >= PlayerScoreVector[i])
				{
					PlayerScoreVector.insert(PlayerScoreVector.begin() + i, ScoreToInput);
					PlayerNameVector.insert(PlayerNameVector.begin() + i, tempPlayerName);
					
					PlayerNameVector.pop_back();
					PlayerScoreVector.pop_back();

					EligibleForHighscore = true;
					break;
				}
			}

			if (!EligibleForHighscore)
			{
				break;
			}

			bool overwrite = true;
			//Save into highscore lua
			for (size_t i = 0; i < PlayerNameVector.size(); ++i)
			{
				std::string PlayerNum = "Player0";
				PlayerNum[6] = static_cast<char>(i) + 48;

				CLuaInterface::GetInstance()->saveStringValue(PlayerNum, PlayerNameVector[i], "SaveHighscore", overwrite);
				PlayerNum.push_back('N');
				overwrite = false;
				CLuaInterface::GetInstance()->saveIntValue(PlayerNum, PlayerScoreVector[i], "SaveHighscore");
			}

			std::string PlayerStats;
			CLuaInterface::GetInstance()->SetLuaFile("Image//DM2240_Highscore.lua", CLuaInterface::GetInstance()->theLuaState);
			PlayerNameVector.clear();
			PlayerScoreVector.clear();

			//Reload the render text
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
			ScoreToInput = 0;
			CLuaInterface::GetInstance()->SetLuaFile("Image//MenuData.lua", CLuaInterface::GetInstance()->theLuaState);

			break;
		}
		case STATE_RETURNMAIN:
		{
			SceneManager::GetInstance()->SetActiveScene("MenuState");
			break;
		}
		default:
			break;
		}
	}	

	std::ostringstream ss;
	ss.precision(5);
	ss << "Score To Input: " << ScoreToInput;
	textObj[10]->SetText(ss.str());
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
	EntityManager::GetInstance()->RemoveEntity(ButtonBorder);

	//Remove the meshes which are specific to CIntroState
	MeshBuilder::GetInstance()->RemoveMesh("MENUSTATE_BKGROUND");
	MeshBuilder::GetInstance()->RemoveMesh("ButtonBorder");
	MeshBuilder::GetInstance()->RemoveMesh("SaveScoreButton");
	MeshBuilder::GetInstance()->RemoveMesh("ExitButton");
	MeshBuilder::GetInstance()->RemoveMesh("text");

	for (size_t i = 0; i < 11; ++i)
	{
		delete textObj[i];
		textObj[i] = nullptr;
	}

	PlayerNameVector.clear();
	PlayerScoreVector.clear();

	EntityManager::GetInstance()->ClearEntities();

	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();

	glEnable(GL_DEPTH_TEST);
}

void HighscoreState::ButtonborderPosSnap()
{
	//Set button border position
	switch (buttonState)
	{
	case STATE_SAVESCORE:
	{
		lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "SaveScoreButton");
		ButtonBorder->SetPosition(Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f));
		break;
	}
	case STATE_RETURNMAIN:
	{
		lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "ExitMenuButton");
		ButtonBorder->SetPosition(Vector3(halfWindowWidth, CLuaInterface::GetInstance()->getFieldFloat("posY"), 1.f));
		break;
	}

	default:
		break;
	}
}
