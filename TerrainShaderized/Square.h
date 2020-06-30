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

using namespace glm;
using namespace std;

class Square
{
public:
	Square(int texNum);
	void Setup();
	void Draw(vec3 cameraPos);
	void bindVao(TexVertex vert[]);
	float offset = 513/2;
	unsigned int buffer[1];
	unsigned int vao[1];
	int texNum;
};

