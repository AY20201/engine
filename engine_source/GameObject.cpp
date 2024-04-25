#include"../engine_headers/GameObject.h"
#include"../engine_headers/Behavior.h"
#include"../engine_headers/ObjectHandler.h"
#include"../engine_headers/CollisionMesh.h"

GameObject::GameObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::vector<Mesh> meshes, std::vector<CollisionMesh> CollisionMeshes, Behavior* behavior)
{
	Transform transform(position, rotation, scale);
	GameObject::transform = transform;
	GameObject::meshes = meshes;
	GameObject::collisionMeshes = collisionMeshes;
	GameObject::behavior = behavior;

	if (behavior != nullptr)
	{
		behavior->Initialize(this);
	}

	id = 0;
	ObjectHandler::scenes[ObjectHandler::currentSceneIndex].AddGameObject(this);
}

void GameObject::Enable(bool enable)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].visible = enable;
	}

	
	for (int i = 0; i < collisionMeshes.size(); i++)
	{
		collisionMeshes[i].enabled = enable;
	}
}