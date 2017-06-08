#pragma once
#include "osl.h"
#include "Camera.h"
#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "src\SOIL.h"
#include <string>

class App {
struct vtxData{
	float a, b, c, x, y, z, u, v;
};
struct face {
	vtxData a, b, c;
};
struct betterData {
	glm::vec3 pos, normals;
	glm::vec2 UV;
};
struct betterFace {
	betterData f1, f2, f3;
};

private:
	int sphereSize;
	GLuint sphereVa;
	GLuint cubeVa;
	GLuint fbo;
	osl oslstuff;
	GLFWwindow* w;
	Camera camera;
	glm::mat4 cubeMatrices[12*24];
	glm::mat4 sphereMatrices[12*24];

	glm::vec3 movement = { 0,0,0 };
	int runTime;
	int everyXFrame;
	int instanceID;
	std::string resultPath;
public:
	App();
	App(bool mode, int runTime, int shotRate, int instanceNumber, std::string path, int asyncer);
	~App();
	void run();
	GLuint createSphereVBO(int xRes, int yRes);
	GLuint createSphereVBO(int resolution);
	GLuint createCubeVBO();
	void createCubes();
	void createSpheres();
	void updateInputs();
	void controls(float dt);

	GLuint screenSaveTex;
	void createFrameBuffer();
	void saveFrameToFile(int nr);
	GLuint loadTexture(std::string path);
};