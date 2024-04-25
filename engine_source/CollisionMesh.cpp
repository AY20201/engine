#include"../engine_headers/CollisionMesh.h"
#include"../engine_headers/Raycast.h"
#include"../engine_headers/GameObject.h"

CollisionMesh::CollisionMesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, glm::mat4 transformMatrix, GameObject* connectedGO, bool hasBoxCollider)
{
	CollisionMesh::vertices = vertices;

	if (transformMatrix != glm::mat4(1.0f))
	{
		for (int i = 0; i < vertices.size(); i++)
		{
			Vertex v = vertices[i];

			CollisionMesh::vertices[i].position = glm::vec3(transformMatrix * glm::vec4(v.position.x, v.position.y, v.position.z, 1.0f));
			CollisionMesh::vertices[i].texcoord = glm::vec2(v.texcoord.x, v.texcoord.y);
			CollisionMesh::vertices[i].normal = glm::vec3(v.normal.x, v.normal.y, v.normal.z);
			CollisionMesh::vertices[i].tangent = glm::vec3(v.tangent.x, v.tangent.y, v.tangent.z);
		}
	}

	CollisionMesh::indices = indices;
	CollisionMesh::connectObject = connectedGO;
	CollisionMesh::hasBoxCollider = hasBoxCollider;

	if (hasBoxCollider)
	{
		boxCollider = AABB(vertices, transformMatrix);
	}
	enabled = true;
	//boxCollider.RecalculateAABB(vertices, transformMatrix);

	//CollisionSolver::Instance.sceneCollisionMeshes.push_back(*this);
}

CollisionMesh::CollisionMesh()
{
	vertices = std::vector<Vertex>{ };
	indices = std::vector<GLuint>{ };

	hasBoxCollider = false;
	enabled = false;

	boxCollider = AABB(vertices, glm::mat4(1.0f));
	connectObject = nullptr;
}

/*
void CollisionMesh::CheckAllTriangles(CollisionObject* collisionObject)
{
	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		Vertex& v0 = vertices[indices[i]];
		Vertex& v1 = vertices[indices[i + 1]];
		Vertex& v2 = vertices[indices[i + 2]];

		collisionObject->packet->CheckTriangleCollision(v0.position, v1.position, v2.position);
	}
}
*/

void CollisionMesh::CheckRaycast(Raycast& ray)
{
	if (enabled)
	{
		for (unsigned int i = 0; i < indices.size(); i += 3)
		{
			glm::vec3 v0 = vertices[indices[i]].position;
			glm::vec3 v1 = vertices[indices[i + 1]].position;
			glm::vec3 v2 = vertices[indices[i + 2]].position;

			ray.CheckRayTriangleCollision(v0, v1, v2);
			if (ray.foundCollision && ray.time < ray.closestTime)
			{
				ray.hitObject = connectObject;
				ray.closestTime = ray.time;
			}
		}
	}
}