#include "SP2_Scene01.h"

#include "GL\glew.h"
#include "Application.h"
#include "Utility.h"
#include "shader.hpp"
#include "LoadTGA.h"
#include "Building.h"
#include "Collision.h"
#include "SceneManager.h"

#define VK_0 0x30
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39

SP2_Scene01::SP2_Scene01()
{
}

SP2_Scene01::~SP2_Scene01()
{
}

void SP2_Scene01::Init()
{
	// Init VBO here
	glClearColor(0.f, 0.f, 0.f, 0.f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Load vertex and fragment shaders
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");

	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");

	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

	m_parameters[U_LIGHT2_POSITION] = glGetUniformLocation(m_programID, "lights[2].position_cameraspace");
	m_parameters[U_LIGHT2_COLOR] = glGetUniformLocation(m_programID, "lights[2].color");
	m_parameters[U_LIGHT2_POWER] = glGetUniformLocation(m_programID, "lights[2].power");
	m_parameters[U_LIGHT2_KC] = glGetUniformLocation(m_programID, "lights[2].kC");
	m_parameters[U_LIGHT2_KL] = glGetUniformLocation(m_programID, "lights[2].kL");
	m_parameters[U_LIGHT2_KQ] = glGetUniformLocation(m_programID, "lights[2].kQ");
	m_parameters[U_LIGHT2_TYPE] = glGetUniformLocation(m_programID, "lights[2].type");
	m_parameters[U_LIGHT2_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[2].spotDirection");
	m_parameters[U_LIGHT2_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[2].cosCutoff");
	m_parameters[U_LIGHT2_COSINNER] = glGetUniformLocation(m_programID, "lights[2].cosInner");
	m_parameters[U_LIGHT2_EXPONENT] = glGetUniformLocation(m_programID, "lights[2].exponent");

	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");

	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");
	glUseProgram(m_programID);

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	light[0].type = Light::LIGHT_DIRECTIONAL;
	light[0].position.Set(-100, 10, 0);
	light[0].color.Set(1, 1, 1);
	light[0].power = 1;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);

	light[1].type = Light::LIGHT_SPOT;
	light[1].position.Set(49, 10, -22);
	light[1].color.Set(1, 1, 1);
	light[1].power = 1;
	light[1].kC = 1.f;
	light[1].kL = 0.01f;
	light[1].kQ = 0.001f;
	light[1].cosCutoff = cos(Math::DegreeToRadian(45));
	light[1].cosInner = cos(Math::DegreeToRadian(30));
	light[1].exponent = 3.f;
	light[1].spotDirection.Set(0.f, 1.f, 0.f);

	light[2].type = Light::LIGHT_POINT;
	light[2].position.Set(-130, 5, -100);
	light[2].color.Set(0, 0, 1);
	light[2].power = 1;
	light[2].kC = 1.f;
	light[2].kL = 0.01f;
	light[2].kQ = 0.001f;
	light[2].cosCutoff = cos(Math::DegreeToRadian(45));
	light[2].cosInner = cos(Math::DegreeToRadian(30));
	light[2].exponent = 3.f;
	light[2].spotDirection.Set(0.f, 1.f, 0.f);

	glUniform1i(m_parameters[U_NUMLIGHTS], 3);

	// Make sure you pass uniform parameters after glUseProgram()
	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);

	glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
	glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
	glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
	glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
	glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
	glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], light[1].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT1_COSINNER], light[1].cosInner);
	glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponent);

	glUniform1i(m_parameters[U_LIGHT2_TYPE], light[2].type);
	glUniform3fv(m_parameters[U_LIGHT2_COLOR], 1, &light[2].color.r);
	glUniform1f(m_parameters[U_LIGHT2_POWER], light[2].power);
	glUniform1f(m_parameters[U_LIGHT2_KC], light[2].kC);
	glUniform1f(m_parameters[U_LIGHT2_KL], light[2].kL);
	glUniform1f(m_parameters[U_LIGHT2_KQ], light[2].kQ);
	glUniform1f(m_parameters[U_LIGHT2_COSCUTOFF], light[2].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT2_COSINNER], light[2].cosInner);
	glUniform1f(m_parameters[U_LIGHT2_EXPONENT], light[2].exponent);

	for (int i = 0; i < NUM_GEOMETRY; i++)
		meshList[i] = NULL;

	//PROTOTYPE FOR CUSTOM MESH
	/*
	meshList[ENUM] = MeshBuilder::GenerateOBJ("reference", "OBJ//filename.obj");
	meshList[ENUM]->textureID = LoadTGA("Image//filename.tga");
	*/

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//GothamMedium.tga");

	meshList[GEO_LIGHT1] = MeshBuilder::GenerateSphere("light", Color(1.f, 1.f, 1.f), 18, 36, 1.f);

	meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(1.f, 1.f, 1.f));
	meshList[GEO_QUAD]->textureID = LoadTGA("Image//pavement.tga");
	meshList[GEO_QUAD]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_QUAD]->material.kDiffuse.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_QUAD]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_QUAD]->material.kShininess = 1.0f;

	meshList[GEO_EXCLAIM] = MeshBuilder::GenerateQuad("exclaimation", Color(1.f, 1.f, 1.f));
	meshList[GEO_EXCLAIM]->textureID = LoadTGA("Image//exclaim.tga");
	meshList[GEO_HEALTH] = MeshBuilder::GenerateQuad("health", Color(1.f, 1.f, 1.f));
	meshList[GEO_HEALTH]->textureID = LoadTGA("Image//health.tga");
	meshList[GEO_AMMO] = MeshBuilder::GenerateQuad("ammo", Color(1.f, 1.f, 1.f));
	meshList[GEO_AMMO]->textureID = LoadTGA("Image//ammo.tga");
	meshList[GEO_BEER] = MeshBuilder::GenerateQuad("beer", Color(1.f, 1.f, 1.f));
	meshList[GEO_BEER]->textureID = LoadTGA("Image//beer.tga");
	meshList[GEO_RESOURCE] = MeshBuilder::GenerateQuad("resource", Color(1.f, 1.f, 1.f));
	meshList[GEO_RESOURCE]->textureID = LoadTGA("Image//resource.tga");
	meshList[GEO_LASERGUN] = MeshBuilder::GenerateQuad("lasergun", Color(1.f, 1.f, 1.f));
	meshList[GEO_LASERGUN]->textureID = LoadTGA("Image//lasergun.tga");
	meshList[GEO_FISTS] = MeshBuilder::GenerateQuad("fists", Color(1.f, 1.f, 1.f));
	meshList[GEO_FISTS]->textureID = LoadTGA("Image//fists.tga");
	meshList[GEO_DAMAGE] = MeshBuilder::GenerateQuad("damage", Color(1.f, 1.f, 1.f));
	meshList[GEO_DAMAGE]->textureID = LoadTGA("Image//damage.tga");

	meshList[GEO_WATERQUAD1] = MeshBuilder::GenerateQuad("waterquad1", Color(1.f, 1.f, 1.f));
	meshList[GEO_WATERQUAD1]->textureID = LoadTGA("Image//water1.tga");
	meshList[GEO_WATERQUAD1]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_WATERQUAD1]->material.kDiffuse.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_WATERQUAD1]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_WATERQUAD1]->material.kShininess = 1.0f;

	meshList[GEO_WATERQUAD2] = MeshBuilder::GenerateQuad("waterquad2", Color(1.f, 1.f, 1.f));
	meshList[GEO_WATERQUAD2]->textureID = LoadTGA("Image//water2.tga");
	meshList[GEO_WATERQUAD2]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_WATERQUAD2]->material.kDiffuse.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_WATERQUAD2]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_WATERQUAD2]->material.kShininess = 1.0f;

	meshList[GEO_WATERQUAD3] = MeshBuilder::GenerateQuad("waterquad3", Color(1.f, 1.f, 1.f));
	meshList[GEO_WATERQUAD3]->textureID = LoadTGA("Image//water3.tga");
	meshList[GEO_WATERQUAD3]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_WATERQUAD3]->material.kDiffuse.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_WATERQUAD3]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_WATERQUAD3]->material.kShininess = 1.0f;

	meshList[GEO_CIRCLE] = MeshBuilder::GenerateCircle("circle", Color(0.412f, 0.412f, 0.412f), 10, 3.f);
	meshList[GEO_CIRCLE]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_CIRCLE]->material.kDiffuse.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_CIRCLE]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_CIRCLE]->material.kShininess = 1.0f;

	meshList[GEO_PROD] = MeshBuilder::GenerateCylinder("prod", Color(0.412f, 0.412f, 0.412f), 10, 36, 1.f, 1.f);
	meshList[GEO_PROD]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_PROD]->material.kDiffuse.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_PROD]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_PROD]->material.kShininess = 1.0f;

	//----- Skybox -----//
	meshList[SKYBOX_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1));
	meshList[SKYBOX_FRONT]->textureID = LoadTGA("Image//front.tga");
	meshList[SKYBOX_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1));
	meshList[SKYBOX_BACK]->textureID = LoadTGA("Image//back.tga");
	meshList[SKYBOX_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1));
	meshList[SKYBOX_LEFT]->textureID = LoadTGA("Image//left.tga");
	meshList[SKYBOX_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1));
	meshList[SKYBOX_RIGHT]->textureID = LoadTGA("Image//right.tga");
	meshList[SKYBOX_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1));
	meshList[SKYBOX_TOP]->textureID = LoadTGA("Image//top.tga");
	meshList[SKYBOX_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1));
	meshList[SKYBOX_BOTTOM]->textureID = LoadTGA("Image//bottom.tga");
	//------------------//

	//----ROADS----//
	meshList[GEO_ROAD] = MeshBuilder::GenerateQuad("road", Color(1, 1, 1));
	meshList[GEO_ROAD]->textureID = LoadTGA("Image//road.tga");
	meshList[GEO_ROAD]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_ROAD]->material.kDiffuse.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_ROAD]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_ROAD]->material.kShininess = 1.0f;
	meshList[GEO_CROSSROAD] = MeshBuilder::GenerateQuad("crossroad", Color(1, 1, 1));
	meshList[GEO_CROSSROAD]->textureID = LoadTGA("Image//crossroad.tga");
	meshList[GEO_CROSSROAD]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_CROSSROAD]->material.kDiffuse.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_CROSSROAD]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_CROSSROAD]->material.kShininess = 1.0f;
	//----ROADS----//

	//----BUILDINGS----//
	meshList[BUILDING_SAFEHOUSE] = MeshBuilder::GenerateOBJwithTexture("safehouse", "safehouse.obj", "safehouse.tga");
	meshList[BUILDING_BULLETTOWER] = MeshBuilder::GenerateOBJwithTexture("bullettower", "BulletTower.obj", "building02.tga");
	meshList[BUILDING_JIGSAWTOWER] = MeshBuilder::GenerateOBJwithTexture("jigsawtower", "JigsawTower.obj", "building06.tga");
	meshList[BUILDING_ELEVATEDJIGSAW] = MeshBuilder::GenerateOBJwithTexture("jigsawelevated", "JigsawTowerElevated.obj", "building05.tga");
	meshList[BUILDING_POINTEDTOWER] = MeshBuilder::GenerateOBJwithTexture("pointedtower", "PointedTower.obj", "building01.tga");
	meshList[BUILDING_SINGLECAPPED] = MeshBuilder::GenerateOBJwithTexture("singlecapped", "SingleCappedTower.obj", "building03.tga");
	meshList[BUILDING_TRIPLECAPPEDPOINTED] = MeshBuilder::GenerateOBJwithTexture("triplecappedpointed", "TripleCappedPointedTower.obj", "building06.tga");
	meshList[BUILDING_TRIPLETOWER] = MeshBuilder::GenerateOBJwithTexture("tripletower", "TripleTower.obj", "building01.tga");
	meshList[BUILDING_CLOVER] = MeshBuilder::GenerateOBJwithTexture("clovertower", "cloverbuilding.obj", "cloverbuilding.tga");
	meshList[GEO_VALVE] = MeshBuilder::GenerateOBJwithTexture("valvebuilding", "valvebuilding.obj", "valvebuilding.tga");
	//----BUILDINGS----//

	//----PROPS----//
	meshList[GEO_BARREL] = MeshBuilder::GenerateOBJwithTexture("barrel", "barrel.obj", "barrel.tga");
	meshList[GEO_CRATE] = MeshBuilder::GenerateOBJwithTexture("crate", "crate.obj", "boxes.tga");
	meshList[GEO_CRATESTACK] = MeshBuilder::GenerateOBJwithTexture("cratestack", "cratestack.obj", "boxes.tga");
	meshList[GEO_CAR] = MeshBuilder::GenerateOBJwithTexture("car", "flyingcar.obj", "flyingcar.tga");
	//----PROPS----//

	//----NPCS----//
	meshList[NPC_NEPHILIM] = MeshBuilder::GenerateOBJwithTexture("nephilim", "nephilim.obj", "nephilim.tga");
	meshList[NPC_MORGANSUIT] = MeshBuilder::GenerateOBJwithTexture("suit", "MorganSuit.obj", "MorganSuit.tga");
	meshList[NPC_MORGANTIE] = MeshBuilder::GenerateOBJwithTexture("tie", "MorganTie.obj", "MorganTie.tga");
	meshList[NPC_MORGANHEAD] = MeshBuilder::GenerateOBJwithTexture("head", "MorganHead.obj", "MorganHead.tga");
	meshList[NPC_LILITHBODY] = MeshBuilder::GenerateOBJwithTexture("lilithbody", "LilithBody.obj", "LilithBody.tga");
	meshList[NPC_LILITHHEAD] = MeshBuilder::GenerateOBJwithTexture("lilithhead", "LilithHead.obj", "LilithHead.tga");
	//----NPCS----//

	//Temporary Water Placement
	WaterBoxes.push_back(new Water(WaterBoxes.size(), new Vector3(-130, 5, -100)));

	meshList[GEO_ORE] = MeshBuilder::GenerateOBJwithTexture("Ore", "Ore00.obj", "Text_Gold.tga");
	OreBoxes.push_back(new Ore(OreBoxes.size(), new Vector3(-53.f, 1.f, -27.f)));
	OreBoxes.push_back(new Ore(OreBoxes.size(), new Vector3(77.f, 1.f, 104.f)));
	OreBoxes.push_back(new Ore(OreBoxes.size(), new Vector3(92.f, 1.f, 23.f)));
	OreBoxes.push_back(new Ore(OreBoxes.size(), new Vector3(-162.f, 1.f, 111.f)));
	OreBoxes.push_back(new Ore(OreBoxes.size(), new Vector3(-53.f, 1.f, -58.f)));

	meshList[GEO_LASER] = MeshBuilder::GenerateCylinder("Lazer", Color(1, 0, 0));
	meshList[GEO_LASER]->material.kAmbient.Set(0.3f, 0.3f, 0.3f);
	meshList[GEO_LASER]->material.kDiffuse.Set(0.7f, 0.7f, 0.7f);
	meshList[GEO_LASER]->material.kSpecular.Set(0.1f, 0.1f, 0.1f);
	meshList[GEO_LASER]->material.kShininess = 1.0f;

	Spawner = new Spawns(10);

	GenerateMonsters();

	NPCs_.push_back(new Ally(NPCs_.size(), new Vector3(60, 0, 25), &Resource));
	NPCs_.push_back(new Ally(NPCs_.size(), new Vector3(-50, 0, 44), &KDA));
	NPCs_.push_back(new Ally(NPCs_.size(), new Vector3(-153, 0, -123), Player::instance()->questBeer()));
	NPCs_.push_back(new Ally(NPCs_.size(), new Vector3(0, 0, 0)));

	Mtx44 projection;
	projection.SetToPerspective(45.f, 16.f / 9.f, 0.1f, 2000.f);
	projectionStack.LoadMatrix(projection);

	std::vector<Hitbox**> eHitbox;
	for (unsigned int k = 0; k < Enemies_.size(); k++)
		eHitbox.push_back(Enemies_[k]->getHitBox());
	Building::inst()->GenerateBuilding();
	ReadCharacterWidth("Image//FontData.csv");
	camera.Init(Vector3(50, 2, -21), Vector3(50, 2, -16), Vector3(0, 1, 0), eHitbox, false, false); //y-axis of position and target should always be the same 5, 2, 5
	Player::instance()->Init(&camera.position, &camera.aim, &Enemies_);
	//-- Initialise this AFTER all Buildings, NPCs/Enemies and Player are initialised --//
	Collision::inst()->Init(&camera.position, &NPCs_, &OreBoxes, &WaterBoxes);
	//----------------------------------------------------------------------------------//

	//-- Initialise Variables --//
	bLightEnabled = true;
	Resource = 0;
	KDA = 0;
	Message = "";
	WaterLimit = 0;
	showFPS = false;

	for (unsigned int i = 0; i < 256; i++)
		Keybounce[i] = false;

	State.TempLoad(NPCs_, Player::instance(), Resource);
}

