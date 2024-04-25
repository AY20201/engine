#include"../engine_headers/ObjectHandler.h"

std::vector<ObjectHandler> ObjectHandler::scenes;
int ObjectHandler::currentSceneIndex = 0;

void ObjectHandler::AddGameObject(GameObject* gameObject)
{
	gameObjects.push_back(gameObject);
}

GameObject* ObjectHandler::GetGameObject(int id)
{
	return gameObjects[id];
}

void ObjectHandler::DrawMeshes() //0 = skip transparent, 1 = draw both, 2 = only transparent
{
	for (unsigned int i = 0; i < gameObjects.size(); i++)
	{
		for (unsigned int j = 0; j < gameObjects[i]->meshes.size(); j++)
		{
			gameObjects[i]->meshes[j].Draw(gameObjects[i]->transform.matrix);
		}
	}
}

void ObjectHandler::DrawMeshes(Shader& shader)
{
	for (unsigned int i = 0; i < gameObjects.size(); i++)
	{
		for (unsigned int j = 0; j < gameObjects[i]->meshes.size(); j++)
		{
			gameObjects[i]->meshes[j].Draw(gameObjects[i]->transform.matrix, shader);
				
		}
	}
}

void ObjectHandler::Awake()
{
	for (unsigned int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->behavior != nullptr)
		{
			gameObjects[i]->behavior->Awake();
		}
	}
}

void ObjectHandler::Update(float deltaTime, GLFWwindow* window)
{
	for (unsigned int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->behavior != nullptr)
		{
			gameObjects[i]->behavior->Update(deltaTime, window);
		}
	}
}

void ObjectHandler::Delete()
{
	for (unsigned int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->behavior != nullptr)
		{
			delete gameObjects[i]->behavior;
		}
		delete gameObjects[i];
	}
}

void ObjectHandler::Reconfigure()
{
	std::vector<GameObject*> newGameObjectList;
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i] != nullptr)
		{
			newGameObjectList.push_back(gameObjects[i]);
		}
	}

	gameObjects.clear();
	gameObjects = newGameObjectList;
}


