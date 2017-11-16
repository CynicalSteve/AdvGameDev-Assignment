#ifndef SP2_SCENE01_H
#define SP2_SCENE01_H

#include "Scene.h"
#include "MatrixStack.h"
#include "MeshBuilder.h"
#include "Camera.h"
#include "Light.h"
#include "Enemy.h"
#include "Spawns.h"
#include "Save.h"
#include "Ally.h"
#include "Safehouse.h"
#include "Water.h"
#include "Ore.h"

class SP2_Scene01 : public Scene
{
public:
	SP2_Scene01();
	~SP2_Scene01();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void RenderMesh(Mesh *mesh, bool enableLight);
	virtual void Exit();

	enum GEOMETRY_TYPE
	{
		GEO_TEXT,
		GEO_QUAD,
		GEO_ROAD,
		GEO_CROSSROAD,
		GEO_BARREL,
		GEO_CRATE,
		GEO_CRATESTACK,
		GEO_TEST,
		GEO_WATER,
		GEO_ORE,
		GEO_CAR,
		GEO_VALVE,
		GEO_LASER,
		GEO_EXCLAIM,
		GEO_HEALTH,
		GEO_AMMO,
		GEO_BEER,
		GEO_LASERGUN,
		GEO_FISTS,
		GEO_RESOURCE,
		GEO_DAMAGE,
		GEO_LIGHT1,
		GEO_CIRCLE,
		GEO_PROD,
		GEO_WATERQUAD1,
		GEO_WATERQUAD2,
		GEO_WATERQUAD3,
		BUILDING_SAFEHOUSE,
		BUILDING_BULLETTOWER,
		BUILDING_JIGSAWTOWER,
		BUILDING_ELEVATEDJIGSAW,
		BUILDING_POINTEDTOWER,
		BUILDING_SINGLECAPPED,
		BUILDING_TRIPLECAPPEDPOINTED,
		BUILDING_TRIPLETOWER,
		BUILDING_CLOVER,
		NPC_MORGANHEAD,
		NPC_MORGANSUIT,
		NPC_MORGANTIE,
		NPC_LILITHBODY,
		NPC_LILITHHEAD,
		NPC_NEPHILIM,
		NPC_ALIEN,
		SKYBOX_LEFT,
		SKYBOX_RIGHT,
		SKYBOX_TOP,
		SKYBOX_BOTTOM,
		SKYBOX_FRONT,
		SKYBOX_BACK,
		NUM_GEOMETRY,
	};

	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHTENABLED,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		U_LIGHT1_POSITION,
		U_LIGHT1_COLOR,
		U_LIGHT1_POWER,
		U_LIGHT1_KC,
		U_LIGHT1_KL,
		U_LIGHT1_KQ,
		U_LIGHT1_TYPE,
		U_LIGHT1_SPOTDIRECTION,
		U_LIGHT1_COSCUTOFF,
		U_LIGHT1_COSINNER,
		U_LIGHT1_EXPONENT,
		U_LIGHT2_POSITION,
		U_LIGHT2_COLOR,
		U_LIGHT2_POWER,
		U_LIGHT2_KC,
		U_LIGHT2_KL,
		U_LIGHT2_KQ,
		U_LIGHT2_TYPE,
		U_LIGHT2_SPOTDIRECTION,
		U_LIGHT2_COSCUTOFF,
		U_LIGHT2_COSINNER,
		U_LIGHT2_EXPONENT,
		U_NUMLIGHTS,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};

private:
	Mesh* meshList[NUM_GEOMETRY];
	Camera camera;
	Light light[3];
	MS modelStack, viewStack, projectionStack;
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	void RenderSkybox();
	void RenderLight(Light light, UNIFORM_TYPE pos, UNIFORM_TYPE dir);
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	bool ReadCharacterWidth(char* csvFilePath);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey, float rotate);
	void GenerateMonsters();
	void RenderMonsters();

	void RenderRoads();
	void RenderBuildings();
	void RenderNPC();
	void RenderProps();
	void RenderUI();
	void RenderWater();
	bool bLightEnabled;
	bool showFPS;

	std::string frameRate, Health;
	Spawns* Spawner;
	Saves State;
	Safehouse House;
	std::string Message, Story;
	std::vector<Enemy*> Enemies_;
	std::vector<Water*> WaterBoxes;
	std::vector<Ally*> NPCs_;
	std::vector<Ore*> OreBoxes;
	int Resource;
	int KDA;

	float WaterLimit;
	int iChWidth[256];
	bool Keybounce[256];
};

#endif