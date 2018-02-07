#include "SceneGame.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>
#include "KeyboardController.h"
#include "MouseController.h"
#include "GameStateManagement\SceneManager.h"
#include "GraphicsManager.h"
#include "ShaderProgram.h"
#include "EntityManager.h"

#include "GenericEntity.h"
#include "GroundEntity.h"
#include "TextEntity.h"
#include "SpriteEntity.h"
#include "Light.h"
#include "SkyBox/SkyBoxEntity.h"
#include "SceneGraph\SceneGraph.h"
#include "SpatialPartition\SpatialPartition.h"

#include <iostream>
using namespace std;

SceneGame* SceneGame::sInstance = new SceneGame(SceneManager::GetInstance());

SceneGame::SceneGame()
{
}

SceneGame::SceneGame(SceneManager* _sceneMgr)
{
	_sceneMgr->AddScene("Start", this);
}

SceneGame::~SceneGame()
{
	CSpatialPartition::GetInstance()->RemoveCamera();
	CSceneGraph::GetInstance()->Destroy();
}

void SceneGame::Init()
{
	ShaderInit();

	CLuaInterface::GetInstance()->SetLuaFile("Image//DM2240.lua", CLuaInterface::GetInstance()->theLuaState);
	EntityManager::GetInstance()->ClearEntities();

	lights[0] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[0]", lights[0]);
	lights[0]->type = Light::LIGHT_DIRECTIONAL;
	lights[0]->position.Set(0, 20, 0);
	lights[0]->color.Set(1, 1, 1);
	lights[0]->power = 1;
	lights[0]->kC = 1.f;
	lights[0]->kL = 0.01f;
	lights[0]->kQ = 0.001f;
	lights[0]->cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0]->cosInner = cos(Math::DegreeToRadian(30));
	lights[0]->exponent = 3.f;
	lights[0]->spotDirection.Set(0.f, 1.f, 0.f);
	lights[0]->name = "lights[0]";

	lights[1] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[1]", lights[1]);
	lights[1]->type = Light::LIGHT_DIRECTIONAL;
	lights[1]->position.Set(1, 1, 0);
	lights[1]->color.Set(1, 1, 0.5f);
	lights[1]->power = 0.4f;
	lights[1]->name = "lights[1]";

	currProg->UpdateInt("numLights", 1);
	currProg->UpdateInt("textEnabled", 0);
	
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	// Load all the meshes
	CreateGameObjects();
	
	// Set up the Spatial Partition and pass it to the EntityManager to manage
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "GridMeshSize");
	CSpatialPartition::GetInstance()->Init(CLuaInterface::GetInstance()->getFieldFloat("xGridSize"), CLuaInterface::GetInstance()->getFieldFloat("zGridSize"), 
		CLuaInterface::GetInstance()->getFieldFloat("xNumofGrid"), CLuaInterface::GetInstance()->getFieldFloat("zNumofGrid"));
	CSpatialPartition::GetInstance()->SetMesh("GRIDMESH");
	CSpatialPartition::GetInstance()->SetCamera(&camera);

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "GridLoD");
	CSpatialPartition::GetInstance()->SetLevelOfDetails(CLuaInterface::GetInstance()->getFieldFloat("distance_High2Mid"), 
		CLuaInterface::GetInstance()->getFieldFloat("distance_Mid2Low"));
	EntityManager::GetInstance()->SetSpatialPartition(CSpatialPartition::GetInstance());

	// Create entities into the scene
	//Create::Entity("reference", Vector3(0.0f, 0.0f, 0.0f)); // Reference

	//Initialise all things that take values from GameSaveData.lua
	GameSaveInit();

	groundEntity = Create::Ground("GEO_PAVEMENT", "GEO_PAVEMENT");

	SkyBoxEntity* theSkyBox = Create::SkyBox("SKYBOX_FRONT", "SKYBOX_BACK",
											 "SKYBOX_LEFT", "SKYBOX_RIGHT",
											 "SKYBOX_TOP", "SKYBOX_BOTTOM");

	// Customise the ground entity
	groundEntity->SetPosition(CLuaInterface::GetInstance()->getVector3Values("GroundPos"));
	groundEntity->SetScale(CLuaInterface::GetInstance()->getVector3Values("GroundScale"));
	groundEntity->SetGrids(CLuaInterface::GetInstance()->getVector3Values("GroundGrids"));
	playerInfo->SetTerrain(groundEntity);
	theEnemy->SetTerrain(groundEntity);

	// Setup the 2D entities
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() * .5f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() * .5f;
	float fontSize = 25.0f;
	float halfFontSize = fontSize / 2.0f;
	for (int i = 0; i < 3; ++i)
	{
		textObj[i] = Create::Text2DObject("text", Vector3(-halfWindowWidth, -halfWindowHeight + fontSize*i + halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f,1.0f,0.0f));
	}

	KeySaveGame = CLuaInterface::GetInstance()->getCharValue("SaveGame");
	KeyPrevWeapon = CLuaInterface::GetInstance()->getCharValue("PrevWeapon");
	KeyNextWeapon = CLuaInterface::GetInstance()->getCharValue("NextWeapon");
}

