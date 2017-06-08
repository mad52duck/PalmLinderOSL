#pragma once
#include <GL\glew.h>
#include <GL\GL.h>
#include <glm\glm.hpp>
#include "Lights.h"
#include "Hitbox.h"

class Forward {

struct renderObject {
	Hitbox hb;
	int lights[10];
	int lightsAffecting;
	bool fixed;
};
public:
	bool dynamic;
	int nrOfSpheres;
	int nrOfCubes;
	Lights lights;
	const int nrOfLights = 10;
	int sphereSize;
	Forward();
	~Forward();
	void init(bool dynamic);
	void render(glm::mat4 view, glm::mat4 viewProj, glm::vec3 position, float dt);
	void updateLights(renderObject* instances, int number);
	glm::mat4* cubes;
	glm::mat4* spheres;
	renderObject* cubeObjs;
	renderObject* sphereObjs;

	GLuint cubeVao;
	GLuint sphereVao;
	GLuint cubeTex;
	GLuint sphereTex;
	GLuint programID;
};