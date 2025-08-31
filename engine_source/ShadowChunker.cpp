#include"../engine_headers/ShadowChunker.h"

ShadowChunker::ShadowChunker(float chunkSize)
{
	currentChunkPos = glm::vec3(0.0f, 0.0f, 0.0f);
	ShadowChunker::chunkSize = chunkSize;
};

void ShadowChunker::Update(glm::vec3 playerPosition)
{
	float x = chunkSize * (int)(playerPosition.x / chunkSize);
	float y = chunkSize * (int)(playerPosition.y / chunkSize);
	float z = chunkSize * (int)(playerPosition.z / chunkSize);

	currentChunkPos = glm::vec3(x, y, z);
}