void SceneGame::Update(double dt)
{
	// Update our entities
	EntityManager::GetInstance()->Update(dt);

	// THIS WHOLE CHUNK TILL <THERE> CAN REMOVE INTO ENTITIES LOGIC! Or maybe into a scene function to keep the update clean
	if(KeyboardController::GetInstance()->IsKeyDown('1'))
		glEnable(GL_CULL_FACE);
	if(KeyboardController::GetInstance()->IsKeyDown('2'))
		glDisable(GL_CULL_FACE);
	if(KeyboardController::GetInstance()->IsKeyDown('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(KeyboardController::GetInstance()->IsKeyDown('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (KeyboardController::GetInstance()->IsKeyReleased('M'))
	{
		CSceneNode* theNode = CSceneGraph::GetInstance()->GetNode(1);
		Vector3 pos = theNode->GetEntity()->GetPosition();
		theNode->GetEntity()->SetPosition(Vector3(pos.x + 50.0f, pos.y, pos.z + 50.0f));
	}
	if (KeyboardController::GetInstance()->IsKeyReleased('N'))
	{
		CSpatialPartition::GetInstance()->PrintSelf();
	}

	// if the left mouse button was released
	if (MouseController::GetInstance()->IsButtonReleased(MouseController::LMB))
	{
		cout << "Left Mouse Button was released!" << endl;
	}
	if (MouseController::GetInstance()->IsButtonReleased(MouseController::RMB))
	{
		cout << "Right Mouse Button was released!" << endl;
	}
	if (MouseController::GetInstance()->IsButtonReleased(MouseController::MMB))
	{
		cout << "Middle Mouse Button was released!" << endl;
	}
	if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_XOFFSET) != 0.0)
	{
		cout << "Mouse Wheel has offset in X-axis of " << MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_XOFFSET) << endl;
		
		
	}
	if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) != 0.0)
	{
		cout << "Mouse Wheel has offset in Y-axis of " << MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) << endl;

		
	}

	if (KeyboardController::GetInstance()->IsKeyReleased(KeyPrevWeapon))
	{
		playerInfo->currentWeapon--;
	}

	if (KeyboardController::GetInstance()->IsKeyReleased(KeyNextWeapon))
	{
		playerInfo->currentWeapon++;
	}

	if (KeyboardController::GetInstance()->IsKeyReleased(KeySaveGame))
	{
		SaveGame();
	}

	// <THERE>

	// Update the player position and other details based on keyboard and mouse inputs
	if (playerInfo->currentWeapon < 0)
	{
		playerInfo->currentWeapon = CPlayerInfo::InventoryWeapons::WEAPON_INVENTORY_TOTAL - 1;
	}
	else if (playerInfo->currentWeapon > CPlayerInfo::InventoryWeapons::WEAPON_INVENTORY_TOTAL - 1)
	{
		playerInfo->currentWeapon = 0;
	}
	playerInfo->Update(dt);

	GraphicsManager::GetInstance()->UpdateLights(dt);

	// Update the 2 text object values. NOTE: Can do this in their own class but i'm lazy to do it now :P
	// Eg. FPSRenderEntity or inside RenderUI for LightEntity
	std::ostringstream ss;
	ss.precision(5);
	float fps = (float)(1.f / dt);
	ss << "FPS: " << fps;
	textObj[0]->SetText(ss.str());

	ss.str("");
	ss.precision(4);
	ss << "Target:" << playerInfo->GetTarget();
	textObj[1]->SetText(ss.str());

	ss.str("");
	ss.precision(4);
	ss << "Position:" << playerInfo->GetPos();
	textObj[2]->SetText(ss.str());
}