void SP2_Scene01::Update(double dt)
{
	frameRate = "FPS: " + std::to_string(1 / dt);
	WaterLimit += (float)(.9f * dt);

	for (unsigned int i = 0; i < NPCs_.size(); i++)
	{
		if (i != NPCs_.size() - 1)
		{
			if (NPCs_[i]->isQuestActive() && !NPCs_[i + 1]->isQuestActive() && Story == "PROGRESS")
			{
				NPCs_[i]->getQuestMessage(Message);
				break;
			}
		}
		else if (NPCs_[i]->isQuestActive() && Story == "PROGRESS")
		{
			NPCs_[i]->getQuestMessage(Message);
		}
		else
		{
			Message = "";
		}
	}
	for (unsigned int i = 0; i < NPCs_.size(); i++)
	{
		if (NPCs_[i]->isQuestActive())
		{
			NPCs_[i]->getStory(Story);
			if (Story != "" && Story != "PROGRESS")
				Story += "        'Press K to continue'";
		}
	}
	for (unsigned int i = 0; i < WaterBoxes.size(); i++)
	{
		WaterBoxes[i]->Update(dt);
		if (WaterBoxes[i]->isActive())
		{
			Player::instance()->gainHealth(WaterBoxes[i]->Healing());
			break;
		}
	}
	camera.Update(dt);
	Player::instance()->Update(dt);

	if (Application::IsKeyPressed(VK_1))
		glEnable(GL_CULL_FACE);
	else if (Application::IsKeyPressed(VK_2))
		glDisable(GL_CULL_FACE);

	if (Application::IsKeyPressed(VK_3))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (Application::IsKeyPressed(VK_4))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (Application::IsKeyPressed(VK_9))
		bLightEnabled = false;
	else if (Application::IsKeyPressed(VK_0))
		bLightEnabled = true;

	Collision::inst()->Update(dt);

	// Run Monsters
	for (unsigned int i = 0; i < Enemies_.size(); i++)
	{
		Enemies_[i]->Update(dt, Player::instance()->getPlayerPosition());
		if (!Enemies_[i]->isActive())
		{
			KDA++;
			Resource += 20;
			Enemies_[i]->reviveEnemy();
			Enemies_[i]->SetStartingPos(Spawner->SpawnLocation(Enemies_[i]->GrabPosition()));
			Enemies_[i]->AddWaypoints(Vector3(Enemies_[i]->GrabPosition()->x + 10, Enemies_[i]->GrabPosition()->y, Enemies_[i]->GrabPosition()->z + 10));
		}
	}

	// Quest System
	if (Application::IsKeyPressed('K'))
	{
		if (!Keybounce['K' - 1])
		{
			for (unsigned int i = 0; i < NPCs_.size(); i++)
			{
				if (i != NPCs_.size() - 1)
				{
					if (NPCs_[i]->isQuestActive() && !NPCs_[i + 1]->isQuestActive() && (Story != "PROGRESS" && Story != ""))
					{
						NPCs_[i]->progressStory();
						break;
					}
				}
				else if (NPCs_[i]->isQuestActive() && Story != "PROGRESS" && Story != "")
				{
					NPCs_[i]->progressStory();
				}
			}
		}
		Keybounce['K' - 1] = true;
	}
	else
	{
		Keybounce['K' - 1] = false;
	}

	if (Application::IsKeyPressed('Z'))  //Weapon Switching
	{
		if (!Keybounce['Z' - 1])
		{
			if (Player::instance()->getWeap() == "Fist")
			{
				if (Player::instance()->HaveGun)
				{
					Player::instance()->WeapState(Player::WEAP_GUN);
				}
			}
			else if (Player::instance()->getWeap() == "Gun")
			{
				Player::instance()->WeapState(Player::WEAP_FIST);
			}
		}

		Keybounce['Z' - 1] = true;
	}
	else
	{
		Keybounce['Z' - 1] = false;
	}

	if (Application::IsKeyPressed('C'))
	{
		if (!Keybounce['C' - 1])
		{
			Player::instance()->UseBeer();
		}
		Keybounce['C' - 1] = true;
	}
	else
	{
		Keybounce['C' - 1] = false;
	}

	if (Application::IsKeyPressed('8'))
	{
		if (!Keybounce['8' - 1])
		{
			if (showFPS == true)
			{
				showFPS = false;
			}
			else
			{
				showFPS = true;
			}
		}

		Keybounce['8' - 1] = true;
	}
	else
	{
		Keybounce['8' - 1] = false;
	}

	if (Application::IsKeyPressed('E'))
	{
		if (!Keybounce['E' - 1])
		{
			for (unsigned int i = 0; i < OreBoxes.size(); i++)
			{
				if (Collision::inst()->CheckOre() == i)
				{
					Resource += OreBoxes[i]->Mining();
					break;
				}
			}
			for (unsigned int i = 0; i < WaterBoxes.size(); i++)
			{
				if (Collision::inst()->CheckWater() == i)
				{
					WaterBoxes[0]->Trigger();
					Player::instance()->SetHealth((float)(int)Player::instance()->getCurHP());
					break;
				}
			}
			for (unsigned int i = 0; i < NPCs_.size(); i++)
			{
				if (i > 0)
				{
					if ((NPCs_[i - 1]->isQuestActive() && NPCs_[i - 1]->checkObjectives() && Story == "PROGRESS" && i - 1 == Collision::inst()->CheckAlly()))
					{
						if (i == 1)
						{
							NPCs_[i - 1]->progressStory();
							Resource -= 10;
							Player::instance()->receiveAmmo(50);
							Player::instance()->HaveGun = true;
						}

						else if (i == 2)
						{
							NPCs_[i - 1]->progressStory();
							Player::instance()->AddBeer();
							Resource += 200;
						}
						else if (i == 3)
						{
							NPCs_[i - 1]->progressStory();
							Player::instance()->receiveAmmo(300);
							Resource += 500;
						}
						break;
					}
					else if (NPCs_[i - 1]->isQuestActive() && Story == "" && i == Collision::inst()->CheckAlly())
					{
						//NPCs_[i]->progressStory();
						NPCs_[i]->setQuest(true);
						break;
					}
				}
				else if (!NPCs_[0]->isQuestActive() && Collision::inst()->CheckAlly() == 0)
				{
					NPCs_[0]->setQuest(true);
					break;
				}
			}
			if (Collision::inst()->CheckSafeHouseDoor())
			{
				State.TempSave(NPCs_, Player::instance(), Resource);
				SceneManager::instance()->SetFile(0);
				SceneManager::instance()->SwitchScene(SceneManager::SCENE_SAFEHOUSE);
			}
		}
		Keybounce['E' - 1] = true;
	}
	else
	{
		Keybounce['E' - 1] = false;
	}
	if (Player::instance()->getCurHP() <= 0.f)
	{
		SceneManager::instance()->SetFile(1);
		SceneManager::instance()->SwitchScene(SceneManager::SCENE_SAFEHOUSE);
	}
}

