#include <iostream>
#include <fstream>

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
#include <vector>
//#include <GL/glew.h>
//#include <GL/freeglut.h>
#include <time.h>
#include "getbmp.h"
#include "Grid.h"
#include "Tree.h"
#include "SOIL.h"
#include "Square.h"
#include "Globals.h"
//#include <GL/glext.h>
#pragma comment(lib, "glew32.lib") 

using namespace std;
using namespace glm;

// Size of the terrain
const int MAP_SIZE = 33;

struct Material {
	vec4 ambRefl;
	vec4 difRefl;
	vec4 specRefl;
	vec4 emitCols;
	float shine;
};

struct Light {
	vec4 ambCols;
	vec4 difCols;
	vec4 specCols;
	vec4 coords;
};

static Light light0 = {
vec4(0.0,0.0,0.0,1.0),
vec4(1.0,1.0,1.0,1.0),
vec4(1.0,1.0,1.0,1.0),
vec4(0.0,1.0,0.0,0.0),
};

static const vec4 globAmb = vec4(0.2, 0.2, 0.2, 1.0);

struct Matrix4x4
{
	float entries[16];
};

static mat4 projMat = mat4(1.0);
static mat3 normalMat = mat3(1.0);

static const Matrix4x4 IDENTITY_MATRIX4x4 =
{
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	}
};

// Globals

static mat4 modelMat = mat4(1.0);
static mat4 ViewMat = mat4(1.0);

const int ASPECT = Globals::SCREEN_WIDTH / Globals::SCREEN_HEIGHT;
float zoomFactor = 0.0f;
int displayMode = 0;
float cameraTheta = 0.0f;
float cameraPhi = 0.0f;

static unsigned int
vertexShaderId,
fragmentShaderId,
modelMatLoc,
ViewMatLoc,
projMatLoc,
normalMatLoc,
timeLoc,
buffer[1],
vao[1],
skyVao[1],
skyVbo[1];

unsigned int cubemapTexture;
static BitMapFile* image[11];
static unsigned int grassTexLoc, waterTexLoc, texLoc, rockLoc,skyBoxSamplerLoc;

vector<Grid*> grids;
vector<Tree> trees;
vector<Square*> skySquares;

bool skyToggle = true;
vec4 terrainCentre;
float waveTime=0;

int frameCount;
float oldTimeSinceStart;
float newTimeSinceStart;
float deltaTime;
float treeSpawnRate = 0.1f;
float terrainMax = 60;
float treeHeightMax = 10;

bool firstMouse = true;
float Yaw = -90.0f;
float Pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
int debugMode = 0;
double renderDistance = 1000.0;
enum texLocations {
	grassTex,
	waterTex,
	rockTex,
	sandTex,
	snowTex,
	upTex,
	downTex,
	leftTex,
	rightTex,
	frontTex,
	backTex,
};
const GLchar* texLocNames[] = { "grassTex", "waterTex", "rockTex", "sandTex", "snowTex", "upTex", "downTex", "leftTex", "rightTex", "frontTex", "backTex" };

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	unsigned char* skyImg;
	int width, height;

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		string path = faces[i];
		skyImg = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		if (skyImg != 0)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, skyImg);
		}
		else
		{
			std::cout << "Cubemap texture failed to load" << std::endl;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureId;
}

void GenerateTextures(int texCount) {

	glGenTextures(texCount, Globals::texture);

	for (int i = 0; i < texCount; i++) {
		glActiveTexture(GL_TEXTURE0+i);
		glBindTexture(GL_TEXTURE_2D, Globals::texture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[i]->sizeX, image[i]->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, image[i]->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		texLoc = glGetUniformLocation(Globals::programId, texLocNames[i]);
		glUniform1i(texLoc, i);
	}
}

void shaderCompileTest(GLuint shader) { 
	GLint result = GL_FALSE; 
	int logLength; 
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result); 
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength); 
	std::vector<GLchar> vertShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(shader, logLength, NULL, &vertShaderError[0]); 
	std::cout << &vertShaderError[0] << std::endl;
}

// Function to read text file, used to read shader files
char* readTextFile(char* aTextFile)
{
	FILE* filePointer = fopen(aTextFile, "rb");
	char* content = NULL;
	long numVal = 0;

	fseek(filePointer, 0L, SEEK_END);
	numVal = ftell(filePointer);
	fseek(filePointer, 0L, SEEK_SET);
	content = (char*)malloc((numVal + 1) * sizeof(char));
	fread(content, 1, numVal, filePointer);
	content[numVal] = '\0';
	fclose(filePointer);
	return content;
}

float RandomFloat(float min, float max)
{
	float random = ((float)rand()) / RAND_MAX;

	float range = max - min;
	return (random * range) + min;
}