void SceneGame::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GraphicsManager::GetInstance()->UpdateLightUniforms();

	// Setup 3D pipeline then render 3D
	GraphicsManager::GetInstance()->SetPerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	GraphicsManager::GetInstance()->AttachCamera(&camera);
	EntityManager::GetInstance()->Render();

	// Setup 2D pipeline then render 2D
	int halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2;
	int halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2;
	GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
	GraphicsManager::GetInstance()->DetachCamera();
	EntityManager::GetInstance()->RenderUI();
}

void SceneGame::Exit()
{
	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();
	playerInfo->DetachCamera();

	if (playerInfo->DropInstance() == false)
	{
#if _DEBUGMODE==1
		cout << "Unable to drop PlayerInfo class" << endl;
#endif
	}

	// Delete the lights
	delete lights[0];
	delete lights[1];
}

void SceneGame::ShaderInit()
{
	currProg = GraphicsManager::GetInstance()->LoadShader("default", "Shader//Texture.vertexshader", "Shader//Texture.fragmentshader");

	// Tell the shader program to store these uniform locations
	currProg->AddUniform("MVP");
	currProg->AddUniform("MV");
	currProg->AddUniform("MV_inverse_transpose");
	currProg->AddUniform("material.kAmbient");
	currProg->AddUniform("material.kDiffuse");
	currProg->AddUniform("material.kSpecular");
	currProg->AddUniform("material.kShininess");
	currProg->AddUniform("lightEnabled");
	currProg->AddUniform("numLights");
	currProg->AddUniform("lights[0].type");
	currProg->AddUniform("lights[0].position_cameraspace");
	currProg->AddUniform("lights[0].color");
	currProg->AddUniform("lights[0].power");
	currProg->AddUniform("lights[0].kC");
	currProg->AddUniform("lights[0].kL");
	currProg->AddUniform("lights[0].kQ");
	currProg->AddUniform("lights[0].spotDirection");
	currProg->AddUniform("lights[0].cosCutoff");
	currProg->AddUniform("lights[0].cosInner");
	currProg->AddUniform("lights[0].exponent");
	currProg->AddUniform("lights[1].type");
	currProg->AddUniform("lights[1].position_cameraspace");
	currProg->AddUniform("lights[1].color");
	currProg->AddUniform("lights[1].power");
	currProg->AddUniform("lights[1].kC");
	currProg->AddUniform("lights[1].kL");
	currProg->AddUniform("lights[1].kQ");
	currProg->AddUniform("lights[1].spotDirection");
	currProg->AddUniform("lights[1].cosCutoff");
	currProg->AddUniform("lights[1].cosInner");
	currProg->AddUniform("lights[1].exponent");
	currProg->AddUniform("colorTextureEnabled");
	currProg->AddUniform("colorTexture");
	currProg->AddUniform("textEnabled");
	currProg->AddUniform("textColor");

	// Tell the graphics manager to use the shader we just loaded
	GraphicsManager::GetInstance()->SetActiveShader("default");
}

void SceneGame::GameSaveInit()
{
	CLuaInterface::GetInstance()->SetLuaFile("Image//GameSaveData.lua", CLuaInterface::GetInstance()->theLuaState);

	// Create the playerinfo instance, which manages all information about the player
	playerInfo = CPlayerInfo::GetInstance();
	playerInfo->Init();

	// Create and attach the camera to the scene
	camera.Init(playerInfo->GetPos(), playerInfo->GetTarget(), playerInfo->GetUp());
	playerInfo->AttachCamera(&camera);

	// Create a CEnemy instance
	theEnemy = new CEnemy();
	theEnemy->Init();

	CLuaInterface::GetInstance()->SetLuaFile("Image//DM2240.lua", CLuaInterface::GetInstance()->theLuaState);
	playerInfo->InitKeyInputs();
}

void SceneGame::SaveGame()
{
   CLuaInterface::GetInstance()->saveVector3Value("PlayerPos", playerInfo->GetPos(), "SaveToGameFile", true);
}

