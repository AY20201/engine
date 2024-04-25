#ifndef RAYCAST_CLASS_H
#define RAYCAST_CLASS_H

#include"Transform.h"
#include"CollisionPlane.h"
#include"GameObject.h"

class Raycast
{
public:
	glm::vec3 origin;
	glm::vec3 direction;

	float maxDist;

	bool foundCollision;
	glm::vec3 collisionPoint;
	glm::vec3 collisionNormal;
	float time;
	float closestTime;
	GameObject* hitObject;

	void CheckRayTriangleCollision(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
};

#endif
