#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<glm/gtx/euler_angles.hpp>

#include"Shader.h"

class Camera
{
	public:
		glm::vec3 Position;
		glm::vec3 Orientation = glm::vec3(0.0f, -0.25f, -1.0f);
		glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 translation = glm::mat4(1.0f);

		bool firstClick = true;

		int width;
		int height;

		float speed = 0.05f;
		float sensitivity = 100.0f;

		//empty constructor for uninitalized camera in player class
		//Camera();
		Camera(int width, int height, glm::vec3 position);
		Camera() = default;

		void UpdateMatrix(float fov, float nearClipPlane, float farClipPlane);
		void SetMatrices(Shader& shader, const char* viewUniform, const char* projUniform);
		void SetViewMatrix(Shader& shader, const char* uniform);
		void SetProjectionMatrix(Shader& shader, const char* uniform);
		void FlyController(GLFWwindow* window);
		void Look(GLFWwindow* window);
};

#endif