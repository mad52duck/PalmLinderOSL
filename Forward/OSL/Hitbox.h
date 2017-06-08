#pragma once
#include <glm\common.hpp>

class Hitbox {
public:
	glm::vec3 position;
	float radius;
public:
	Hitbox();
	void init(glm::vec3 pos, float r);
	bool intersect(Hitbox* other);
};