#pragma once

//Include lua headers
#include "lua.hpp"
#include "lua.h"
#include "luaconf.h"
#include "lualib.h"
#include <string>
#include "Vector3.h"

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

	//Drop a specific lua interface
	void Drop(lua_State* &luaStateToDrop);
	//Drop all interfaces
	void DropAll();

	int getIntValue(const std::string &Name);
	float getFloatValue(const std::string &Name);
	char getCharValue(const std::string &Name);
	Vector3 getVector3Values(const std::string &Name);
	float getDistanceSquareValue(const std::string &Name, const Vector3 &source, const Vector3 &destination);
	float GetField(const char *key);
	std::string getStringValue(const std::string &Name);

	void saveIntValue(std::string Name, const int &value, const std::string &FunctionName, const bool &overwrite = false);
	void saveFloatValue(std::string Name, const float &value, const bool &overwrite = false);
	
	void error(const std::string &errorCode);
	void SetLuaFile(const std::string &NewLuaFileName, lua_State* &luaState);

	std::string GetTheLuaStateCurrentFilename();

	//Pointers to the Lua State
	lua_State *theLuaState;
	lua_State *theErrorState;

protected:
	std::string CurrentFileName;
	CLuaInterface();
};