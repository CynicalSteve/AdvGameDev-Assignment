#include "SceneText.h"
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

SceneText* SceneText::sInstance = new SceneText(SceneManager::GetInstance());

SceneText::SceneText()
{
}

SceneText::SceneText(SceneManager* _sceneMgr)
{
	_sceneMgr->AddScene("Start", this);
}

SceneText::~SceneText()
{
	CSpatialPartition::GetInstance()->RemoveCamera();
	CSceneGraph::GetInstance()->Destroy();
}

void SceneText::Init()
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
	
	// Create the playerinfo instance, which manages all information about the player
	playerInfo = CPlayerInfo::GetInstance();
	playerInfo->Init();

	// Create and attach the camera to the scene
	//camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	camera.Init(playerInfo->GetPos(), playerInfo->GetTarget(), playerInfo->GetUp());
	playerInfo->AttachCamera(&camera);
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	// Load all the meshes
	MeshBuilder::GetInstance()->GenerateAxes("reference");
	MeshBuilder::GetInstance()->GenerateCrossHair("crosshair");
	MeshBuilder::GetInstance()->GenerateQuad("quad", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("quad")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GenerateText("text", 16, 16);
	MeshBuilder::GetInstance()->GetMesh("text")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GetMesh("text")->material.kAmbient.Set(1, 0, 0);
	MeshBuilder::GetInstance()->GenerateOBJ("HQAlien", "OBJ//alien.obj");
	MeshBuilder::GetInstance()->GetMesh("HQAlien")->textureID = LoadTGA("Image//alien.tga");
	MeshBuilder::GetInstance()->GenerateQuad("LQAlien", Color(1, 1, 1), 5.f);
	MeshBuilder::GetInstance()->GetMesh("LQAlien")->textureID = LoadTGA("Image//alienFront.tga");
	MeshBuilder::GetInstance()->GenerateRing("ring", Color(1, 0, 1), 36, 1, 0.5f);
	MeshBuilder::GetInstance()->GenerateSphere("lightball", Color(1, 1, 1), 18, 36, 1.f);
	MeshBuilder::GetInstance()->GenerateSphere("sphere", Color(1, 0, 0), 18, 36, 0.5f);
	MeshBuilder::GetInstance()->GenerateCone("cone", Color(0.5f, 1, 0.3f), 36, 10.f, 10.f);
	MeshBuilder::GetInstance()->GenerateCube("cube", Color(1.0f, 1.0f, 0.0f), 1.0f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kDiffuse.Set(0.99f, 0.99f, 0.99f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kSpecular.Set(0.f, 0.f, 0.f);
	MeshBuilder::GetInstance()->GenerateQuad("GRASS_DARKGREEN", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("GRASS_DARKGREEN")->textureID = LoadTGA("Image//grass_darkgreen.tga");
	MeshBuilder::GetInstance()->GenerateQuad("GEO_GRASS_LIGHTGREEN", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("GEO_GRASS_LIGHTGREEN")->textureID = LoadTGA("Image//grass_lightgreen.tga");
	MeshBuilder::GetInstance()->GenerateCube("cubeSG", Color(1.0f, 0.64f, 0.0f), 1.0f);

	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_FRONT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BACK", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_LEFT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_RIGHT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_TOP", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BOTTOM", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_FRONT")->textureID = LoadTGA("Image//SkyBox//skybox_front.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BACK")->textureID = LoadTGA("Image//SkyBox//skybox_back.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_LEFT")->textureID = LoadTGA("Image//SkyBox//skybox_left.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_RIGHT")->textureID = LoadTGA("Image//SkyBox//skybox_right.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_TOP")->textureID = LoadTGA("Image//SkyBox//skybox_top.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BOTTOM")->textureID = LoadTGA("Image//SkyBox//skybox_bottom.tga");
	MeshBuilder::GetInstance()->GenerateRay("laser", 10.0f);
	MeshBuilder::GetInstance()->GenerateQuad("GRIDMESH", Color(1, 1, 1), 10.f);

	// Set up the Spatial Partition and pass it to the EntityManager to manage
	CSpatialPartition::GetInstance()->Init(100, 100, 10, 10);
	CSpatialPartition::GetInstance()->SetMesh("GRIDMESH");
	CSpatialPartition::GetInstance()->SetCamera(&camera);
	CSpatialPartition::GetInstance()->SetLevelOfDetails(40000.0f, 160000.0f);
	EntityManager::GetInstance()->SetSpatialPartition(CSpatialPartition::GetInstance());

	//Game objects
	MeshBuilder::GetInstance()->GenerateOBJ("Safehouse", "OBJ//safehouse.obj");
	MeshBuilder::GetInstance()->GetMesh("Safehouse")->textureID = LoadTGA("Image//safehouse.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("CloverBuilding", "OBJ//cloverbuilding.obj");
	MeshBuilder::GetInstance()->GetMesh("CloverBuilding")->textureID = LoadTGA("Image//cloverbuilding.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("JigsawTower", "OBJ//JigsawTower.obj");
	MeshBuilder::GetInstance()->GetMesh("JigsawTower")->textureID = LoadTGA("Image//jigsawbuilding.tga");
	MeshBuilder::GetInstance()->GenerateQuad("GEO_PAVEMENT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("GEO_PAVEMENT")->textureID = LoadTGA("Image//pavement.tga");

	// Create entities into the scene
	//Create::Entity("reference", Vector3(0.0f, 0.0f, 0.0f)); // Reference
	Create::Entity("lightball", Vector3(lights[0]->position.x, lights[0]->position.y, lights[0]->position.z)); // Lightball

	GenericEntity* aCube = Create::Entity("cube", Vector3(-20.0f, 0.0f, -20.0f));
	aCube->SetCollider(true);
	aCube->SetAABB(Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f));
	aCube->InitLOD("cube", "sphere", "cubeSG");

	// Add the pointer to this new entity to the Scene Graph
	CSceneNode* theNode = CSceneGraph::GetInstance()->AddNode(aCube);
	if (theNode == nullptr)
	{
		cout << "EntityManager::AddEntity: Unable to add to scene graph!" << endl;
	}

	GenericEntity* anotherCube = Create::Entity("cube", Vector3(-20.0f, 1.1f, -20.0f));
	anotherCube->SetCollider(true);
	anotherCube->SetAABB(Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f));
	CSceneNode* anotherNode = theNode->AddChild(anotherCube);
	if (anotherNode == nullptr)
	{
		cout << "EntityManager::AddEntity: Unable to add to scene graph!" << endl;
	}
	
	AddHelicopters();
	AddBuildings();

	/*grandchildNode->ApplyTranslate(0.0f, 0.0f, 1.0f);
	CUpdateTransformation* aRotateMtx = new CUpdateTransformation();
	aRotateMtx->ApplyUpdate(1.0f, 0.0f, 0.0f, 1.0f);
	aRotateMtx->SetSteps(-120, 60);
	grandchildNode->SetUpdateTransformation(aRotateMtx);
	*/
	// Create a CEnemy instance
	theEnemy = new CEnemy();
	theEnemy->Init();


	groundEntity = Create::Ground("GEO_PAVEMENT", "GEO_PAVEMENT");
	//groundEntity = Create::Ground("GRASS_DARKGREEN", "GEO_GRASS_LIGHTGREEN");
//	Create::Text3DObject("text", Vector3(0.0f, 0.0f, 0.0f), "DM2210", Vector3(10.0f, 10.0f, 10.0f), Color(0, 1, 1));
	//Create::Sprite2DObject("crosshair", Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));

	SkyBoxEntity* theSkyBox = Create::SkyBox("SKYBOX_FRONT", "SKYBOX_BACK",
											 "SKYBOX_LEFT", "SKYBOX_RIGHT",
											 "SKYBOX_TOP", "SKYBOX_BOTTOM");

	// Customise the ground entity
	groundEntity->SetPosition(Vector3(0, -10, 0));
	groundEntity->SetScale(Vector3(100.0f, 100.0f, 100.0f));
	groundEntity->SetGrids(Vector3(10.0f, 1.0f, 10.0f));
	playerInfo->SetTerrain(groundEntity);
	theEnemy->SetTerrain(groundEntity);

	// Setup the 2D entities
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;
	float fontSize = 25.0f;
	float halfFontSize = fontSize / 2.0f;
	for (int i = 0; i < 3; ++i)
	{
		textObj[i] = Create::Text2DObject("text", Vector3(-halfWindowWidth, -halfWindowHeight + fontSize*i + halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f,1.0f,0.0f));
	}
}
void SceneText::AddHelicopters()
{
	GenericEntity* heli = Create::Entity("cube", Vector3(0.0f, 0.0f, 0.0f), Vector3(10.f, 10.f, 20.f));
	heli->SetCollider(false);
	heli->SetAABB(Vector3(5.f, 5.f, 10.f), Vector3(-5.f, -5.f, -10.f));
	heli->InitLOD("cube", "sphere", "cubeSG");
	CSceneNode* baseNode = CSceneGraph::GetInstance()->AddNode(heli);

	GenericEntity* heliRotator = Create::Entity("cubeSG", Vector3(0.0f, 7.0f, .0f), Vector3(5.f, 5.f, 5.f));
	heliRotator->SetCollider(false);
	heliRotator->SetAABB(Vector3(2.5f, 2.5f, 2.5f), Vector3(-2.5f, -2.5f, -2.5f));
	heliRotator->InitLOD("cubeSG", "sphere", "cube");
	CSceneNode* childNode = baseNode->AddChild(heliRotator);

	CUpdateTransformation* baseMtx = new CUpdateTransformation();
	baseMtx->ApplyUpdate(5.0f, 0.0f, 1.0f, .0f); //Rotation to heli rotor
	baseMtx->SetSteps(INT_MIN, INT_MAX);

	CUpdateTransformation* heliMove = new CUpdateTransformation();
	heliMove->ApplyUpdate(.5f, .5f, .0f); //Transformation to whole heli
	heliMove->SetSteps(INT_MIN, INT_MAX);

	childNode->SetUpdateTransformation(baseMtx);

	GenericEntity* heliBladeA = Create::Entity("cubeSG", Vector3(-10.0f, 7.0f, .0f), Vector3(20.f, 0.5f, 5.f));
	heliBladeA->SetCollider(false);
	heliBladeA->SetAABB(Vector3(10.f, 0.25f, 2.5f), Vector3(-10.f, -0.25f, -2.5f));
	heliBladeA->InitLOD("cubeSG", "sphere", "cube");
	CSceneNode* grandchildNodeA = childNode->AddChild(heliBladeA);

	GenericEntity* heliBladeB = Create::Entity("cubeSG", Vector3(10.0f, 7.0f, .0f), Vector3(20.f, 0.5f, 5.f));
	heliBladeB->SetCollider(false);
	heliBladeB->SetAABB(Vector3(10.f, 0.25f, 0.25f), Vector3(-10.f, -0.25f, -0.25f));
	heliBladeB->InitLOD("cubeSG", "sphere", "cube");
	CSceneNode* grandchildNodeB = childNode->AddChild(heliBladeB);

	//Second heli
	{
		GenericEntity* SecondHeli = Create::Entity("cube", Vector3(.0f, 0.0f, .0f), Vector3(10.f, 10.f, 20.f));
		SecondHeli->SetCollider(true);
		SecondHeli->SetAABB(Vector3(5.f, 5.f, 10.f), Vector3(-5.f, -5.f, -10.f));
		SecondHeli->InitLOD("cube", "sphere", "cubeSG");
		CSceneNode* HeliBaseNode = CSceneGraph::GetInstance()->AddNode(SecondHeli);

		GenericEntity* SecondheliRotator = Create::Entity("cubeSG", Vector3(0.0f, 10.0f, .0f), Vector3(5.f, 5.f, 5.f));
		SecondheliRotator->SetCollider(true);
		SecondheliRotator->SetAABB(Vector3(2.5f, 2.5f, 2.5f), Vector3(-2.5f, -2.5f, -2.5f));
		SecondheliRotator->InitLOD("cubeSG", "sphere", "cube");
		CSceneNode* HelichildNode = HeliBaseNode->AddChild(SecondheliRotator);

		CUpdateTransformation* baseMtx2 = new CUpdateTransformation();
		baseMtx2->ApplyUpdate(5.0f, 0.0f, 1.0f, .0f); //Rotation to heli rotor
		baseMtx2->SetSteps(INT_MIN, INT_MAX);
		HelichildNode->SetUpdateTransformation(baseMtx2);

	CUpdateTransformation* heliMove = new CUpdateTransformation();
	heliMove->ApplyUpdate(.5f, .5f, .0f); //Transformation to whole heli
	heliMove->SetSteps(INT_MIN, INT_MAX);

	HeliBaseNode->SetUpdateTransformation(heliMove);

		GenericEntity* SecondheliBladeA = Create::Entity("cubeSG", Vector3(-13.0f, 7.0f, .0f), Vector3(20.f, 0.5f, 5.f));
		SecondheliBladeA->SetCollider(true);
		SecondheliBladeA->SetAABB(Vector3(10.f, 0.25f, 2.5f), Vector3(-10.f, -0.25f, -2.5f));
		SecondheliBladeA->InitLOD("cubeSG", "sphere", "cube");
		CSceneNode* HeligrandchildNodeA = HelichildNode->AddChild(SecondheliBladeA);

		GenericEntity* SecondheliBladeB = Create::Entity("cubeSG", Vector3(13.0f, 7.0f, .0f), Vector3(20.f, 0.5f, 5.f));
		SecondheliBladeB->SetCollider(true);
		SecondheliBladeB->SetAABB(Vector3(10.f, 0.25f, 0.25f), Vector3(-10.f, -0.25f, -0.25f));
		SecondheliBladeB->InitLOD("cubeSG", "sphere", "cube");
		CSceneNode* HeligrandchildNodeB = HelichildNode->AddChild(SecondheliBladeB);
	}
}

void SceneText::AddBuildings()
{
	GenericEntity* BuildingBaseA = Create::Entity("cube", Vector3(50.0f, 0.0f, 50.0f), Vector3(25.f, 25.f, 25.f));
	BuildingBaseA->SetCollider(true);
	BuildingBaseA->SetAABB(Vector3(12.5f, 12.5f, 12.5f), Vector3(-12.5f, -12.5f, -12.5f));
	BuildingBaseA->InitLOD("cube", "sphere", "cubeSG");
	CSceneNode* BuildingBaseNodeA = CSceneGraph::GetInstance()->AddNode(BuildingBaseA);

	GenericEntity* BuildingBaseB = Create::Entity("cubeSG", Vector3(50.0f, 35.0f, 50.0f), Vector3(25.f, 25.f, 25.f));
	BuildingBaseB->SetCollider(true);
	BuildingBaseB->SetAABB(Vector3(12.5f, 12.5f, 12.5f), Vector3(-12.5f, -12.5f, -12.5f));
	BuildingBaseB->InitLOD("cubeSG", "sphere", "cube");
	CSceneNode* BuildingBaseNodeB = BuildingBaseNodeA->AddChild(BuildingBaseB);

	GenericEntity* BuildingChild = Create::Entity("sphere", Vector3(50.0f, 70.0f, 50.0f), Vector3(25.f, 25.f, 25.f));
	BuildingChild->SetCollider(true);
	BuildingChild->SetAABB(Vector3(12.5f, 12.5f, 0.5f), Vector3(-12.5f, -12.5f, -12.5f));
	BuildingChild->InitLOD("sphere", "cube", "cubeSG");
	CSceneNode* BuildingChildNode = BuildingBaseNodeB->AddChild(BuildingChild);
	
	CUpdateTransformation* BuildingMove = new CUpdateTransformation();
	BuildingMove->ApplyUpdate(0.0f, .5f, .0f);
	BuildingMove->SetSteps(0, 10);
	BuildingChildNode->SetUpdateTransformation(BuildingMove);
}
void SceneText::Update(double dt)
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
	
	if(KeyboardController::GetInstance()->IsKeyDown('5'))
	{
		lights[0]->type = Light::LIGHT_POINT;
	}
	else if(KeyboardController::GetInstance()->IsKeyDown('6'))
	{
		lights[0]->type = Light::LIGHT_DIRECTIONAL;
	}
	else if(KeyboardController::GetInstance()->IsKeyDown('7'))
	{
		lights[0]->type = Light::LIGHT_SPOT;
	}

	if(KeyboardController::GetInstance()->IsKeyDown('I'))
		lights[0]->position.z -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('K'))
		lights[0]->position.z += (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('J'))
		lights[0]->position.x -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('L'))
		lights[0]->position.x += (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('O'))
		lights[0]->position.y -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('P'))
		lights[0]->position.y += (float)(10.f * dt);

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

	if (KeyboardController::GetInstance()->IsKeyReleased('Q'))
	{
		playerInfo->currentWeapon--;
	}

	if (KeyboardController::GetInstance()->IsKeyReleased('E'))
	{
		playerInfo->currentWeapon++;
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

	//camera.Update(dt); // Can put the camera into an entity rather than here (Then we don't have to write this)

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

void SceneText::Render()
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

void SceneText::Exit()
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
