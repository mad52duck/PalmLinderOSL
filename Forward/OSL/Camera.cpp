#include "Camera.h"
#include <iostream>

Camera::Camera()
{
	this->cameraPos = { 53.0, 20.0, 15.0 };
	this->cameraDir = { -0.8, -0.4, -0.0 };
	this->cameraRight = cross(vec3(0.0, 1.0, 0.0), this->cameraDir);

	this->view = glm::lookAt(this->cameraPos,
		this->cameraPos + this->cameraDir,
		glm::vec3(0.0, 1.0, 0.0));
	this->projection = glm::perspective(45.f, (float) SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.f);
}

Camera::~Camera()
{
}

void Camera::update(double xpos, double ypos, float dt)
{
	dt *= 0.1;
	this->cameraDir +=  this->cameraRight * (float)xpos / 400.f;
	this->cameraDir.y += (float)ypos /400.f;
	this->cameraDir = normalize(cameraDir);
	this->cameraRight = cross(vec3(0.0, 1.0, 0.0), this->cameraDir);
	this->view = glm::lookAt(this->cameraPos,
		this->cameraPos + this->cameraDir,
		glm::vec3(0.0, 1.0, 0.0));
}

glm::mat4 Camera::getViewProjection()
{
	return this->projection * this->view;
}

void Camera::move(glm::vec3 direction, float dt)
{
	this->cameraPos += this->cameraDir * direction.z * 10.f * dt;
	this->cameraPos += this->cameraRight * direction.x * 10.f * dt;
	this->cameraPos += glm::vec3(0, 1, 0)*direction.y * 10.f*dt;
}