void mouseMovement(int xpos, int ypos) {
	if (displayMode == 0) {
		cameraUp = vec3(0, 1, 0);
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.2f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(radians(Yaw)) * cos(radians(Pitch));
		front.y = sin(radians(Pitch));
		front.z = sin(radians(Yaw)) * cos(radians(Pitch));
		cameraFront = glm::normalize(front);
		int winHeight = glutGet(GLUT_WINDOW_HEIGHT);
		int winWidth = glutGet(GLUT_WINDOW_WIDTH);
		if (xpos<100 || xpos >winWidth - 100) {
			lastX = winWidth / 2;
			lastY = winHeight / 2;
			glutWarpPointer(winWidth / 2, winHeight / 2);
		}
		else if (ypos<100 || ypos >winWidth - 100) {
			lastX = winWidth / 2;
			lastY = winHeight / 2;
			glutWarpPointer(winWidth / 2, winHeight / 2);
		}
	}
}

void GenerateTrees() {
	float spawnCheck = 0;
	for (int i = 0; i < grids[0]->MAP_SIZE * grids[0]->MAP_SIZE; i++) {
		if (grids[0]->GridVertices[i].coords.y > 0 && grids[0]->GridVertices[i].coords.y < treeHeightMax) {
			spawnCheck = RandomFloat(0, 1);
			if (spawnCheck < treeSpawnRate) {
				Tree tree(grids[0]->GridVertices[i].coords.x, grids[0]->GridVertices[i].coords.y, grids[0]->GridVertices[i].coords.z);
				trees.push_back(tree);
			}
		}
	}
}

