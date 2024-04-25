#include "../engine_headers/Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::UpdateMatrix(float fov, float nearClipPlane, float farClipPlane)
{
	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);

	view = glm::lookAt(Position, Position + Orientation, Up);
	projection = glm::perspective(glm::radians(fov), (float)width / (float)height, nearClipPlane, farClipPlane);

	translation = glm::translate(translation, Position);
	translation = glm::rotate(translation, glm::radians(Orientation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	translation = glm::rotate(translation, glm::radians(Orientation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	translation = glm::rotate(translation, glm::radians(Orientation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Camera::SetMatrices(Shader& shader, const char* viewUniform, const char* projUniform)
{
	shader.Activate();

	glUniformMatrix4fv(glGetUniformLocation(shader.ID, viewUniform), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, projUniform), 1, GL_FALSE, glm::value_ptr(projection));
}

void Camera::SetViewMatrix(Shader& shader, const char* uniform)
{
	shader.Activate();

	int uniLocation = glGetUniformLocation(shader.ID, uniform);
	glUniformMatrix4fv(uniLocation, 1, GL_FALSE, glm::value_ptr(view));
}

void Camera::SetProjectionMatrix(Shader& shader, const char* uniform)
{
	shader.Activate();

	int uniLocation = glGetUniformLocation(shader.ID, uniform);
	glUniformMatrix4fv(uniLocation, 1, GL_FALSE, glm::value_ptr(projection));
}

void Camera::FlyController(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += speed * Orientation;
	}
		
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position += speed * -Orientation;
	}
		
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += -glm::normalize(glm::cross(Orientation, Up)) * speed;
	}
		
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += glm::normalize(glm::cross(Orientation, Up)) * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		Position += speed * Up;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position += speed * -Up;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}
		Look(window);
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		firstClick = true;
	}

}

void Camera::Look(GLFWwindow* window)
{
	//modified for golf simulator
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		glm::vec3 orientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));
		//check to prevent over rotating
		if (abs(glm::angle(orientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = orientation;
		}

		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		firstClick = true;
	}
}