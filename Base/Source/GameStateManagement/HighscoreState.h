#pragma once

#include "Scene.h"
#include "Mtx44.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "../FPSCamera.h"
#include "../SpriteEntity.h"
#include "../Lua/LuaInterface.h"
#include "ShaderProgram.h"

class SceneManager;
class TextEntity;

class HighscoreState : public Scene
{
public:
	HighscoreState();
	~HighscoreState();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	float halfWindowWidth;
	float halfWindowHeight;

	char MoveUp, MoveDown;

private:
	FPSCamera camera;
	SpriteEntity *MenuStateBackground;

	TextEntity* textObj[11];
	bool isHighscoreFull;

	unsigned int ScoreToInput;
};