#include"../engine_headers/CollisionObject.h"
#include"../engine_headers/ObjectHandler.h"

CollisionObject::CollisionObject(glm::vec3 boxDimensions, glm::vec3 position, glm::vec3 velocity)
{
	CollisionObject::position = position;
	CollisionObject::velocity = velocity;

	boxCollider = AABB(position - boxDimensions / 2.0f, position + boxDimensions / 2.0f, glm::mat4(1.0f));
}

glm::vec3 CollisionObject::CollideWithWorld(glm::vec3 currentVelocity, float movementSpeed, float deltaTime, glm::vec3 vmv, AABB& lastCollision)
{
	//vmv = verticalMovementVector
	glm::vec3 newVelocityVector = currentVelocity;

	if (collisionRecursionDepth > maxRecursionDepth)
	{
		return currentVelocity;
	}

	collisionRecursionDepth += 1;

	for (unsigned int i = 0; i < ObjectHandler::scenes[ObjectHandler::currentSceneIndex].gameObjects.size(); i++)
	{
		for (unsigned int j = 0; j < ObjectHandler::scenes[ObjectHandler::currentSceneIndex].gameObjects[i]->collisionMeshes.size(); j++)
		{
			CollisionMesh collisionMesh = ObjectHandler::scenes[ObjectHandler::currentSceneIndex].gameObjects[i]->collisionMeshes[j];
			if (collisionMesh.hasBoxCollider && collisionMesh.enabled && collisionMesh.vertices.size() > 0)
			{
				AABB aabb = collisionMesh.boxCollider;

				if (!(lastCollision.extentsMax == aabb.extentsMax && lastCollision.extentsMin == aabb.extentsMin))
				{
					if (boxCollider.CollideWithAABB(aabb))
					{
						newVelocityVector = boxCollider.GetNewVelocity(aabb, currentVelocity, vmv);

						glm::vec3 newPosition = position + newVelocityVector * movementSpeed * deltaTime;

						boxCollider.TransformExtents(glm::translate(glm::mat4(1.0f), newPosition));

						return CollideWithWorld(newVelocityVector, movementSpeed, deltaTime, vmv, aabb);
					}
				}
			}
		}
	}
	
	return currentVelocity;
}

