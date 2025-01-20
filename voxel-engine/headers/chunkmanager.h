#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include "chunk.h"
#include "main.h"
#include <math.h>
#include <memory>
#include <unordered_map>
#include "glm/gtx/hash.hpp"
#include <noise/noise.h>
#include <random>
#include <limits>

using namespace noise;

using ChunkMap = std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>>;

class ChunkManager {
private:
	glm::vec3 lastPlayerPos;
	module::Perlin myModule;
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	std::random_device rd;
public:
	// Hashmap where we store visible chunks
	ChunkMap chunks;
	ChunkManager();
	~ChunkManager();
	void Update();
	void Render(Shader shader, glm::mat4 &vp);
};
#endif