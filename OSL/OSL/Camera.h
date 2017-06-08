#pragma once
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW\glfw3.h>

using namespace glm;

class Camera {
public:
	Camera();
	~Camera();
	void update(double xpos, double ypos, float dt);
	glm::mat4 getViewProjection();
	static const int SCREEN_WIDTH = 1080;
	static const int SCREEN_HEIGHT = 720;

	//Controls
	void move(glm::vec3 direction, float dt);

	mat4 projection;
	mat4 view;
	vec3 cameraPos;
private:
	vec3 cameraDir;
	vec3 cameraRight;	
};