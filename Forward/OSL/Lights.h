#pragma once
#include "GL\glew.h"
#include "glm\glm.hpp"
#include "GLFW\glfw3.h"
#include "Hitbox.h"

using namespace glm;
struct aLight {
	vec4 position, colora;
};
class Lights {
public:
	Lights();
	~Lights();

	void update(GLuint programID, float dt);
	void init(GLuint programID, int nrOfLights);

	aLight* allLights;
	Hitbox* lightboxes;

private:
	GLuint lightBuffer;
	float timer;
	int nrOfLight;
};