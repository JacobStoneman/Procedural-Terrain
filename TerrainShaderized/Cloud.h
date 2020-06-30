#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Globals.h"
#include <vector>
#include "getbmp.h"
#include "TexVertex.h"

using namespace std;
using namespace glm;

class Cloud
{
public:
	Cloud(int objNum);
	void Setup();
	unsigned int buffer[1];
	unsigned int vao[1];
	static const int MAP_SIZE = 513;
	TexVertex CloudVertices[MAP_SIZE * MAP_SIZE];
	static const int numStripsRequired = MAP_SIZE - 1;
	static const int verticesPerStrip = 2 * MAP_SIZE;
	long GridIndexData[numStripsRequired][verticesPerStrip];
	float Gridf[MAP_SIZE][MAP_SIZE];
};

