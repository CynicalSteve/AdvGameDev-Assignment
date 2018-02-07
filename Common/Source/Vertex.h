#ifndef VERTEX_H
#define VERTEX_H

#include "Vector3.h"

struct Position
{
	float x, y, z;
	Position(float x = 0, float y = 0, float z = 0) {Set(x, y, z);}
	void Set(float x, float y, float z) {this->x = x; this->y = y; this->z = z;}
};

struct Color
{
	float r, g, b;
	Color(float r = 1, float g = 1, float b = 1) {Set(r, g, b);}
	void Set(float r, float g, float b) {this->r = r; this->g = g; this->b = b;}

	Color(const std::string &ColorName) 
	{
		if (ColorName == "white")
		{
			Set(1, 1, 1);
		}
		else if (ColorName == "black")
		{
			Set(0, 0, 0);
		}
		else if (ColorName == "red")
		{
			Set(1, 0, 0);
		}
		else if (ColorName == "green")
		{
			Set(0, 1, 0);
		}
		else if (ColorName == "blue")
		{
			Set(0, 0, 1);
		}
		else if (ColorName == "yellow")
		{
			Set(1, 1, 0);
		}
		else if (ColorName == "violet")
		{
			Set(1, 0, 1);
		}
		else if (ColorName == "cyan")
		{
			Set(0, 1, 1);
		}
		else if (ColorName == "orange")
		{
			Set(1, .647f, 0);
		}
		else if (ColorName == "grey")
		{
			Set(.663f, .663f, .663f);
		}
		
		else if (ColorName == "purple")
		{
			Set(.502f, 0, .502f);
		}
		else if (ColorName == "brown")
		{
			Set(.647f, .165f, .165f);
		}
		else if (ColorName == "pink")
		{
			Set(1, .753f, .796f);
		}
		
		else
		{
			//If no color is found, default to white
			printf_s("No predefined colour of '%s' was found. Please check if the colour is defined in Vertex.h\n", ColorName.c_str());
			Set(1, 1, 1);
		}
	}
};

struct TexCoord
{
	float u, v;
	TexCoord(float u = 0, float v = 0) {Set(u, v);}
	void Set(float u, float v) {this->u = u; this->v = v;}
};

struct Vertex 
{
	Position pos;
	Color color;
	Vector3 normal;
	TexCoord texCoord;
	Vertex(){}
};

#endif