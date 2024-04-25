#include "../engine_headers/Transform.h"

Transform Transform::Zero(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));

Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	Transform::position = position;
	Transform::rotation = rotation;
	Transform::scale = scale;

	matrix = glm::translate(matrix, position);
	matrix = glm::scale(matrix, scale);
	matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Transform::RecalculateMatrix()
{
	matrix = glm::mat4(1.0f);

	matrix = glm::translate(matrix, position);
	matrix = glm::scale(matrix, scale);
	matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}