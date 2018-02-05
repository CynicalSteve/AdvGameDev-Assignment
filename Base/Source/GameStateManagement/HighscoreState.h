#pragma once

#include "Scene.h"
#include "Mtx44.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "../FPSCamera.h"
#include "../SpriteEntity.h"
#include "../Lua/LuaInterface.h"
#include "ShaderProgram.h"

#include <vector>

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

	char ScoreIncrease, ScoreDecrease;
	char MoveUp, MoveDown;

	std::vector<unsigned int> PlayerScoreVector;
	std::vector<std::string> PlayerNameVector;

private:
	FPSCamera camera;
	SpriteEntity *MenuStateBackground;
	SpriteEntity *ButtonBorder;

	TextEntity* textObj[11];
	bool isHighscoreFull;

	

	unsigned int ScoreToInput;
	void ButtonborderPosSnap();

	enum ButtonState
	{
		STATE_SAVESCORE,
		STATE_RETURNMAIN,

		STATES_TOTAL
	};

	ButtonState buttonState;
};