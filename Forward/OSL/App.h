#pragma once
#include "Forward.h"
#include "Camera.h"
#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "src\SOIL.h"
#include <string>
#include "Lights.h"

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
	Forward forwardProgram;
	GLFWwindow* w;
	Camera camera;
	glm::mat4 cubeMatrices[256];
	glm::mat4 sphereMatrices[256];
	glm::vec3 movement = { 0,0,0 };

	int runTime;
	int everyXFrame;
	int instanceID;
	std::string resultPath;

	void controls(float dt);
public:
	App();
	App(bool mode, int runTime, int shotRate, int instanceNumber, std::string path);
	~App();
	void run();
	GLuint createSphereVBO(int resolution);
	GLuint createSphereVBO(int xRes, int yRes);
	GLuint createCubeVBO();
	void createCubes();
	void createSpheres();
	void updateInputs();
	GLuint loadTexture(std::string path);

	GLuint fbo;
	GLuint screenSaveTex;
	void createFrameBuffer();
	void saveFrameToFile(int nr);
};