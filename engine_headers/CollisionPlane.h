#ifndef COLLISIONPLANE_CLASS_H
#define COLLISIONPLANE_CLASS_H

#include"Transform.h"

class CollisionPlane
{
public:
	float equation[4];
	glm::vec3 origin;
	glm::vec3 normal;

	CollisionPlane(glm::vec3 origin, glm::vec3 normal);
	CollisionPlane(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	CollisionPlane() = default;

	bool IsFrontFacingTo(glm::vec3 direction);
	double SignedDistanceTo(glm::vec3 point);
};

#endif