void SceneGame::CreateGameObjects()
{
	//Axes
	MeshBuilder::GetInstance()->GenerateAxes(CLuaInterface::GetInstance()->getStringValue("AxesName"));
	//Crosshair
	MeshBuilder::GetInstance()->GenerateCrossHair(CLuaInterface::GetInstance()->getStringValue("CrossHairName"));

	//Calibri Quad
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "quad");
	std::string Name = CLuaInterface::GetInstance()->getFieldString("Name");
	float Length = CLuaInterface::GetInstance()->getFieldFloat("length");
	std::string TextureName = CLuaInterface::GetInstance()->getFieldString("Texture");
	std::string ColorName = CLuaInterface::GetInstance()->getStringValue("quadColor");

	MeshBuilder::GetInstance()->GenerateQuad(Name, Color(ColorName), Length);
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(TextureName.c_str());

	//Text Quad
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "text");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	unsigned int row = CLuaInterface::GetInstance()->getFieldFloat("row");
	unsigned int col = CLuaInterface::GetInstance()->getFieldFloat("column");
	TextureName = CLuaInterface::GetInstance()->getFieldString("Texture");

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "textMaterialRGB");

	MeshBuilder::GetInstance()->GenerateText(Name, row, col);
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(TextureName.c_str());
	MeshBuilder::GetInstance()->GetMesh(Name)->material.kAmbient.Set
	(CLuaInterface::GetInstance()->getFieldFloat("r"), CLuaInterface::GetInstance()->getFieldFloat("g"),
		CLuaInterface::GetInstance()->getFieldFloat("b"));

	//HQ Alien
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "HQAlien");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	std::string OBJdir = CLuaInterface::GetInstance()->getFieldString("ObjDir");
	TextureName = CLuaInterface::GetInstance()->getFieldString("Texture");

	MeshBuilder::GetInstance()->GenerateOBJ(Name, OBJdir);
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(TextureName.c_str());

	//LQ Alien
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "LQAlien");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	ColorName = CLuaInterface::GetInstance()->getFieldString("ColorName");
	Length = CLuaInterface::GetInstance()->getFieldFloat("length");
	TextureName = CLuaInterface::GetInstance()->getFieldString("Texture");

	MeshBuilder::GetInstance()->GenerateQuad(Name, Color(ColorName), Length);
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(TextureName.c_str());

	//Ring
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "ring");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	ColorName = CLuaInterface::GetInstance()->getFieldString("ColorName");
	unsigned int slices = CLuaInterface::GetInstance()->getFieldFloat("slices");
	float innerR = CLuaInterface::GetInstance()->getFieldFloat("innerRadius");
	float outerR = CLuaInterface::GetInstance()->getFieldFloat("outerRadius");

	MeshBuilder::GetInstance()->GenerateRing(Name, Color(ColorName), slices, innerR, outerR);

	//Lightball
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "lightball");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	ColorName = CLuaInterface::GetInstance()->getFieldString("ColorName");
	unsigned int stacks = CLuaInterface::GetInstance()->getFieldFloat("stacks");
	slices = CLuaInterface::GetInstance()->getFieldFloat("slices");

	MeshBuilder::GetInstance()->GenerateSphere(Name, Color(ColorName), slices, stacks);
	Create::Entity(Name, Vector3(lights[0]->position.x, lights[0]->position.y, lights[0]->position.z)); // Lightball

	//Sphere
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "sphere");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	ColorName = CLuaInterface::GetInstance()->getFieldString("ColorName");
	stacks = CLuaInterface::GetInstance()->getFieldFloat("stacks");
	slices = CLuaInterface::GetInstance()->getFieldFloat("slices");
	float radius = CLuaInterface::GetInstance()->getFieldFloat("radius");

	MeshBuilder::GetInstance()->GenerateSphere(Name, Color(ColorName), stacks, slices, radius);

	//Cube
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "cube");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	ColorName = CLuaInterface::GetInstance()->getFieldString("ColorName");

	MeshBuilder::GetInstance()->GenerateCube(Name, Color(ColorName));

	//Cone
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "cone");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	slices = CLuaInterface::GetInstance()->getFieldFloat("slices");
	radius = CLuaInterface::GetInstance()->getFieldFloat("radius");
	float height = CLuaInterface::GetInstance()->getFieldFloat("height");

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "coneColor");
	MeshBuilder::GetInstance()->GenerateCone(Name, Color(CLuaInterface::GetInstance()->getFieldFloat("r"),
		CLuaInterface::GetInstance()->getFieldFloat("g"), CLuaInterface::GetInstance()->getFieldFloat("b")),
		slices, radius, height);

	//Grass Dark Green
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "DarkGrass");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	ColorName = CLuaInterface::GetInstance()->getFieldString("ColorName");
	TextureName = CLuaInterface::GetInstance()->getFieldString("Texture");

	MeshBuilder::GetInstance()->GenerateQuad(Name, Color(ColorName));
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(TextureName.c_str());

	//Grass Light Green
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "LightGrass");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	ColorName = CLuaInterface::GetInstance()->getFieldString("ColorName");
	TextureName = CLuaInterface::GetInstance()->getFieldString("Texture");

	MeshBuilder::GetInstance()->GenerateQuad(Name, Color(ColorName));
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(TextureName.c_str());

	//CubeSG
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "CubeSG");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "CubeSGColor");
	MeshBuilder::GetInstance()->GenerateCube(Name, Color(CLuaInterface::GetInstance()->getFieldFloat("r"),
		CLuaInterface::GetInstance()->getFieldFloat("g"), CLuaInterface::GetInstance()->getFieldFloat("b")));

	//Skybox
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "SkyboxFront");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	ColorName = CLuaInterface::GetInstance()->getFieldString("ColorName");
	TextureName = CLuaInterface::GetInstance()->getFieldString("Texture");
	MeshBuilder::GetInstance()->GenerateQuad(Name, Color(ColorName));
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(TextureName.c_str());

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "SkyboxBack");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	ColorName = CLuaInterface::GetInstance()->getFieldString("ColorName");
	TextureName = CLuaInterface::GetInstance()->getFieldString("Texture");
	MeshBuilder::GetInstance()->GenerateQuad(Name, Color(ColorName));
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(TextureName.c_str());

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "SkyboxLeft");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	ColorName = CLuaInterface::GetInstance()->getFieldString("ColorName");
	TextureName = CLuaInterface::GetInstance()->getFieldString("Texture");
	MeshBuilder::GetInstance()->GenerateQuad(Name, Color(ColorName));
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(TextureName.c_str());

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "SkyboxRight");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	ColorName = CLuaInterface::GetInstance()->getFieldString("ColorName");
	TextureName = CLuaInterface::GetInstance()->getFieldString("Texture");
	MeshBuilder::GetInstance()->GenerateQuad(Name, Color(ColorName));
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(TextureName.c_str());

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "SkyboxTop");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	ColorName = CLuaInterface::GetInstance()->getFieldString("ColorName");
	TextureName = CLuaInterface::GetInstance()->getFieldString("Texture");
	MeshBuilder::GetInstance()->GenerateQuad(Name, Color(ColorName));
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(TextureName.c_str());

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "SkyboxBottom");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	ColorName = CLuaInterface::GetInstance()->getFieldString("ColorName");
	TextureName = CLuaInterface::GetInstance()->getFieldString("Texture");
	MeshBuilder::GetInstance()->GenerateQuad(Name, Color(ColorName));
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(TextureName.c_str());

	//Laser
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "Laser");
	MeshBuilder::GetInstance()->GenerateRay(CLuaInterface::GetInstance()->getFieldString("Name"), CLuaInterface::GetInstance()->getFieldFloat("length"));

	//Grid Mesh
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "GridMesh");
	MeshBuilder::GetInstance()->GenerateQuad(CLuaInterface::GetInstance()->getFieldString("Name"),
		Color(CLuaInterface::GetInstance()->getFieldString("ColorName")), CLuaInterface::GetInstance()->getFieldFloat("length"));

	//Game objects
	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "Safehouse");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	MeshBuilder::GetInstance()->GenerateOBJ(Name, CLuaInterface::GetInstance()->getFieldString("ObjDir"));
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(CLuaInterface::GetInstance()->getFieldString("Texture").c_str());

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "CloverBuilding");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	MeshBuilder::GetInstance()->GenerateOBJ(Name, CLuaInterface::GetInstance()->getFieldString("ObjDir"));
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(CLuaInterface::GetInstance()->getFieldString("Texture").c_str());

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "JigsawTower");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	MeshBuilder::GetInstance()->GenerateOBJ(Name, CLuaInterface::GetInstance()->getFieldString("ObjDir"));
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(CLuaInterface::GetInstance()->getFieldString("Texture").c_str());

	lua_getglobal(CLuaInterface::GetInstance()->theLuaState, "Pavement");
	Name = CLuaInterface::GetInstance()->getFieldString("Name");
	MeshBuilder::GetInstance()->GenerateQuad(Name, Color(CLuaInterface::GetInstance()->getFieldString("ColorName")));
	MeshBuilder::GetInstance()->GetMesh(Name)->textureID = LoadTGA(CLuaInterface::GetInstance()->getFieldString("Texture").c_str());
}
