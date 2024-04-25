#ifndef GAMEOBJECT_CLASS_H
#define GAMEOBJECT_CLASS_H

#include"Transform.h"
#include"Mesh.h"
#include"CollisionMesh.h"
#include<vector>

class Behavior;
class CollisionMesh;

class GameObject
{
	public:

		Transform transform;
		std::vector<Mesh> meshes;
		std::vector<CollisionMesh> collisionMeshes;
		Behavior* behavior;

		int id;

		GameObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::vector<Mesh> meshes, std::vector<CollisionMesh> CollisionMeshes, Behavior* behavior);
		void Enable(bool enable);

};

#endif

