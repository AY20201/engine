#include"../engine_headers/Behavior.h"
#include"../engine_headers/GameObject.h"

void Behavior::Initialize(GameObject* parentObj)
{
	Behavior::parentObj = parentObj;
}

void Behavior::Awake()
{
	
}

void Behavior::Update(float deltaTime, GLFWwindow* window)
{
	
}