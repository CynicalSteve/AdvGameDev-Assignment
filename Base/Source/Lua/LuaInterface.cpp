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

	theErrorState = lua_open();

	if (theLuaState && theErrorState)
	{
		//Load lua auxiliary libraries
		luaL_openlibs(theLuaState);

		//Load the error lua script
		luaL_dofile(theErrorState, "Image//errorLookup.lua");
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

int CLuaInterface::getIntValue(const std::string &Name)
{
	if (!theLuaState)
	{
		cout << "Lua State does not exist!\n";
		return -1;
	}

	lua_getglobal(theLuaState, Name.c_str());
	return lua_tointeger(theLuaState, -1);
}

float CLuaInterface::getFloatValue(const std::string &Name)
{
	if (!theLuaState)
	{
		cout << "Lua State does not exist!\n";
		return -1;
	}

	lua_getglobal(theLuaState, Name.c_str());
	return (float)lua_tonumber(theLuaState, -1);
}

char CLuaInterface::getCharValue(const std::string &Name)
{
	lua_getglobal(theLuaState, Name.c_str());

	size_t length;
	const std::string string = lua_tolstring(theLuaState, -1, &length);

	//If the string is not empty, then return the first char
	if (length > 0)
	{
		return string[0];
	}
	else //else return a default value of a white space
	{
		return ' ';
	}
}

Vector3 CLuaInterface::getVector3Values(const std::string &Name)
{
	//Find the name in lua file
	lua_getglobal(theLuaState, Name.c_str());
	
	//Extract the first value from the variable container and places it into the lua stack
	//The third parameter of rawgeti specifies the index of the container (starts from index 1, not 0)
	lua_rawgeti(theLuaState, -1, 1);
	//Assign the value which is at the top of stack to Pos x
	const int x = lua_tonumber(theLuaState, -1);
	//Pop the value from lua stack
	lua_pop(theLuaState, 1); 

	lua_rawgeti(theLuaState, -1, 2);
	const int y = lua_tonumber(theLuaState, -1);
	lua_pop(theLuaState, 1);

	lua_rawgeti(theLuaState, -1, 3);
	const int z = lua_tonumber(theLuaState, -1);
	lua_pop(theLuaState, 1);

	return Vector3(x, y, z);
}

float CLuaInterface::getDistanceSquareValue(const std::string & Name, const Vector3 & source, const Vector3 & destination)
{
	lua_getglobal(theLuaState, Name.c_str());
	
	//Push position vertices onto lua stack
	lua_pushnumber(theLuaState, source.x);
	lua_pushnumber(theLuaState, source.y);
	lua_pushnumber(theLuaState, source.z);

	lua_pushnumber(theLuaState, destination.x);
	lua_pushnumber(theLuaState, destination.y);
	lua_pushnumber(theLuaState, destination.z);

	//Calls a function
	//lua_call 2nd Parameter -> The number of arguments pushed onto stack 
	//and are popped automatically after being passed as arguments
	//3rd parameter -> the number of results
	//The returned value is pushed onto the stack after function call
	lua_call(theLuaState, 6, 1);
	
	//Take the retured value from the stack and assign it to local variable
	const float distanceSquared = (float)lua_tonumber(theLuaState, -1);
	//Pop the value from stack
	lua_pop(theLuaState, 1);

	return distanceSquared;

}

float CLuaInterface::GetField(const char * key)
{
	//Error Check - whether the variables in the stack belong to a table
	if (!lua_istable(theLuaState, -1))
	{
		error("error100");
		return 0;
	}

	lua_pushstring(theLuaState, key);
	lua_gettable(theLuaState, -2);

	//Error check - returned value is not a number
	if (!lua_isnumber(theLuaState, - 1))
	{
		error("error101");
		return 0;
	}
	
	int result = static_cast<int>(lua_tonumber(theLuaState, -1));
	lua_pop(theLuaState, 1);

	return result;
}

void CLuaInterface::saveIntValue(std::string Name, const int &value, const bool &overwrite)
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

void CLuaInterface::saveFloatValue(std::string Name, const float &value, const bool &overwrite)
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

void CLuaInterface::error(const std::string & errorCode)
{
	if (!theErrorState)
	{
		return;
	}

	lua_getglobal(theErrorState, errorCode.c_str());

	const std::string errorMsg = lua_tostring(theErrorState, -1);

	if (!errorMsg.empty())
	{
		printf_s("%s\n", errorMsg.c_str());
	}
	else
	{
		printf_s("Error Code: %s is an unkown error!\n", errorCode.c_str());
	}
}
