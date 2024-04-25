#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include"VBO.h"
#include"VAO.h"
#include"EBO.h"
#include"Transform.h"
#include"Shader.h"
#include"Material.h"

#include<vector>

class Mesh
{
	public:

		std::vector <Vertex> vertices;
		std::vector <GLuint> indices;
		Material* material = new Material();

		bool shadowCaster = true;
		bool visible = true;
		bool triplanar = false;
		bool topLayer = false;

		VAO vao;

		Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, Material* material, bool tangentsCalculated, bool triplanar, bool shadowCaster = true, bool topLayer = false);
		void Draw(glm::mat4 matrix);
		void Draw(glm::mat4 matrix, Shader& shader);
		Mesh() = default;

	private:
		void CalculateTangents();
		void CalculateUVS();
};

#endif