void SP2_Scene01::Render()
{
	// Render VBO here
	Mtx44 MVP;

	// Clear color & depth buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z);

	modelStack.LoadIdentity();

	RenderLight(light[0], U_LIGHT0_POSITION, U_LIGHT0_SPOTDIRECTION);
	RenderLight(light[1], U_LIGHT1_POSITION, U_LIGHT1_SPOTDIRECTION);
	RenderLight(light[2], U_LIGHT2_POSITION, U_LIGHT2_SPOTDIRECTION);

	modelStack.PushMatrix();
	modelStack.Rotate(-90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(1000.f, 1000.f, 1000.f);
	RenderMesh(meshList[GEO_QUAD], true);
	modelStack.PopMatrix();

	RenderSkybox();
	RenderMonsters();
	RenderRoads();
	RenderBuildings();
	RenderNPC();
	RenderProps();
	RenderWater();

	modelStack.PushMatrix();
	modelStack.Translate(49, 12, -22);
	RenderMesh(meshList[GEO_LIGHT1], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(49, 12.5f, -22);
	modelStack.Rotate(180, 0, 0, 1);
	RenderMesh(meshList[GEO_CIRCLE], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(49, 9.5f, -27);
	modelStack.Rotate(225, 1, 0, 0);
	modelStack.Scale(0.3f, 8, 0.3f);
	RenderMesh(meshList[GEO_PROD], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //Fountain
	modelStack.Translate(-130, 0, -100);
	RenderMesh(meshList[GEO_VALVE], true);
	modelStack.PopMatrix(); //-125 -95

	if (Player::instance()->Shooting())
	{
		modelStack.PushMatrix();
		modelStack.Translate(Player::instance()->getPlayerPosition().x, Player::instance()->getPlayerPosition().y - .1f, Player::instance()->getPlayerPosition().z);
		modelStack.Rotate(Player::instance()->getAngle() + 90.f, 0.f, 1.f, 0.f);
		modelStack.Rotate(Player::instance()->getAngleY(), 1.f, 0.f, 0.f);
		modelStack.Scale(.01f, 100.f, .01f);
		RenderMesh(meshList[GEO_LASER], false);
		modelStack.PopMatrix();
	}
	RenderUI();

	// Testing lines //
	//RenderTextOnScreen(meshList[GEO_TEXT], frameRate, Color(0, 1, 0), 0.3f, 0.5f, 6.f);
	//RenderTextOnScreen(meshList[GEO_TEXT], "X: " + std::to_string((int)camera.position.x), Color(0, 1, 0), 0.5f, 0.5f, 4.f);
	//RenderTextOnScreen(meshList[GEO_TEXT], "Z: " + std::to_string((int)camera.position.z), Color(0, 1, 0), 0.5f, 0.5f, 3.5f);
	RenderTextOnScreen(meshList[GEO_TEXT], Story, Color(0, 1, 0), 0.3f, 0.5f, 3.f);
	//RenderTextOnScreen(meshList[GEO_TEXT], Player::instance()->getWeap(), Color(0, 1, 0), 0.5f, 3.5f, 2.5f);
	RenderTextOnScreen(meshList[GEO_TEXT], Message, Color(0, 1, 0), 0.5f, 0.5f, 1.5f);
	//RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(*Requirements[1]), Color(0, 1, 0), 0.5f, 0.5f, 0.5f);

	SceneManager::instance()->State(SceneManager::SCENE_GAME);
}

void SP2_Scene01::RenderMesh(Mesh *mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight && bLightEnabled)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}

	if (mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}

	mesh->Render();

	if (mesh->textureID > 0)
		glBindTexture(GL_TEXTURE_2D, 0);
}

void SP2_Scene01::RenderSkybox()
{
	modelStack.PushMatrix();
	modelStack.Translate(camera.position.x, camera.position.y, camera.position.z);

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.f, -499.f);
	modelStack.Scale(1000.f, 1000.f, 1000.f);
	RenderMesh(meshList[SKYBOX_FRONT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.f, 499.f);
	modelStack.Rotate(180.f, 0.f, 1.f, 0.f);
	modelStack.Scale(1000.f, 1000.f, 1000.f);
	RenderMesh(meshList[SKYBOX_BACK], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-499.f, 0.f, 0.f);
	modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
	modelStack.Scale(1000.f, 1000.f, 1000.f);
	RenderMesh(meshList[SKYBOX_LEFT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(499.f, 0.f, 0.f);
	modelStack.Rotate(-90.f, 0.f, 1.f, 0.f);
	modelStack.Scale(1000.f, 1000.f, 1000.f);
	RenderMesh(meshList[SKYBOX_RIGHT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 499.f, 0.f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(1000.f, 1000.f, 1000.f);
	RenderMesh(meshList[SKYBOX_TOP], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, -499.f, 0.f);
	modelStack.Rotate(-90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(1000.f, 1000.f, 1000.f);
	RenderMesh(meshList[SKYBOX_BOTTOM], false);
	modelStack.PopMatrix();

	modelStack.PopMatrix();
}

void SP2_Scene01::RenderWater()
{
	//Water 1
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-129.f, 3, -97);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-129.f, 3, -97);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-129.f, 3, -97);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}

	//Water 1.5
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-129.f, 3, -97);
		modelStack.Rotate(270, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-129.f, 3, -97);
		modelStack.Rotate(270, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-129.f, 3, -97);
		modelStack.Rotate(270, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}

	//Water 2
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-129.f, 3, -103);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-129.f, 3, -103);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-129.f, 3, -103);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}

	//Water 2.5
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-129.f, 3, -103);
		modelStack.Rotate(270, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-129.f, 3, -103);
		modelStack.Rotate(270, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-129.f, 3, -103);
		modelStack.Rotate(270, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}

	//Water 3
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-127.f, 3, -99);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-127.f, 3, -99);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-127.f, 3, -99);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}

	//Water 3.5
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-127.f, 3, -99);
		modelStack.Rotate(180, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-127.f, 3, -99);
		modelStack.Rotate(180, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-127.f, 3, -99);
		modelStack.Rotate(180, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}

	//Water 4
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-128.f, 3, -101);
		modelStack.Rotate(180, 0, 1, 0);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-128.f, 3, -101);
		modelStack.Rotate(180, 0, 1, 0);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-128.f, 3, -101);
		modelStack.Rotate(180, 0, 1, 0);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}

	//Water 4.5
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-127.f, 3, -101);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-127.f, 3, -101);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-127.f, 3, -101);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}

	//Water 5
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-131.f, 3, -103);
		modelStack.Rotate(270, 0, 1, 0);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-131.f, 3, -103);
		modelStack.Rotate(270, 0, 1, 0);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-131.f, 3, -103);
		modelStack.Rotate(270, 0, 1, 0);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}

	//Water 5.5
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-131.f, 3, -103);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-131.f, 3, -103);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-131.f, 3, -103);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}

	//Water 6
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-133.f, 3, -99);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-133.f, 3, -99);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-133.f, 3, -99);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}

	//Water 6.5
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-133.f, 3, -99);
		modelStack.Rotate(180, 0, 1, 0);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-133.f, 3, -99);
		modelStack.Rotate(180, 0, 1, 0);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-133.f, 3, -99);
		modelStack.Rotate(180, 0, 1, 0);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}

	//Water 7
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-133.f, 3, -101);
		modelStack.Rotate(180, 0, 1, 0);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-133.f, 3, -101);
		modelStack.Rotate(180, 0, 1, 0);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-133.f, 3, -101);
		modelStack.Rotate(180, 0, 1, 0);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}

	//Water 7.5
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-133.f, 3, -101);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-133.f, 3, -101);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-133.f, 3, -101);
		modelStack.Scale(4.f, 9.f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}

	//Water 8
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-131.f, 3, -97);
		modelStack.Rotate(270, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-131.f, 3, -97);
		modelStack.Rotate(270, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-131.f, 3, -97);
		modelStack.Rotate(270, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}

	//Water 8.5
	if (WaterLimit >= 0 && WaterLimit < 1.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-131.f, 3, -97);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD1], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 1.f && WaterLimit < 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-131.f, 3, -97);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD2], true);
		modelStack.PopMatrix();
	}

	if (WaterLimit >= 2.f)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-131.f, 3, -97);
		modelStack.Rotate(90, 0, 1, 0);
		modelStack.Scale(4.f, 8.8f, 1);
		RenderMesh(meshList[GEO_WATERQUAD3], true);
		modelStack.PopMatrix();

		WaterLimit = 0;
	}
}

