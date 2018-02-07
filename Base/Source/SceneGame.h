#ifndef SCENE_TEXT_H
#define SCENE_TEXT_H

#include "Scene.h"
#include "Mtx44.h"
#include "PlayerInfo/PlayerInfo.h"
#include "GroundEntity.h"
#include "FPSCamera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "GenericEntity.h"
#include "SceneGraph/UpdateTransformation.h"
#include "Enemy\Enemy.h"
#include "Lua\LuaInterface.h"

class ShaderProgram;
class SceneManager;
class TextEntity;
class Light;
class SceneGame : public Scene
{	
public:
	SceneGame();
	~SceneGame();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void ShaderInit();

	//Initialise all things that take values from GameSaveData.lua
	void GameSaveInit();

private:
	SceneGame(SceneManager* _sceneMgr); // This is used to register to SceneManager

	ShaderProgram* currProg;
	CPlayerInfo* playerInfo;
	GroundEntity* groundEntity;
	FPSCamera camera;
	TextEntity* textObj[3];
	Light* lights[2];

	CEnemy* theEnemy;

	static SceneGame* sInstance; // The pointer to the object that gets registered

	char KeySaveGame, KeyPrevWeapon, KeyNextWeapon;

	void SaveGame();
	void CreateGameObjects();
};

#endif