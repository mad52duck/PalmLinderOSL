#include "Hitbox.h"
#include "glm\glm.hpp"

Hitbox::Hitbox() {
	
}

void Hitbox::init(glm::vec3 pos, float r)
{
	position = pos;
	radius = r;
}

bool Hitbox::intersect(Hitbox* other)
{
	bool result = false;
	float length = abs(glm::distance(position, other->position));
	if (radius > 1 || radius < 0.4 || abs(other->radius - 5.0f )> 0.1) {
		int a = 0;
	}
	if (length < other->radius + radius) {
		result = true;
	}
	return result;
}
