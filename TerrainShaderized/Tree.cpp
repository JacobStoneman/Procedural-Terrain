#include "Tree.h"

TreeVertex Tree::TrunkVertices[Tree::NUMPOINTS] = {};
TreeVertex Tree::LeafVertices[Tree::NUMPOINTS] = {};

float TreeRandomFloat(float min, float max)
{
	float random = ((float)rand()) / RAND_MAX;
	float range = max - min;
	return (random * range) + min;
}

Tree::Tree(float startPointX, float startPointY, float startPointZ)
{
	this->startPointX = startPointX;
	this->startPointY = startPointY;
	this->startPointZ = startPointZ;
	this->branchLevel = TreeRandomFloat(4, 8.5);
	glGenVertexArrays(1, vao);
	glGenBuffers(1, buffer);
	this->Setup();
	this->Draw();
}

void Tree::ComputeSingleBranch(int depthLevel, int direction, float x0, float y0, float z0, float x1, float y1, float z1, float& x2, float& y2, float& z2)
{
	float angle = radians((TreeRandomFloat(MINROTATION, MAXROTATION) * direction));
	float xs, ys, zs, xl, yl, zl, m, xll, yll, zll;
	double val;
	xs = (x1 - x0) * TreeRandomFloat(MINRATIO, MAXRATIO);
	ys = (y1 - y0) * TreeRandomFloat(MINRATIO, MAXRATIO);
	zs = (z1, z0) * TreeRandomFloat(MINRATIO, MAXRATIO);

	m = sqrt(xs * xs + ys * ys + zs * zs);

	xll = cos(angle / 2.0) * xs - sin(angle / 2.0) * ys;
	yll = sin(angle / 2.0) * xs + cos(angle / 2.0) * ys;
	zll = 0.0;

	if (depthLevel % 2 == 0) {
		val = -2;
	}
	else {
		val = 2;
	}

	xl = cos(val * angle / 2.0) * xll - sin(val * angle / 2.0) * zll;
	yl = yll;
	zl = sin(val * angle / 2.0) * xll + cos(val * angle / 2.0) * zll;

	x2 = x1 + xl;
	y2 = y1 + yl;
	z2 = z1 + zl;
}

void Tree::ComputeBranch(int maxLevel, int depthLevel, int index, float angle, vector<vec3> BasePoints, vector<vec3> BranchPoints)
{
	int size;
	float x2, y2, z2;
	vec3 ttPt;
	vector<vec3> NewBasePoints, NewBranchPoints;

	int newPrev = index;

	if (depthLevel > maxLevel) {
		return;
	}

	size = BasePoints.size();

	if (size == 0) {
		return;
	}

	for (int i = 0; i < size; i++) {
		Tree::ComputeSingleBranch(depthLevel, 1, BasePoints[i].x, BasePoints[i].y, BasePoints[i].z, BranchPoints[i].x, BranchPoints[i].y, BranchPoints[i].z, x2, y2, z2);
		TrunkVertices[index].coords[0] = x2;
		TrunkVertices[index].coords[1] = y2;
		TrunkVertices[index].coords[2] = z2;
		TrunkVertices[index].coords[3] = 1.0;
		index++;


		NewBasePoints.push_back(BranchPoints[i]);
		ttPt.x = x2;
		ttPt.y = y2;
		ttPt.z = z2;
		NewBranchPoints.push_back(ttPt);
		ComputeSingleBranch(depthLevel, -1, BasePoints[i].x, BasePoints[i].y, BasePoints[i].z, BranchPoints[i].x, BranchPoints[i].y, BranchPoints[i].z, x2, y2, z2);
		TrunkVertices[index].coords[0] = x2;
		TrunkVertices[index].coords[1] = y2;
		TrunkVertices[index].coords[2] = z2;
		TrunkVertices[index].coords[3] = 1.0;

		index++;
		NewBasePoints.push_back(BranchPoints[i]);
		ttPt.x = x2;
		ttPt.y = y2;
		ttPt.z = z2;
		NewBranchPoints.push_back(ttPt);
	}
	depthLevel++;
	Tree::ComputeBranch(maxLevel, depthLevel, index, angle, NewBasePoints, NewBranchPoints);
}