void SP2_Scene01::RenderUI()
{
	if (Player::instance()->PlayerisHit)
	{
		RenderMeshOnScreen(meshList[GEO_DAMAGE], 8.1f, 4.5f, 20.f, 10.f, 0.f);

		Player::instance()->PlayerisHit = 0;
	}

	if (Player::instance()->getWeap() == "Fist")
	{
		RenderMeshOnScreen(meshList[GEO_FISTS], 15.f, 4.5f, 1.f, 1.f, 0.f);
	}
	else if (Player::instance()->getWeap() == "Gun")
	{
		RenderMeshOnScreen(meshList[GEO_LASERGUN], 14.5f, 4.5f, 3.f, 1.f, 0.f);
	}
	for (unsigned int i = 0; i < NPCs_.size(); i++)
	{
		if (Collision::inst()->CheckAlly() == i && !NPCs_[i]->isQuestActive())
			RenderTextOnScreen(meshList[GEO_TEXT], "Press E to Interact", Color(0, 1, 0), 0.5f, 0.5f, 4.f);
		else if (Collision::inst()->CheckAlly() == i && Story == "PROGRESS")
			RenderTextOnScreen(meshList[GEO_TEXT], "Press E to Submit", Color(0, 1, 0), 0.5f, 0.5f, 4.f);
	}
	for (unsigned int i = 0; i < WaterBoxes.size(); i++)
	{
		if (WaterBoxes[i]->isActive())
			RenderTextOnScreen(meshList[GEO_TEXT], "Healing in Progress", Color(0, 1, 0), 0.5f, 0.5f, 4.f);
		else if (Collision::inst()->CheckWater() == i && WaterBoxes[i]->getCooldown() > 0)
			RenderTextOnScreen(meshList[GEO_TEXT], "Cooldown: " + std::to_string(WaterBoxes[i]->getCooldown()), Color(0, 1, 0), 0.5f, 0.5f, 4.f);
		else if (Collision::inst()->CheckWater() == i)
			RenderTextOnScreen(meshList[GEO_TEXT], "Press E to heal", Color(0, 1, 0), 0.5f, 0.5f, 4.f);
	}
	for (unsigned int i = 0; i < OreBoxes.size(); i++)
	{
		if (Collision::inst()->CheckOre() == i)
			RenderTextOnScreen(meshList[GEO_TEXT], "Press E to mine", Color(0, 1, 0), 0.5f, 0.5f, 4.f);
	}
	if (Collision::inst()->CheckSafeHouseDoor())
		RenderTextOnScreen(meshList[GEO_TEXT], "Press E to enter", Color(0, 1, 0), 0.5f, 0.5f, 4.f);

	for (unsigned int i = 0; i < Enemies_.size(); i++)
	{
		if (Enemies_[i]->isActive() && (Enemies_[i]->getDistance(camera.position) < 50.f || Enemies_[i]->getHealth() < 100))
		{
			modelStack.PushMatrix();
			modelStack.Translate(Enemies_[i]->getPosition().x, 4.f, Enemies_[i]->getPosition().z);
			modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.z - camera.target.z, camera.target.x - camera.position.x)) - 90, 0, 1, 0);
			RenderText(meshList[GEO_TEXT], Enemies_[i]->CurrentHealth(), Color(0.f, 1.f, 0.f));
			modelStack.PopMatrix();
		}
	}
	if (Player::instance()->getCurHP() < 30.f && Player::instance()->getBeer() > 0)
		RenderTextOnScreen(meshList[GEO_TEXT], "Press C to Heal", Color(0, 1, 0), 0.5f, 0.5f, 4.5f);

	RenderMeshOnScreen(meshList[GEO_HEALTH], 7.5f, 0.4f, .4f, .4f, 0.f);
	RenderMeshOnScreen(meshList[GEO_AMMO], 9.5f, 0.5f, .7f, .7f, 0.f);
	RenderMeshOnScreen(meshList[GEO_BEER], 11.5f, 0.4f, .5f, .5f, 0.f);
	RenderMeshOnScreen(meshList[GEO_RESOURCE], 13.7f, 0.4f, .5f, .5f, 0.f);

	RenderTextOnScreen(meshList[GEO_TEXT], "X", Color(0, 1, 0), 0.6f, 12.2f, .1f); //Beer
	RenderTextOnScreen(meshList[GEO_TEXT], "X", Color(0, 1, 0), 0.6f, 14.4f, .1f); //Resource

	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string((int)Player::instance()->getCurHP()), Color(0, 1, 0), 0.5f, 8.2f, .15f); //Health Value  
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string((int)Player::instance()->getAmmo()), Color(0, 1, 0), 0.5f, 10.3f, .15f);  //Ammo Value 
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(Resource), Color(0, 1, 0), 0.5f, 14.8f, .15f);  //Resource Value
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string((int)Player::instance()->getBeer()), Color(0, 1, 0), 0.5f, 12.6f, .15f);  //Beer

	RenderTextOnScreen(meshList[GEO_TEXT], "+", Color(0, 1, 0), 0.5f, 8.12f, 4.25f); //Crosshair

	if (showFPS == true)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], frameRate, Color(0, 1, 0), 0.5f, 0.5f, 8.5f);
	}
}

