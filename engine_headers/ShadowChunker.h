#ifndef SHADOWCHUNKER_CLASS_H
#define SHADOWCHUNKER_CLASS_H

#include"Transform.h"

class ShadowChunker
{
public:
	glm::vec3 currentChunkPos;
	float chunkSize;

	ShadowChunker(float chunkSize);
	void Update(glm::vec3 playerPosition);
};

#endif