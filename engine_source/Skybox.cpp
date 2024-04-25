#include"../engine_headers/Skybox.h"

Skybox::Skybox(std::vector<const char*> faceTexPaths, Shader& shader)
{
	Skybox::skyBoxShader = shader;

	InitializeTexture(faceTexPaths);

	//skyboxVertices = std::vector<Vertex>(skyboxVerticesArray, skyboxVerticesArray + sizeof(skyboxVerticesArray) / sizeof(Vertex));
	SetVertexBuffers();
}

void Skybox::Draw()
{
	glDepthFunc(GL_LEQUAL);

	skyBoxShader.Activate();
	glBindVertexArray(vaoID);

	glUniform1i(glGetUniformLocation(skyBoxShader.ID, "skybox"), texUnit);
	Bind();

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}

void Skybox::Bind()
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

void Skybox::Unbind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::InitializeTexture(std::vector<const char*> faceTexPaths)
{
	glGenTextures(1, &ID);
	texUnit = Texture::activeTexUnit;
	Texture::activeTexUnit += 1;
	Bind();

	int imgWidth, imgHeight, numColChannels;
	stbi_set_flip_vertically_on_load(false);
	//right, left, up, down, front, back
	for (unsigned int i = 0; i < faceTexPaths.size(); i++)
	{
		unsigned char* bytes = stbi_load(faceTexPaths[i], &imgWidth, &imgHeight, &numColChannels, 0);
		if (bytes)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
			stbi_image_free(bytes);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faceTexPaths[i] << std::endl;
			stbi_image_free(bytes);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	Unbind();

	stbi_set_flip_vertically_on_load(true);
}

void Skybox::SetVertexBuffers()
{
	glGenVertexArrays(1, &vaoID);

	glBindVertexArray(vaoID);

	GLuint vboID;
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	//vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}