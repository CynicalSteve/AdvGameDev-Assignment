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

class OptionsState : public Scene
{
public:
	OptionsState();
	~OptionsState();

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
	TextEntity *CurrentResolution;
	SpriteEntity *ButtonBorder;
	ShaderProgram *currProg;

	float tempWidth, tempHeight;

	void ButtonborderPosSnap();

	enum Resolutions
	{
		Res_800x600,
		Res_1280x720,
		Res_1366x768,
		Res_1920x1080,
		Res_3840x2160,

		TOTAL_RESOLUTIONS
	};
	Resolutions resolutionState;

	enum ButtonState
	{
		STATE_RESOLUTION,
		
		STATE_SAVEEXIT,
		STATE_CANCELEXIT,

		STATES_TOTAL
	};
	ButtonState buttonState;
};