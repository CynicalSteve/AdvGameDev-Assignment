#include "LuaInterface.h"
#include <iostream>

using namespace std;

//Allocating & initialising CLuaInterface's static data member
//The pointer is allocated but not the object's constructor
CLuaInterface *CLuaInterface::s_instance = 0;

CLuaInterface::CLuaInterface() : theLuaState(nullptr)
{
}

CLuaInterface::~CLuaInterface()
{
}

CLuaInterface* CLuaInterface::GetInstance()
{
	if (!s_instance)
	{
		s_instance = new CLuaInterface;
	}

	return s_instance;
}

bool CLuaInterface::DropInstance()
{
	if (s_instance)
	{
		//Drop the Lua Interface class
		s_instance->Drop();

		delete s_instance;
		s_instance = nullptr;
		return true;
	}
	return false;
}

bool CLuaInterface::Init()
{
	bool result = false;

	//Create the lua state
	theLuaState = lua_open();

	if (theLuaState)
	{
		//Load lua auxiliary libraries
		luaL_openlibs(theLuaState);

		//Load lua scripts
		luaL_dofile(theLuaState, "Image//DM2240.lua");

		result = true;
	}

	return result;
}

void CLuaInterface::Run()
{
	if (!theLuaState)
	{
		return;
	}

	//Read the variables
	lua_getglobal(theLuaState, "title");
	//Extract value, index -1 as variable is already retrieved using lua_getglobal
	const char *title = lua_tostring(theLuaState, -1);

	lua_getglobal(theLuaState, "width");
	int width = lua_tointeger(theLuaState, -1);

	lua_getglobal(theLuaState, "height");
	int height = lua_tointeger(theLuaState, -1);

	//Display on screen
	cout << title << "\n";
	cout << "-------------------------------------------------\n";
	cout << "Width of screen: " << width << "\n";
	cout << "Height of screen" << height << "\n";
}

void CLuaInterface::Drop()
{
	if (theLuaState)
	{
		//close Lua State
		lua_close(theLuaState);
	}
}

int CLuaInterface::getIntValue(const std::string Name)
{
	if (!theLuaState)
	{
		cout << "Lua State does not exist!\n";
		return -1;
	}

	lua_getglobal(theLuaState, Name.c_str());
	return lua_tointeger(theLuaState, -1);
}

float CLuaInterface::getFloatValue(const std::string Name)
{
	if (!theLuaState)
	{
		cout << "Lua State does not exist!\n";
		return -1;
	}

	lua_getglobal(theLuaState, Name.c_str());
	return (float)lua_tonumber(theLuaState, -1);
}

void CLuaInterface::saveIntValue(std::string Name, const int value, const bool overwrite)
{
	if (!theLuaState)
	{
		cout << "Lua State does not exist!\n";
		return;
	}

	lua_getglobal(theLuaState, "SaveToLuaFile");
	Name += " = " + std::to_string(value) + '\n';
	lua_pushstring(theLuaState, Name.c_str());
	lua_pushinteger(theLuaState, overwrite);
	lua_call(theLuaState, 2, 0);
}

void CLuaInterface::saveFloatValue(std::string Name, const float value, const bool overwrite)
{
	if (!theLuaState)
	{
		cout << "Lua State does not exist!\n";
		return;
	}

	lua_getglobal(theLuaState, "SaveToLuaFile");
	Name += " = " + std::to_string(value) + '\n';
	lua_pushstring(theLuaState, Name.c_str());
	lua_pushinteger(theLuaState, overwrite);
	lua_call(theLuaState, 2, 0);
}
