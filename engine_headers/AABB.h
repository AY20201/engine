#ifndef AABB_CLASS_H
#define AABB_CLASS_H

#include"Transform.h"
#include"Mesh.h"

class Capsule
{
public:
	glm::vec3 top;
	glm::vec3 bottom;
	float radius;

	glm::vec3 start;
	glm::vec3 end;

	Capsule(glm::vec3 top, glm::vec3 bottom, float radius);
	Capsule() = default;

	void TransformExtents(glm::mat4 transformMatrix);
};

class AABB
{
public:
	glm::vec3 extentsMax = glm::vec3(1.0f);
	glm::vec3 extentsMin = glm::vec3(1.0f);
	glm::vec3 center = glm::vec3(0.0f);

	glm::vec3 originalExtentsMax = glm::vec3(1.0f);
	glm::vec3 originalExtentsMin = glm::vec3(1.0f);
	glm::vec3 originalCenter = glm::vec3(0.0f);

	glm::mat4 lastTransformMatrix = glm::mat4(-10000.0f);
	bool isColliding = false;

	AABB(std::vector<Vertex>& vertices, glm::mat4 transformMatrix);
	AABB(glm::vec3 min, glm::vec3 max, glm::mat4 transformMatrix);
	AABB() = default;

	void RecalculateAABB(std::vector<Vertex>& vertices, glm::mat4 transformMatrix);
	void TransformExtents(glm::mat4 transformMatrix);
	bool CollideWithAABB(AABB& aabb);
	bool CollideWithSphere(glm::vec3 spherePos, float radius);
	bool CollideWithCapsule(AABB& encapsulatingAABB, Capsule& capsule);

	glm::vec3 GetNewVelocity(AABB& aabb, glm::vec3 currentVelocity, glm::vec3 vmv);
	glm::vec3 GetCollisionNormal(AABB& aabb);
};

#endif

