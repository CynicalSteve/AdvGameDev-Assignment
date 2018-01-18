#pragma once

//Include lua headers
#include "lua.hpp"
#include "lua.h"
#include "luaconf.h"
#include "lualib.h"
#include <string>

class CLuaInterface
{
public:
	static CLuaInterface *GetInstance();
	static CLuaInterface *s_instance;
	static bool DropInstance();

	virtual ~CLuaInterface();

	//Init Lua Interface Class
	bool Init();

	//Run Lua Interface Class
	void Run();

	//Drop the Lua Interface Class
	void Drop();

	int getIntValue(const std::string Name);
	float getFloatValue(const std::string Name);

	void saveIntValue(std::string Name, const int value, const bool overwrite = false);
	void saveFloatValue(std::string Name, const float value, const bool overwrite = false);


protected:

	CLuaInterface();

	//Pointer to the Lua State
	lua_State *theLuaState;
};