// Initialization routine.
void setup(void)
{
	// Create shader program executable - read, compile and link shaders
	char* vertexShader = readTextFile("vertexShader.glsl");
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, (const char**)&vertexShader, NULL);
	glCompileShader(vertexShaderId);
	cout << "VERTEX::" << endl;
	shaderCompileTest(vertexShaderId);

	char* fragmentShader = readTextFile("fragmentShader.glsl");
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, (const char**)&fragmentShader, NULL);
	glCompileShader(fragmentShaderId);
	cout << "FRAGMENT::" << endl;
	shaderCompileTest(fragmentShaderId);

	Globals::programId = glCreateProgram();
	glAttachShader(Globals::programId, vertexShaderId);
	glAttachShader(Globals::programId, fragmentShaderId);
	glLinkProgram(Globals::programId);
	glUseProgram(Globals::programId);

	cout << "Loading textures..." << endl;
	image[0] = getbmp("./Textures/grass.bmp");
	image[1] = getbmp("./Textures/water.bmp");
	image[2] = getbmp("./Textures/rock.bmp");
	image[3] = getbmp("./Textures/sand.bmp");
	image[4] = getbmp("./Textures/snow.bmp");
	image[5] = getbmp("./Textures/skyboxBmp/cloudtop_up.bmp");
	image[6] = getbmp("./Textures/skyboxBmp/cloudtop_dn.bmp");
	image[7] = getbmp("./Textures/skyboxBmp/cloudtop_lf.bmp");
	image[8] = getbmp("./Textures/skyboxBmp/cloudtop_rt.bmp");
	image[9] = getbmp("./Textures/skyboxBmp/cloudtop_ft.bmp");
	image[10] = getbmp("./Textures/skyboxBmp/cloudtop_bk.bmp");

	cout << "Generating textures..." << endl;
	GenerateTextures(11);

	// Obtain projection matrix uniform location and set value.
	projMatLoc = glGetUniformLocation(Globals::programId, "projMat");
	projMat = perspective(radians(60.0), (double)Globals::SCREEN_WIDTH / (double)Globals::SCREEN_HEIGHT, 0.1, renderDistance);
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));
	///////////////////////////////////////

	int seed = time(NULL);
	srand(seed);
	cout << "Seed: " << seed << endl;
	//srand(1574559616); //Test seed 
	float testNum = RandomFloat(0, 1);

	//sky = new TextureCube(ViewMat, projMat);

	float skyVertices[] = {
		-10.0f,  10.0f, -10.0f,
		-10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		-10.0f,  10.0f, -10.0f,
		 10.0f,  10.0f, -10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		 10.0f, -10.0f,  10.0f
	};
	glGenVertexArrays(1, skyVao);
	glGenBuffers(1, skyVbo);
	glBindVertexArray(skyVao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, skyVbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), &skyVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	vector<std::string> faces
	{
		("./Textures/skyboxBmp/cloudtop_ft.jpg"),
		("./Textures/skyboxBmp/cloudtop_bk.jpg"),
		("./Textures/skyboxBmp/cloudtop_up.jpg"),
		("./Textures/skyboxBmp/cloudtop_dn.jpg"),
		("./Textures/skyboxBmp/cloudtop_rt.jpg"),
		("./Textures/skyboxBmp/cloudtop_lf.jpg"),

	};
	cubemapTexture = loadCubemap(faces);
	skyBoxSamplerLoc = glGetUniformLocation(Globals::programId, "skybox");
	glUniform1i(skyBoxSamplerLoc, cubemapTexture);



	glUseProgram(Globals::programId);
	cout << "Generating sky..." << endl;
	for (int i = 4; i < 10; i++) {
		Square* square = new Square(i);
		skySquares.push_back(square);
	}
	cout << "Generating terrain..." << endl;
	Grid* terrain = new Grid(0, terrainMax, 1);
	cout << "Generating water..." << endl;
	Grid* water = new Grid(1, 1, 1);
	grids.push_back(terrain);
	grids.push_back(water);
	cout << "Generating trees..." << endl;
	GenerateTrees();
	cout <<trees.size()<< " trees created" << endl;
	cout << endl;
	terrainCentre = grids[0]->terrainCentre;
	cameraPos = vec3(terrainCentre.x,terrainCentre.y + 100,terrainCentre.z);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.67, 0.84, 0.8, 0.9);

	timeLoc = glGetUniformLocation(Globals::programId, "time");

	// Obtain modelview matrix uniform location and set value.
	modelMatLoc = glGetUniformLocation(Globals::programId, "modelMat");
	ViewMatLoc = glGetUniformLocation(Globals::programId, "ViewMat");
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, value_ptr(modelMat));
	glUniformMatrix4fv(ViewMatLoc, 1, GL_FALSE, value_ptr(ViewMat));

	static const Material terrainFandB = {
		vec4(1.0,1.0,1.0,1.0),
		vec4(1.0,1.0,1.0,1.0),
		vec4(1.0,1.0,1.0,1.0),
		vec4(0.0,0.0,0.0,1.0),
		50.0f
	};

	glUniform4fv(glGetUniformLocation(Globals::programId, "terrainFandB.ambRefl"), 1, &terrainFandB.ambRefl[0]);
	glUniform4fv(glGetUniformLocation(Globals::programId, "terrainFandB.difRefl"), 1, &terrainFandB.difRefl[0]);
	glUniform4fv(glGetUniformLocation(Globals::programId, "terrainFandB.specRefl"), 1, &terrainFandB.specRefl[0]);
	glUniform4fv(glGetUniformLocation(Globals::programId, "terrainFandB.emitCols"), 1, &terrainFandB.emitCols[0]);
	glUniform1f(glGetUniformLocation(Globals::programId, "terrainFandB.shine"), terrainFandB.shine);

	glUniform4fv(glGetUniformLocation(Globals::programId, "globAmb"), 1, &globAmb[0]);

	glUniform4fv(glGetUniformLocation(Globals::programId, "light0.ambCols"), 1, &light0.ambCols[0]);
	glUniform4fv(glGetUniformLocation(Globals::programId, "light0.difCols"), 1, &light0.difCols[0]);
	glUniform4fv(glGetUniformLocation(Globals::programId, "light0.specCols"), 1, &light0.specCols[0]);
	glUniform4fv(glGetUniformLocation(Globals::programId, "light0.coords"), 1, &light0.coords[0]);

	normalMatLoc = glGetUniformLocation(Globals::programId, "normalMat");
	normalMat = transpose(inverse(mat3(modelMat)));
	glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, value_ptr(normalMat));
}

// Drawing routine.
void drawScene(void)
{
	glUseProgram(Globals::programId);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	ViewMat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(ViewMatLoc, 1, GL_FALSE, glm::value_ptr(ViewMat));
	glUniform1f(timeLoc, waveTime);
	waveTime+= 1 * (deltaTime / 1000.0f);
	if (waveTime > 360) {
		waveTime = 0;
	}
	if (debugMode == 0) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (debugMode == 1) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (debugMode == 2) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	for (int i = 0; i < grids.size(); i++) {
		grids[i]->Draw();
	}
	for (int i = 0; i < trees.size(); i++) {
		trees[i].Draw();
	}
	if (skyToggle) {
		glUniform1i(glGetUniformLocation(Globals::programId, "objNum"),10);
		glUniform3f(glGetUniformLocation(Globals::programId, "camPos"), cameraPos.x, cameraPos.y, cameraPos.z);
		glDepthFunc(GL_EQUAL);
		glBindVertexArray(skyVao[0]);
		glActiveTexture(GL_TEXTURE0 + cubemapTexture);
		glUniform1i(glGetUniformLocation(Globals::programId, "skybox"), cubemapTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LEQUAL);
	}
	else {
		for (int i = 0; i < skySquares.size(); i++) {
			skySquares[i]->Draw(cameraPos);
		}
	}
	glFlush();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	projMatLoc = glGetUniformLocation(Globals::programId, "projMat");
	projMat = perspective(radians(60.0), (double)Globals::SCREEN_WIDTH / (double)Globals::SCREEN_HEIGHT, 0.1, renderDistance);
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));
}

