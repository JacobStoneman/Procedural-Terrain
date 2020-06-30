#include "Square.h"

unsigned int modelMatLoc;

Square::Square(int texNum)
{
	glGenVertexArrays(1, vao);
	glGenBuffers(1, buffer);
	this->texNum = texNum;
	Setup();
}

void Square::bindVao(TexVertex vert[]) {
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);   // Bind vertex buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexVertex) * 4, vert, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vert[0]), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vert[0]), (GLvoid*)sizeof(vert[0].coords));
	glEnableVertexAttribArray(1);
	Globals::objectLoc = glGetUniformLocation(Globals::programId, "objNum");
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vert[0]), (GLvoid*)(sizeof(vert[0].coords) + sizeof(vert[0].colours)));
	glEnableVertexAttribArray(2);
}

void Square::Setup()
{
	TexVertex squareVertices[] =
	{
		{ { 0.0, -offset, 0.0, 1.0 },{ 1.0, 0.0, 1.0} ,{ 0.0, 0.0 } },
	{ {offset * 2, -offset, 0.0, 1.0 },{ 1.0, 0.0, 1.0} ,{ 1.0, 0.0 } },
	{ { 0.0, offset, 0.0, 1.0 },{ 1.0, 0.0, 1.0},{ 0.0, 1.0 } },
	{ { offset * 2, offset, 0.0, 1.0 },{ 1.0, 0.0, 1.0},{ 1.0, 1.0 } }
	};

	TexVertex square2Vertices[] =
	{
		{ { offset * 2, -offset, 0.0, 1.0 },{ 0.0, 0.0, 1.0} ,{ 0.0, 0.0 } },
	{ { offset * 2, -offset, offset * 2, 1.0 },{ 0.0, 0.0, 1.0},{ 1.0, 0.0 } },
	{ { offset * 2, offset, 0.0, 1.0 },{ 0.0, 0.0, 1.0} ,{ 0.0, 1.0 } },
	{ { offset * 2, offset, offset * 2, 1.0 },{ 0.0, 0.0, 1.0},{ 1.0, 1.0 } }
	};

	TexVertex square3Vertices[] =
	{
		{ { offset * 2, -offset, offset * 2, 1.0 },{ 1.0, 0.0, 0.0 },{ 0.0, 0.0 } },
	{ { 0.0, -offset, offset * 2, 1.0 },{ 1.0, 0.0, 0.0},{ 1.0, 0.0 } },
	{ { offset * 2, offset, offset * 2, 1.0 },{ 1.0, 0.0, 0.0 },{ 0.0, 1.0 } },
	{ { 0.0, offset, offset * 2, 1.0 },{ 1.0, 0.0, 0.0},{ 1.0, 1.0 } }
	};

	TexVertex square4Vertices[] =
	{
		{ { 0.0, -offset, 0.0, 1.0 },{ 1.0, 1.0, 0.0 } ,{ 1.0, 0.0 } },
	{ { 0.0, -offset, offset * 2, 1.0 },{ 1.0, 1.0, 0.0 },{ 0.0, 0.0 } },
	{ { 0.0, offset, 0.0, 1.0 },{ 1.0, 1.0, 0.0 },{ 1.0, 1.0 } },
	{ { 0.0, offset, offset * 2, 1.0 },{ 1.0, 1.0, 0.0 },{ 0.0, 1.0 } }
	};

	TexVertex square5Vertices[] =
	{
		{ { 0.0, offset, 0.0, 1.0 },{ 0.0, 1.0, 1.0 },{ 1.0, 0.0 } },
	{ { 0.0, offset, offset * 2, 1.0 },{ 0.0, 1.0, 1.0 },{ 0.0, 0.0 } },
	{ { offset * 2, offset, 0.0, 1.0 },{ 0.0, 1.0, 1.0},{ 1.0, 1.0 } },
	{ { offset * 2, offset, offset * 2, 1.0 },{ 0.0, 1.0, 1.0},{ 0.0, 1.0 } }
	};

	TexVertex square6Vertices[] =
	{
		{ { 0.0, -80.0, 0.0, 1.0 },{ 0.0, 1.0, 0.0 } ,{ 0.0, 0.0 } },
	{ { 0.0, -80.0, offset * 2, 1.0 },{ 0.0, 1.0, 0.0},{ 0.0, 1.0 } },
	{ { offset * 2, -80.0, 0.0, 1.0 },{ 0.0, 1.0, 0.0},{ 1.0, 0.0 } },
	{ { offset * 2, -80.0, offset * 2, 1.0 },{ 0.0, 1.0, 0.0},{ 1.0, 1.0 } }
	};

	switch (texNum) {
	case 4:
		bindVao(squareVertices);
		break;
	case 5:
		bindVao(square2Vertices);
		break;
	case 6:
		bindVao(square3Vertices);
		break;
	case 7:
		bindVao(square4Vertices);
		break;
	case 8:
		bindVao(square5Vertices);
		break;
	case 9:
		bindVao(square6Vertices);
		break;
	}
}

void Square::Draw(vec3 cameraPos)
{
	glm::mat4 modelMat = glm::mat4(1.0f);

	modelMat = glm::translate(modelMat, glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z));
	modelMat = glm::translate(modelMat, glm::vec3(-offset, -offset, -offset));

	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
	glUniform1i(Globals::objectLoc, texNum);
	glBindVertexArray(vao[0]);

	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}