void SP2_Scene01::RenderProps()
{
	modelStack.PushMatrix();
	modelStack.Translate(-176, 0, 195);
	RenderMesh(meshList[GEO_CRATESTACK], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-38, 0, 187);
	RenderMesh(meshList[GEO_CRATE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(55, 0, -35);
	RenderMesh(meshList[GEO_CRATE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-163, 0, 182);
	RenderMesh(meshList[GEO_BARREL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-163, 0, 179);
	RenderMesh(meshList[GEO_BARREL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-11, 0.f, -77);
	RenderMesh(meshList[GEO_CAR], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(34, 0.f, 11);
	modelStack.Rotate(-90, 0, 1, 0);
	RenderMesh(meshList[GEO_CAR], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-10, 0.f, 62);
	modelStack.Rotate(180, 0, 1, 0);
	RenderMesh(meshList[GEO_CAR], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-95, 0.f, -9);
	modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[GEO_CAR], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(163, 0.f, 12);
	modelStack.Rotate(-90, 0, 1, 0);
	RenderMesh(meshList[GEO_CAR], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-53, 0, -27);
	RenderMesh(meshList[GEO_ORE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(77, 0, 104);
	RenderMesh(meshList[GEO_ORE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(92, 0, 23);
	RenderMesh(meshList[GEO_ORE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-162, 0, 111);
	RenderMesh(meshList[GEO_ORE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-53, 0, -58);
	RenderMesh(meshList[GEO_ORE], true);
	modelStack.PopMatrix();
}

void SP2_Scene01::RenderNPC()
{
	//NEPHILIM//
	modelStack.PushMatrix();
	modelStack.Translate(60, 0, 25);
	modelStack.Rotate(180, 0, 1, 0);
	RenderMesh(meshList[NPC_NEPHILIM], true);
	modelStack.PopMatrix();

	if (!NPCs_[0]->isQuestActive())
	{
		modelStack.PushMatrix();
		modelStack.Translate(59.9f, 8, 25);
		modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.z - camera.target.z, camera.target.x - camera.position.x)) - 90, 0, 1, 0);
		modelStack.Scale(3, 3, 3);
		RenderMesh(meshList[GEO_EXCLAIM], false);
		modelStack.PopMatrix();
	}
	//NEPHILIM//

	//MORGAN FREEMAN//
	modelStack.PushMatrix(); {
		modelStack.Translate(-153, 0, -123);

		modelStack.PushMatrix();
		RenderMesh(meshList[NPC_MORGANSUIT], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		RenderMesh(meshList[NPC_MORGANHEAD], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		RenderMesh(meshList[NPC_MORGANTIE], true);
		modelStack.PopMatrix();

	} modelStack.PopMatrix();
	if (!NPCs_[2]->isQuestActive() && NPCs_[1]->isQuestActive() && Story == "")
	{
		modelStack.PushMatrix();
		modelStack.Translate(-153, 4, -123.1f);
		modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.z - camera.target.z, camera.target.x - camera.position.x)) - 90, 0, 1, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_EXCLAIM], false);
		modelStack.PopMatrix();
	}
	//MORGAN FREEMAN//

	//LILITH//
	modelStack.PushMatrix(); {
		modelStack.Translate(-50, 0, 44);
		modelStack.Rotate(90, 0, 1, 0);

		modelStack.PushMatrix();
		RenderMesh(meshList[NPC_LILITHBODY], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		RenderMesh(meshList[NPC_LILITHHEAD], true);
		modelStack.PopMatrix();

	} modelStack.PopMatrix();
	if (!NPCs_[1]->isQuestActive() && NPCs_[0]->isQuestActive() && Story == "")
	{
		modelStack.PushMatrix();
		modelStack.Translate(-50.1f, 3.5f, 44.f);
		modelStack.Rotate(Math::RadianToDegree(atan2(camera.position.z - camera.target.z, camera.target.x - camera.position.x)) - 90, 0, 1, 0);
		modelStack.Scale(2, 2, 2);
		RenderMesh(meshList[GEO_EXCLAIM], false);
		modelStack.PopMatrix();
	}
	//LILITH//
}

void SP2_Scene01::RenderRoads()
{
	//ROADS//
	modelStack.PushMatrix();
	modelStack.Translate(0, 0.1f, 0);
	modelStack.Scale(40, 40, 40);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_CROSSROAD], true);
	modelStack.PopMatrix();

	//Bottom

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.1f, 70.f);
	modelStack.Scale(30.5f, 10, 100.f);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.1f, 170.f);
	modelStack.Scale(30.5f, 10, 100.f);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.1f, 270.f);
	modelStack.Scale(30.5f, 10, 100.f);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.1f, 370.f);
	modelStack.Scale(30.5f, 10, 100.f);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.1f, 470.f);
	modelStack.Scale(30.5f, 10, 100.f);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	//Top
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.1f, -70.f);
	modelStack.Scale(30.5f, 10, 100.f);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.1f, -170.f);
	modelStack.Scale(30.5f, 10, 100.f);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.1f, -270.f);
	modelStack.Scale(30.5f, 10, 100.f);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.1f, -370.f);
	modelStack.Scale(30.5f, 10, 100.f);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.1f, -470.f);
	modelStack.Scale(30.5f, 10, 100.f);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	//Right
	modelStack.PushMatrix();
	modelStack.Translate(70.f, 0.1f, 0.f);
	modelStack.Scale(100.f, 10, 30.5f);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(170.f, 0.1f, 0.f);
	modelStack.Scale(100.f, 10, 30.5f);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(270.f, 0.1f, 0.f);
	modelStack.Scale(100.f, 10, 30.5f);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(370.f, 0.1f, 0.f);
	modelStack.Scale(100.f, 10, 30.5f);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(470.f, 0.1f, 0.f);
	modelStack.Scale(100.f, 10, 30.5f);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	//Left
	modelStack.PushMatrix();
	modelStack.Translate(-70.f, 0.1f, 0.f);
	modelStack.Scale(100.f, 10, 30.5f);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-170.f, 0.1f, 0.f);
	modelStack.Scale(100.f, 10, 30.5f);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-270.f, 0.1f, 0.f);
	modelStack.Scale(100.f, 10, 30.5f);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-370.f, 0.1f, 0.f);
	modelStack.Scale(100.f, 10, 30.5f);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-470.f, 0.1f, 0.f);
	modelStack.Scale(100.f, 10, 30.5f);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_ROAD], true);
	modelStack.PopMatrix();

	//ROADS//
}

