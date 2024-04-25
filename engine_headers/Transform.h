#ifndef TRANSFORM_CLASS_H
#define TRANSFORM_CLASS_H

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

class Transform
{
	public:

		static Transform Zero;

		glm::mat4 matrix = glm::mat4(1.0);

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
		Transform() = default;
		void RecalculateMatrix();

};

#endif
