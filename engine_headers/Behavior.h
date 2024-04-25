#ifndef BEHAVIOR_CLASS_H
#define BEHAVIOR_CLASS_H

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

class GameObject;

class Behavior
{
public:
	GameObject* parentObj = nullptr;

	void Initialize(GameObject* parentObj);
	virtual void Awake();
	virtual void Update(float deltaTime, GLFWwindow* window);
};

#endif