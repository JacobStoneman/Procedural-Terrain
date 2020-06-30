#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Vertex.h"
#include "Globals.h"
#include <iostream>

using namespace std;
using namespace glm;

class Grid
{
public:
	Grid(int texNum, float randMax, float H);
	void DiamondStep(int x, int z, int stepSize);
	void calcPoint(int pX, int pZ, float stepDiv);
	void SquareStep(int x, int z, int stepSize);
	void Setup();
	void otherSetup();
	void Draw();
	unsigned int buffer[1];
	unsigned int vao[1];
	unsigned int programId = 0;
	float randMax;
	float H;
	vec4 terrainCentre;
	int texNum;
	static const int MAP_SIZE = 513;
	Vertex GridVertices[MAP_SIZE * MAP_SIZE];
	static const int numStripsRequired = MAP_SIZE - 1;
	static const int verticesPerStrip = 2 * MAP_SIZE;
	long GridIndexData[numStripsRequired][verticesPerStrip];
	float Gridf[MAP_SIZE][MAP_SIZE];
};

