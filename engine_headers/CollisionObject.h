#ifndef COLLISIONOBJECT_CLASS_H
#define COLLISIONOBJECT_CLASS_H

#include"AABB.h"

class CollisionObject
{
public:
	glm::vec3 position;
	glm::vec3 velocity;

	AABB boxCollider;

	int collisionRecursionDepth = 0;
	int maxRecursionDepth = 5;

	//CollisionObject(glm::vec3 colliderRadius, glm::vec3 position, glm::vec3 velocity);
	CollisionObject(glm::vec3 boxDimensions, glm::vec3 position, glm::vec3 velocity);
	//void UpdateCollisionPacket(glm::vec3 r3Pos, glm::vec3 r3Vel);

	glm::vec3 CollideWithWorld(glm::vec3 currentVelocity, float movementSpeed, float deltaTime, glm::vec3 vmv, AABB& lastCollision);

	//void CollideAndSlide(glm::vec3 r3Vel, glm::vec3 gravity, float movementSpeed, float deltaTime);
	//glm::vec3 CollideWithWorld(glm::vec3 ePos, glm::vec3 eVel, CollisionPlane lastPlane);
};

#endif
