#include "Grid.h"

float initialT = 0;

Grid::Grid(int texNum, float randMax, float H)
{
	this->H = H;
	this->randMax = randMax;
	this->texNum = texNum;
	glGenVertexArrays(1, vao);
	glGenBuffers(1, buffer);
	Setup();
}

float GridRandomFloat(float min, float max)
{
	float random = ((float)rand()) / RAND_MAX;

	float range = max - min;
	return (random * range) + min;
}

void Grid::DiamondStep(int x, int z, int stepSize) {
	float p1 = this->Gridf[x][z];
	float p2 = this->Gridf[x + stepSize][z];
	float p3 = this->Gridf[x][z + stepSize];
	float p4 = this->Gridf[x + stepSize][z + stepSize];
	float average = (p1 + p2 + p3 + p4) / 4;

	float num = average + GridRandomFloat(-randMax, randMax);

	x += stepSize / 2;
	z += stepSize / 2;

	this->Gridf[x][z] = num;
}

void Grid::calcPoint(int pX, int pZ, float stepDiv) {
	float p1Av = 0;
	int i = 0;
	int p1TopX = pX + stepDiv;
	int p1TopZ = pZ;
	if (p1TopX >= 0 && p1TopX < MAP_SIZE) {
		p1Av += this->Gridf[p1TopX][p1TopZ];
		i++;
	}
	int p1LeftX = pX;
	int p1LeftZ = pZ - stepDiv;
	if (p1LeftZ >= 0 && p1LeftZ < MAP_SIZE) {
		p1Av += this->Gridf[p1LeftX][p1LeftZ];
		i++;
	}
	int p1RightX = pX;
	int p1RightZ = pZ + stepDiv;
	if (p1RightZ >= 0 && p1RightZ < MAP_SIZE) {
		p1Av += this->Gridf[p1RightX][p1RightZ];
		i++;
	}
	int p1BottomX = pX - stepDiv;
	int p1BottomZ = pZ;
	if (p1BottomX >= 0 && p1BottomX < MAP_SIZE) {
		p1Av += this->Gridf[p1BottomX][p1BottomZ];
		i++;
	}
	p1Av /= i;
	p1Av += GridRandomFloat(-randMax, randMax);
	this->Gridf[pX][pZ] = p1Av;
}

void Grid::SquareStep(int x, int z, int stepSize) {
	float stepDiv = stepSize / 2;

	int p1X = x + stepDiv;
	int p1Z = z;
	calcPoint(p1X, p1Z, stepDiv);

	int p2X = x + stepSize;
	int p2Z = z + stepDiv;
	calcPoint(p2X, p2Z, stepDiv);

	int p3X = x + stepDiv;
	int p3Z = z + stepSize;
	calcPoint(p3X, p3Z, stepDiv);

	int p4X = x;
	int p4Z = z + stepDiv;
	calcPoint(p4X, p4Z, stepDiv);
}

