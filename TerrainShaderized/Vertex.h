#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>

using namespace std;
using namespace glm;

class Vertex
{
public:
	vec4 coords;
	vec3 normals;
	vec2 texCoords;
};

