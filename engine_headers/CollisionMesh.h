#ifndef COLLISIONMESH_CLASS_H
#define COLLISIONMESH_CLASS_H

#include"CollisionObject.h"
#include"Mesh.h"
#include"AABB.h"

class GameObject;
class Raycast;

class CollisionMesh
{
public:
	std::vector <Vertex> vertices;
	std::vector <GLuint> indices;

	bool hasBoxCollider;
	bool enabled;

	AABB boxCollider;
	GameObject* connectObject;

	CollisionMesh();
	CollisionMesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, glm::mat4 transformMatrix, GameObject* connectedGO, bool hasBoxCollider);
	void CheckRaycast(Raycast& ray);
};

#endif