void Grid::otherSetup() {
	// Intialise vertex array
	int i = 0;
	float fTextureS = float(MAP_SIZE) * 0.1f;
	float fTextureT = float(MAP_SIZE) * 0.1f;

	for (int z = 0; z < MAP_SIZE; z++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			// Set the coords (1st 4 elements) and a default colour of black (2nd 4 elements) 
			this->GridVertices[i] = { { (float)x, this->Gridf[x][z], (float)z, 1.0 }, { 0.0, 0.0, 0.0 } };

			float fScaleC = float(x) / float(MAP_SIZE - 1);
			float fScaleR = float(z) / float(MAP_SIZE - 1);
			this->GridVertices[i].texCoords = vec2(fTextureS * fScaleC, fTextureT * fScaleR);

			i++;
		}
	}

	// Now build the index data 
	i = 0;
	for (int z = 0; z < MAP_SIZE - 1; z++)
	{
		i = z * MAP_SIZE;
		for (int x = 0; x < MAP_SIZE * 2; x += 2)
		{
			this->GridIndexData[z][x] = i;
			i++;
		}
		for (int x = 1; x < MAP_SIZE * 2 + 1; x += 2)
		{
			this->GridIndexData[z][x] = i;
			i++;
		}
	}

	for (int z = 0; z < MAP_SIZE - 1; z++) {
		for (int x = 0; x < MAP_SIZE * 2 - 2; x++) {
			int index1 = this->GridIndexData[z][x];
			int index2 = this->GridIndexData[z][x + 1];
			int index3 = this->GridIndexData[z][x + 2];

			vec3 point1;
			vec3 point2;
			vec3 point3;

			//cout << index1 << endl;

			point1.x = this->GridVertices[index1].coords.x;
			point1.y = this->GridVertices[index1].coords.y;
			point1.z = this->GridVertices[index1].coords.z;

			point2.x = this->GridVertices[index2].coords.x;
			point2.y = this->GridVertices[index2].coords.y;
			point2.z = this->GridVertices[index2].coords.z;

			point3.x = this->GridVertices[index3].coords.x;
			point3.y = this->GridVertices[index3].coords.y;
			point3.z = this->GridVertices[index3].coords.z;

			vec3 edge1 = point2 - point1;
			vec3 edge2 = point3 - point1;

			vec3 normal;

			if (x % 2 == 1) {
				normal = cross(edge1, edge2);
			}
			else {
				normal = cross(edge2, edge1);
			}

			if (dot(normal, vec3(0, 1, 0)) < 0.0001) {
				normal = -normal;
			}

			vec3 idx1normal = this->GridVertices[index1].normals;
			vec3 idx2normal = this->GridVertices[index2].normals;
			vec3 idx3normal = this->GridVertices[index3].normals;

			this->GridVertices[index1].normals = normalize(normal + this->GridVertices[index1].normals);
			this->GridVertices[index2].normals = normalize(normal + this->GridVertices[index2].normals);
			this->GridVertices[index3].normals = normalize(normal + this->GridVertices[index3].normals);
		}
	}

	this->terrainCentre = this->GridVertices[(MAP_SIZE * MAP_SIZE) / 2].coords;


	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->GridVertices), this->GridVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(this->GridVertices[0]), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(this->GridVertices[0]), (GLvoid*)sizeof(this->GridVertices[0].coords));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(this->GridVertices[0]), (GLvoid*)(sizeof(this->GridVertices[0].coords) + sizeof(this->GridVertices[0].normals)));
	glEnableVertexAttribArray(2);
	Globals::objectLoc = glGetUniformLocation(Globals::programId, "objNum");
}

void Grid::Setup()
{
	if (this->texNum == 0) {
		for (int x = 0; x < MAP_SIZE; x++)
		{
			for (int z = 0; z < MAP_SIZE; z++)
			{
				if (x == 0 && z == 0) {
					this->Gridf[x][z] = GridRandomFloat(-randMax, randMax);
				}
				else if (x == MAP_SIZE - 1 && z == 0) {
					this->Gridf[x][z] = GridRandomFloat(-randMax, randMax);
				}
				else if (x == 0 && z == MAP_SIZE - 1) {
					this->Gridf[x][z] = GridRandomFloat(-randMax, randMax);
				}
				else if (x == MAP_SIZE - 1 && z == MAP_SIZE - 1) {
					this->Gridf[x][z] = GridRandomFloat(-randMax, randMax);
				}
				else {
					this->Gridf[x][z] = 0;
				}
			}
		}

		int stepSize = MAP_SIZE - 1;

		while (stepSize > 1) {
			for (int x = 0; x < MAP_SIZE - 1; x += stepSize) {
				for (int z = 0; z < MAP_SIZE - 1; z += stepSize) {
					DiamondStep(x, z, stepSize);
				}
			}
			for (int x = 0; x < MAP_SIZE - 1; x += stepSize) {
				for (int z = 0; z < MAP_SIZE - 1; z += stepSize) {
					SquareStep(x, z, stepSize);
				}
			}
			randMax = randMax * pow(2, -H);
			stepSize = stepSize / 2;
		}
	}
	else if(this->texNum == 1) {
		for (int x = 0; x < MAP_SIZE; x++) {
			for (int z = 0; z < MAP_SIZE; z++) {
				this->Gridf[x][z] = 0.0f;
			}
		}
	}
	Grid::otherSetup();
}

void Grid::Draw()
{
	glBindVertexArray(vao[0]);
	glUniform1i(Globals::objectLoc, this->texNum);
	for (int i = 0; i < MAP_SIZE - 1; i++)
	{
		glDrawElements(GL_TRIANGLE_STRIP, verticesPerStrip, GL_UNSIGNED_INT, this->GridIndexData[i]);
	}
}
