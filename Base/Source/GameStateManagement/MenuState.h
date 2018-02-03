#pragma once

#include "Scene.h"
#include "Mtx44.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "../FPSCamera.h"
#include "../SpriteEntity.h"
#include "../Lua/LuaInterface.h"

class SceneManager;

class CMenuState : public Scene
{
public:
	CMenuState();
	~CMenuState();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	FPSCamera camera;
	SpriteEntity *MenuStateBackground;
	SpriteEntity *ButtonBorder;

	enum ButtonState
	{
		STATE_PLAY_GAME = 0,
		STATE_OPTIONS,
		STATE_HIGHSCORES,
		STATE_EXIT,

		STATES_TOTAL
	};

	ButtonState buttonState;
};