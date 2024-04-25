#include"../engine_headers/CollisionPlane.h"

CollisionPlane::CollisionPlane(glm::vec3 origin, glm::vec3 normal)
{
	CollisionPlane::origin = origin;
	CollisionPlane::normal = normal;
	equation[0] = normal.x;
	equation[1] = normal.y;
	equation[2] = normal.z;
	equation[3] = -(normal.x * origin.x + normal.y * origin.y + normal.z * origin.z);
}

CollisionPlane::CollisionPlane(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	normal = glm::cross(p2 - p1, p3 - p1);
	normal = glm::normalize(normal);
	origin = p1;

	equation[0] = normal.x;
	equation[1] = normal.y;
	equation[2] = normal.z;
	equation[3] = -(normal.x * origin.x + normal.y * origin.y + normal.z * origin.z);
}

bool CollisionPlane::IsFrontFacingTo(glm::vec3 direction)
{
	double dot = glm::dot(normal, direction);
	return (dot <= 0);
}

double CollisionPlane::SignedDistanceTo(glm::vec3 point)
{
	return (glm::dot(point, normal)) + (double)equation[3];
}