void checkPos() {
	if (!debugMode) {
		int x = cameraPos.x;
		int z = cameraPos.z;
		if (x < 0) {
			x = 0;
		}
		else if (x > 513) {
			x = 513;
		}
		if (z < 0) {
			z = 0;
		}
		else if (z > 513) {
			z = 513;
		}
		float y = (grids[0]->Gridf[x][z]) + 1;
		if (cameraPos.y > 250) {
			cameraPos.y = 250;
		} else if (cameraPos.y < y) {
			cameraPos.y = y;
		}
		if (cameraPos.z < 2) {
			cameraPos.z = 2;
		}
		if (cameraPos.z > 511.5f) {
			cameraPos.z = 511.5f;
		}
		if (cameraPos.x < 2) {
			cameraPos.x = 2;
		}
		if (cameraPos.x > 511.5f) {
			cameraPos.x = 511.5f;
		}
	}
}

void setLight() {
	if (light0.coords.z < 1) {
		light0.coords.z = 1;
	}
	if (light0.coords.z > 511.5f) {
		light0.coords.z = 511.5f;
	}
	if (light0.coords.x < 1) {
		light0.coords.x = 1;
	}
	if (light0.coords.x > 511.5f) {
		light0.coords.x = 511.5f;
	}
	glUniform4fv(glGetUniformLocation(Globals::programId, "light0.coords"), 1, &light0.coords[0]);
}

void keyInput(unsigned char key, int x, int y)
{
	float cameraSpeed = 5.0f;
	int seed = 0;
	int num = 0;
	float spawnCheck = 0;
	float lightMoveSpeed = 5;
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'w':
		cameraPos += cameraSpeed * cameraFront;
		break;
	case 's':
		cameraPos -= cameraSpeed * cameraFront;
		break;
	case 'a':
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		break;
	case 'd':
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		break;
	case 'q':
		displayMode = 1;
		cameraUp = vec3(0, 0, 1);
		cameraFront = vec3(0, -1, 0);
		cameraPos = vec3(terrainCentre.x, terrainCentre.y + 200, terrainCentre.z);
		break;
	case 'e':
		displayMode = 0;
		break;
	case 'n':
		debugMode++;
		if (debugMode > 2) {
			debugMode = 0;
		}
		break;
	case 'p':
		seed = time(NULL);
		seed += RandomFloat(-100000, 100000);
		srand(seed);
		cout << "Seed: " << seed << endl;
		cout << "Generating terrain..." << endl;
		grids[0]->randMax = terrainMax;
		grids[0]->H = 1; //Hardcoded should change
		grids[0]->Setup();
		cout << "Generating trees..." << endl;
		for (int i = 0; i < trees.size(); i++) {
			trees[i].Clear();
		}
		trees.clear();
		GenerateTrees();
		cout << trees.size() << " trees created" << endl;
		cout << endl;
		break;
	case 'i':
		light0.coords.z += lightMoveSpeed * (deltaTime / 1000.0f);
		setLight();
		break;
	case 'k':
		light0.coords.z -= lightMoveSpeed * (deltaTime / 1000.0f);
		setLight();
		break;
	case 'j':
		light0.coords.x -= lightMoveSpeed * (deltaTime / 1000.0f);
		setLight();
		break;
	case 'l':
		light0.coords.x += lightMoveSpeed * (deltaTime / 1000.0f);
		setLight();
		break;
	case 'o':
		if (skyToggle) {
			skyToggle = false;
		}
		else {
			skyToggle = true;
		}
		break;
	default:
		break;
	}
	checkPos();
}

void update() {
	cout << cameraPos.x && ", " && cameraPos.y && ", " && cameraPos.z;
	frameCount++;
	oldTimeSinceStart = newTimeSinceStart;
	newTimeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = newTimeSinceStart - oldTimeSinceStart;
	if (deltaTime == 0) {
		Sleep(1);
		newTimeSinceStart = glutGet(GLUT_ELAPSED_TIME);
		deltaTime = newTimeSinceStart - oldTimeSinceStart;
	}
	glutPostRedisplay();
}

// Main routine.
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	// Set the version of OpenGL (4.2)
	glutInitContextVersion(4, 2);
	// The core profile excludes all discarded features
	glutInitContextProfile(GLUT_CORE_PROFILE);
	// Forward compatibility excludes features marked for deprecation ensuring compatability with future versions
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(Globals::SCREEN_WIDTH, Globals::SCREEN_HEIGHT);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("322 Coursework");

	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);

	glewExperimental = GL_TRUE;
	glewInit();
	glutPassiveMotionFunc(mouseMovement);
	glutIdleFunc(update);

	setup();

	glutMainLoop();
}