void SP2_Scene01::RenderBuildings()
{
	//BUILDINGS//

	modelStack.PushMatrix();
	modelStack.Translate(120, -1, -200);
	modelStack.Rotate(180, 0, 1, 0);
	RenderMesh(meshList[BUILDING_TRIPLETOWER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(205, -1, -110);
	modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[BUILDING_TRIPLETOWER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(200, -1, 120);
	modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[BUILDING_TRIPLETOWER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(50, 125, 233);
	RenderMesh(meshList[BUILDING_ELEVATEDJIGSAW], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-112, -1, 225);
	modelStack.Rotate(180, 0, 1, 0);
	RenderMesh(meshList[BUILDING_TRIPLETOWER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-214, -1, 105);
	modelStack.Rotate(100, 0, 1, 0);
	RenderMesh(meshList[BUILDING_TRIPLETOWER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-213, -1, -105);
	modelStack.Rotate(100, 0, 1, 0);
	RenderMesh(meshList[BUILDING_TRIPLETOWER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-110, -1, -180);
	modelStack.Rotate(180, 0, 1, 0);
	RenderMesh(meshList[BUILDING_TRIPLETOWER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(48, 0, -40);
	RenderMesh(meshList[BUILDING_SAFEHOUSE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(130, 65, -80);
	RenderMesh(meshList[BUILDING_JIGSAWTOWER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(60, 125, -150);
	RenderMesh(meshList[BUILDING_ELEVATEDJIGSAW], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(190, -5, -180);
	RenderMesh(meshList[BUILDING_POINTEDTOWER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(60, 10, 55);
	RenderMesh(meshList[BUILDING_BULLETTOWER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(120, 10, 55);
	RenderMesh(meshList[BUILDING_BULLETTOWER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(180, -5, 85);
	RenderMesh(meshList[BUILDING_SINGLECAPPED], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(120, -1, 150);
	RenderMesh(meshList[BUILDING_TRIPLETOWER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-160, -1, 60);
	RenderMesh(meshList[BUILDING_TRIPLECAPPEDPOINTED], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-90, -5, 45);
	RenderMesh(meshList[BUILDING_POINTEDTOWER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-110, -1, 110);
	modelStack.Rotate(45, 0, 1, 0);
	RenderMesh(meshList[BUILDING_TRIPLETOWER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-90, 65, 180);
	modelStack.Rotate(45, 0, 1, 0);
	RenderMesh(meshList[BUILDING_JIGSAWTOWER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-80, 0, -100);
	RenderMesh(meshList[BUILDING_CLOVER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-130, 0, -150);
	RenderMesh(meshList[BUILDING_CLOVER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-170, 0, -100);
	RenderMesh(meshList[BUILDING_CLOVER], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-130, 0, -50);
	RenderMesh(meshList[BUILDING_CLOVER], true);
	modelStack.PopMatrix();
}

void SP2_Scene01::RenderLight(Light light, UNIFORM_TYPE pos, UNIFORM_TYPE dir)
{
	if (light.type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light.position.x, light.position.y, light.position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[pos], 1, &lightDirection_cameraspace.x);
	}
	else if (light.type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light.position;
		glUniform3fv(m_parameters[pos], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light.spotDirection;
		glUniform3fv(m_parameters[dir], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light.position;
		glUniform3fv(m_parameters[pos], 1, &lightPosition_cameraspace.x);
	}
}

void SP2_Scene01::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	Mtx44 characterSpacing;
	float width = 0.f;

	for (unsigned i = 0; i < text.length(); ++i)
	{
		characterSpacing.SetToTranslation(width / 20.f, 0, Math::EPSILON * i * 300);
		width += iChWidth[text[i]];
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
}

void SP2_Scene01::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);

	Mtx44 ortho;
	ortho.SetToOrtho(0, 16, 0, 9, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	Mtx44 characterSpacing;
	float width = 0.f;

	for (unsigned i = 0; i < text.length(); ++i)
	{
		characterSpacing.SetToTranslation(width / 20.f, 0.5f, 0.f);
		width += iChWidth[text[i]];
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);

	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glEnable(GL_DEPTH_TEST);
}

void SP2_Scene01::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey, float rotate)
{
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 16, 0, 9, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	//to do: scale and translate accordingly
	modelStack.Translate(x, y, 0.1f);
	modelStack.Rotate(rotate, 0, 0, 1);
	modelStack.Scale(sizex, sizey, 1.f);
	RenderMesh(mesh, false); //UI should not have light
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SP2_Scene01::Exit()
{
	for (int i = 0; i < NUM_GEOMETRY; i++)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}

	for (unsigned int i = 0; i < Enemies_.size(); i++)
		delete Enemies_[i];
	for (unsigned int i = 0; i < NPCs_.size(); i++)
		delete NPCs_[i];
	for (unsigned int i = 0; i < WaterBoxes.size(); i++)
		delete WaterBoxes[i];
	for (unsigned int i = 0; i < OreBoxes.size(); i++)
		delete OreBoxes[i];

	delete Spawner;
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

// Everything related to Monsters/Enemies will be done here
void SP2_Scene01::GenerateMonsters()
{
	meshList[NPC_ALIEN] = MeshBuilder::GenerateOBJwithTexture("alien", "alien.obj", "alien.tga");

	for (unsigned int i = 0; i < Spawner->SpawningLimit(); i++)
	{
		Vector3* EnemyPosition;
		EnemyPosition = Spawner->SpawnLocation();
		Enemies_.push_back(new Enemy(Enemies_.size(), EnemyPosition));
		Enemies_[i]->AddWaypoints(Vector3(EnemyPosition->x + 10, EnemyPosition->y, EnemyPosition->z + 10));
	}
}

void SP2_Scene01::RenderMonsters()
{
	for (unsigned int i = 0; i < Enemies_.size(); i++)
	{
		modelStack.PushMatrix();
		modelStack.Translate(Enemies_[i]->getPosition().x, 0.f, Enemies_[i]->getPosition().z);
		modelStack.Rotate(Enemies_[i]->getAngle(), 0.f, 1.f, 0.f);
		RenderMesh(meshList[NPC_ALIEN], true);
		modelStack.PopMatrix();
	}
}

bool SP2_Scene01::ReadCharacterWidth(char* csvFilePath)
{
	std::ifstream file(csvFilePath);
	string data[256];

	if (file.good())
	{
		for (int a = 0; a < 256; a++)
		{
			getline(file, data[a]);
			iChWidth[a] = stoi(data[a]);
		}
		file.close();

		return true;
	}
	return false;
}