void Tree::CreateTree(int maxLevel, float startPointX, float startPointY, float startPointZ)
{
	int i, count;
	float angle = radians(MAXROTATION);
	vec3 ttPt;
	vector<vec3> BasePoints, BranchPoints;

	float height = TreeRandomFloat(MINHEIGHT, MAXHEIGHT);

	for (i = 0; i < NUMPOINTS; i++) {
		TrunkVertices[i].colors[0] = 0.55;
		TrunkVertices[i].colors[1] = 0.27;
		TrunkVertices[i].colors[2] = 0.075;
		TrunkVertices[i].colors[3] = 1.0;
	}

	TrunkVertices[0].coords[0] = startPointX;
	TrunkVertices[0].coords[1] = startPointY;
	TrunkVertices[0].coords[2] = startPointZ;
	TrunkVertices[0].coords[3] = 1.0;
	ttPt.x = TrunkVertices[0].coords[0];
	ttPt.y = TrunkVertices[0].coords[1];
	ttPt.z = TrunkVertices[0].coords[2];
	BasePoints.push_back(ttPt);

	TrunkVertices[1].coords[0] = startPointX;
	TrunkVertices[1].coords[1] = startPointY + height;
	TrunkVertices[1].coords[2] = startPointZ;
	TrunkVertices[1].coords[3] = 1.0;
	ttPt.x = TrunkVertices[1].coords[0];
	ttPt.y = TrunkVertices[1].coords[1];
	ttPt.z = TrunkVertices[1].coords[2];
	BranchPoints.push_back(ttPt);

	branchIndexData.push_back(0);
	branchIndexData.push_back(1);
	branchIndexData.push_back(1);
	branchIndexData.push_back(2);
	branchIndexData.push_back(1);
	branchIndexData.push_back(3);

	i = 2;
	ComputeBranch(maxLevel, 0, i, angle, BasePoints, BranchPoints);

	for (int n = 1; n < pow(2, maxLevel); n++) {
		branchIndexData.push_back(n);
		branchIndexData.push_back((n) * 2);
		branchIndexData.push_back(n);
		branchIndexData.push_back(((n) * 2) + 1);
	}

	//for (int j = pow(2, maxLevel); j < pow(2, maxLevel + 1); j++) {
	//	leafStartPointData.push_back(j);
	//}

	//int vCount = 0;
	//for (int k = 0; k < leafStartPointData.size(); k++) {
	//	LeafVertices[vCount].coords[0] = TrunkVertices[leafStartPointData[k]].coords[0];
	//	LeafVertices[vCount].coords[1] = TrunkVertices[leafStartPointData[k]].coords[1];
	//	LeafVertices[vCount].coords[2] = TrunkVertices[leafStartPointData[k]].coords[2];
	//	LeafVertices[vCount].coords[3] = TrunkVertices[leafStartPointData[k]].coords[3];
	//	vCount++;

	//	LeafVertices[vCount].coords[0] = TrunkVertices[leafStartPointData[k]].coords[0] - 2;
	//	LeafVertices[vCount].coords[1] = TrunkVertices[leafStartPointData[k]].coords[1] + 2;
	//	LeafVertices[vCount].coords[2] = TrunkVertices[leafStartPointData[k]].coords[2];
	//	LeafVertices[vCount].coords[3] = TrunkVertices[leafStartPointData[k]].coords[3];
	//	vCount++;

	//	LeafVertices[vCount].coords[0] = TrunkVertices[leafStartPointData[k]].coords[0] + 2;
	//	LeafVertices[vCount].coords[1] = TrunkVertices[leafStartPointData[k]].coords[1] + 2;
	//	LeafVertices[vCount].coords[2] = TrunkVertices[leafStartPointData[k]].coords[2];
	//	LeafVertices[vCount].coords[3] = TrunkVertices[leafStartPointData[k]].coords[3];
	//	vCount++;
	//}
}

void Tree::Setup()
{
	int branchLevel = TreeRandomFloat(4, 8.5);
	Tree::CreateTree(branchLevel, this->startPointX, this->startPointY, this->startPointZ);
	glGenVertexArrays(2, vao);
	glGenBuffers(1, buffer);
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->TrunkVertices), this->TrunkVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(this->TrunkVertices[0]), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(this->TrunkVertices[0]), (GLvoid*)sizeof(this->TrunkVertices[0].coords));
	glEnableVertexAttribArray(1);

	//glBindVertexArray(vao[1]);
	//glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(this->LeafVertices), this->LeafVertices, GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(this->LeafVertices[0]), 0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(this->LeafVertices[0]), (GLvoid*)sizeof(this->LeafVertices[0].coords));
	//glEnableVertexAttribArray(1);
}

void Tree::Clear()
{
	branchIndexData.clear();
	leafStartPointData.clear();
	glDeleteBuffers(1, buffer);
	glDeleteVertexArrays(1, vao);
	//Clear leaf buffers if used
}

void Tree::Draw()
{
	glUniform1i(Globals::objectLoc, 2);
	glBindVertexArray(vao[0]);
	glDrawElements(GL_LINES, branchIndexData.size(), GL_UNSIGNED_INT, branchIndexData.data());
	//glBindVertexArray(vao[1]);
	//glDrawElements(GL_TRIANGLE_STRIP, 3, GL_UNSIGNED_INT, this->leafStartPointData.data());
}