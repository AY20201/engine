#ifndef SKYBOX_CLASS_H
#define SKYBOX_CLASS_H

#include"Mesh.h"
#include"Texture.h"

/*
const Vertex skyboxVerticesArray[] =
{
	Vertex{glm::vec3(-1.0f, 1.0f,  -1.0f)},
	Vertex{glm::vec3(-1.0f, -1.0f,  -1.0f)},
	Vertex{glm::vec3(1.0f, -1.0f,  -1.0f)},
	Vertex{glm::vec3(1.0f, -1.0f,  -1.0f)},
	Vertex{glm::vec3(1.0f, 1.0f,  -1.0f)},
	Vertex{glm::vec3(-1.0f, 1.0f,  -1.0f)},

	Vertex{glm::vec3(-1.0f, -1.0f,  1.0f)},
	Vertex{glm::vec3(-1.0f, -1.0f,  -1.0f)},
	Vertex{glm::vec3(-1.0f, 1.0f,  -1.0f)},
	Vertex{glm::vec3(-1.0f, 1.0f,  -1.0f)},
	Vertex{glm::vec3(-1.0f, 1.0f,  1.0f)},
	Vertex{glm::vec3(-1.0f, -1.0f,  1.0f)},

	Vertex{glm::vec3(1.0f, -1.0f,  -1.0f)},
	Vertex{glm::vec3(1.0f, -1.0f,  1.0f)},
	Vertex{glm::vec3(1.0f, 1.0f,  1.0f)},
	Vertex{glm::vec3(1.0f, 1.0f,  1.0f)},
	Vertex{glm::vec3(1.0f, 1.0f,  -1.0f)},
	Vertex{glm::vec3(1.0f, -1.0f,  -1.0f)},

	Vertex{glm::vec3(-1.0f, -1.0f,  1.0f)},
	Vertex{glm::vec3(-1.0f, 1.0f,  1.0f)},
	Vertex{glm::vec3(1.0f, 1.0f,  1.0f)},
	Vertex{glm::vec3(1.0f, 1.0f,  1.0f)},
	Vertex{glm::vec3(1.0f, -1.0f,  1.0f)},
	Vertex{glm::vec3(-1.0f, -1.0f,  1.0f)},

	Vertex{glm::vec3(-1.0f, 1.0f,  -1.0f)},
	Vertex{glm::vec3(1.0f, 1.0f,  -1.0f)},
	Vertex{glm::vec3(1.0f, 1.0f,  1.0f)},
	Vertex{glm::vec3(1.0f, 1.0f,  1.0f)},
	Vertex{glm::vec3(-1.0f, 1.0f,  1.0f)},
	Vertex{glm::vec3(-1.0f, 1.0f,  -1.0f)},

	Vertex{glm::vec3(-1.0f, -1.0f,  -1.0f)},
	Vertex{glm::vec3(-1.0f, -1.0f,  1.0f)},
	Vertex{glm::vec3(1.0f, -1.0f,  -1.0f)},
	Vertex{glm::vec3(1.0f, -1.0f,  -1.0f)},
	Vertex{glm::vec3(-1.0f, -1.0f,  1.0f)},
	Vertex{glm::vec3(1.0f, -1.0f,  1.0f)}
};
*/

const GLfloat skyboxVertices[] = {
      
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

class Skybox
{
	public:

		GLuint ID;
		GLuint texUnit;

		GLuint vaoID;

		Shader skyBoxShader;

		Skybox(std::vector<const char*> faceTexPaths, Shader& skyBoxShader);
		void Bind();
		void Unbind();
		void Draw();

	private:

		void InitializeTexture(std::vector<const char*> faceTexPaths);
		void SetVertexBuffers();
};

#endif
