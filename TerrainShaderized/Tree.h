#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "TreeVertex.h"
#include "Globals.h"
#include <vector>

using namespace std;
using namespace glm;

class Tree
{
public:
	Tree(float startPointX, float startPointY, float startPointZ);
	void ComputeSingleBranch(int depthLevel, int direction, float x0, float y0, float z0, float x1, float y1, float z1, float& x2, float& y2, float& z2);
	void ComputeBranch(int maxLevel, int depthLevel, int index, float angle, vector<vec3> BasePoints, vector<vec3> BranchPoints);
	void CreateTree(int maxLevel, float startPointX, float startPointY, float startPointZ);
	void Setup();
	void Clear();
	void Draw();
	float startPointX = 0;
	float startPointY = 0;
	float startPointZ = 0;
	unsigned int buffer[1];
	unsigned int vao[2];
	int branchLevel = 0;
	static const int NUMPOINTS = 2000;
	const float MINHEIGHT = 0.5;
	const float MAXHEIGHT = 1.5;
	const float MAXRATIO = 0.85f;
	const float MINRATIO = 0.60f;
	const float MAXROTATION = 60.0f;
	const float MINROTATION = 20.0f;
	static TreeVertex TrunkVertices[NUMPOINTS];
	static TreeVertex LeafVertices[NUMPOINTS];
	vector<uint> branchIndexData;
	vector<uint> leafStartPointData;
};

