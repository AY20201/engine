#include"../engine_headers/ComputeShader.h"

ComputeShader::ComputeShader(const char* file)
{
	std::string code = get_file_contents(file);

	const char* source = code.c_str();

	//create vertex shader based on source code
	GLuint compute = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute, 1, &source, NULL);
	glCompileShader(compute);

	//create the shader program and attach shaders
	ID = glCreateProgram();
	glAttachShader(ID, compute);
	glLinkProgram(ID);

	textureID = 0;
	textureUnit = 0;

	attachedTextureWidth = 0;
	attachedTextureHeight = 0;

	//delete shader because they've been attach to program
	glDeleteShader(compute);
}

void ComputeShader::AttachTexture(int textureWidth, int textureHeight)
{
	textureUnit = Texture::activeTexUnit;
	Texture::activeTexUnit += 1;

	attachedTextureWidth = textureWidth;
	attachedTextureHeight = textureHeight;

	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindImageTexture(textureUnit, textureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void ComputeShader::BindTexture()
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void ComputeShader::SetTexture(Shader& shader, const char* uniformName)
{
	shader.Activate();
	glUniform1i(glGetUniformLocation(shader.ID, uniformName), textureUnit);
	BindTexture();
}

void ComputeShader::Dispatch()
{
	Activate();
	glDispatchCompute(attachedTextureWidth, attachedTextureHeight, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void ComputeShader::Activate()
{
	glUseProgram(ID);
}

void ComputeShader::Delete()
{
	glDeleteProgram(ID);
}