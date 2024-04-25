#include"../engine_headers/Raycast.h"

void Raycast::CheckRayTriangleCollision(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	CollisionPlane trianglePlane(p1, p2, p3);
	if (trianglePlane.IsFrontFacingTo(glm::normalize(direction)))
	{
		glm::vec3 edge1 = p2 - p1;
		glm::vec3 edge2 = p3 - p1;

		glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

		glm::vec3 q = glm::cross(direction, edge2);
		float a = glm::dot(edge1, q);

		if ((glm::dot(faceNormal, direction) >= 0.0f) || (fabs(a) <= 0.001f))
		{
			foundCollision = false;
			return;
		}

		glm::vec3 s = (origin - p1) / a;
		glm::vec3 r = glm::cross(s, edge1);

		//barycentric coordinates
		glm::vec3 b;
		b.x = glm::dot(s, q);
		b.y = glm::dot(r, direction);
		b.z = 1.0f - b.x - b.y;

		if ((b.x < 0.0f) || (b.y < 0.0f) || (b.z < 0.0f))
		{
			foundCollision = false;
			return;
		}

		if (glm::length(direction * glm::dot(edge2, r)) > maxDist)
		{
			foundCollision = false;
			return;
		}

		time = glm::dot(edge2, r);

		foundCollision = (time >= 0.0f);
		collisionPoint = origin + direction * time;
		collisionNormal = faceNormal;
	}
}