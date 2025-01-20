#include "../headers/chunkmanager.h"

extern Camera camera;

ChunkManager::ChunkManager() {
    // Set a random seed number for our heightmap generation
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    myModule.SetSeed(dist(mt));
    // Create random heightmap using Perlin noise
    heightMapBuilder.SetSourceModule(myModule);
    heightMapBuilder.SetDestNoiseMap(heightMap);
    heightMapBuilder.SetDestSize(CX, CZ);

    Update();
}

ChunkManager::~ChunkManager() {
    for (ChunkMap::iterator iter = chunks.begin(); iter != chunks.end(); ++iter) {
        iter->second.reset();
    }
}

// Unloading and loading chunks
void ChunkManager::Update() {
    glm::vec3 key;
    int playerPosX = floor(camera.Position[0] / CX);
    int playerPosY = floor(camera.Position[1] / CY);
    int playerPosZ = floor(camera.Position[2] / CZ);
    lastPlayerPos = glm::vec3(playerPosX, playerPosY, playerPosZ);

    // Unload chunks far away from the player
    for (ChunkMap::iterator iter = chunks.begin(); iter != chunks.end(); ++iter) {
        if (iter->second->posX / CX > playerPosX + SCX / 2 || iter->second->posX / CX < playerPosX + (-SCX / 2) ||
            iter->second->posZ / CZ > playerPosZ + SCZ / 2 || iter->second->posZ / CZ < playerPosZ + (-SCZ / 2)) {
            chunks.erase(iter);
        }
    }
 
    // Create chunks around the player
    for (int x = playerPosX + (-SCX / 2); x <= playerPosX + SCX / 2; x++)
        for (int y = 0; y < SCY; y++)
            for (int z = playerPosZ + (-SCZ / 2); z <= playerPosZ + SCZ / 2; z++) {
                key.x = x; key.y = y; key.z = z;
                if (!chunks[key]) {
                    // Heightmap has continuous data, we only need to change bounds for every chunk
                    heightMapBuilder.SetBounds(x / 10.0f, x / 10.0f + 0.1, z / 10.0f, z / 10.0f + 0.1);
                    heightMapBuilder.Build();
                    chunks[key] = std::unique_ptr<Chunk>(new Chunk(x, y, z, heightMap));
                }
            }

    // Set their neighbours
    for (ChunkMap::iterator iter = chunks.begin(); iter != chunks.end(); ++iter) {
        key.x = iter->second->posX; key.y = iter->second->posY; key.z = iter->second->posZ;
        iter->second->setNeighbours(chunks.count(glm::ivec3(key.x / CX - 1, key.y / CY, key.z / CZ)) ? chunks[glm::ivec3(key.x / CX - 1, key.y / CY, key.z / CZ)].get() : nullptr,
            chunks.count(glm::ivec3(key.x / CX + 1, key.y / CY, key.z / CZ)) ? chunks[glm::ivec3(key.x / CX + 1, key.y / CY, key.z / CZ)].get() : nullptr,
            chunks.count(glm::ivec3(key.x / CX, key.y / CY - 1, key.z / CZ)) ? chunks[glm::ivec3(key.x / CX, key.y / CY - 1, key.z / CZ)].get() : nullptr,
            chunks.count(glm::ivec3(key.x / CX, key.y / CY + 1, key.z / CZ)) ? chunks[glm::ivec3(key.x / CX, key.y / CY + 1, key.z / CZ)].get() : nullptr,
            chunks.count(glm::ivec3(key.x / CX, key.y / CY, key.z / CZ - 1)) ? chunks[glm::ivec3(key.x / CX, key.y / CY, key.z / CZ - 1)].get() : nullptr,
            chunks.count(glm::ivec3(key.x / CX, key.y / CY, key.z / CZ + 1)) ? chunks[glm::ivec3(key.x / CX, key.y / CY, key.z / CZ + 1)].get() : nullptr);
    }
}

void ChunkManager::Render(Shader shader, glm::mat4 &vp) {
    // Checking if player position changed, to load/unload chunks
    int playerPosX = floor(camera.Position[0] / CX);
    int playerPosY = floor(camera.Position[1] / CY);
    int playerPosZ = floor(camera.Position[2] / CZ);
    if (playerPosX != lastPlayerPos[0] || playerPosY != lastPlayerPos[1] || playerPosZ != lastPlayerPos[2]) {
        Update();
    }
    // Iterate through existing calling their render method
    glm::vec3 pos;
    if (!chunks.empty()) {
        for (ChunkMap::iterator iter = chunks.begin(); iter != chunks.end(); ++iter) {
            // Changing each chunk to their position so they aren't stacked on top of eachother
            pos.x = iter->second->posX; pos.y = iter->second->posY; pos.z = iter->second->posZ;
            glm::mat4 model = glm::translate(glm::mat4(1), pos);
            shader.setMat4("model", model);

            // Checking if chunk is on screen to render or not
            glm::vec4 center = (vp * model) * glm::vec4(CX / 2, CY / 2, CZ / 2, 1);
            float d = glm::length(center);
            center.x /= center.w;
            center.y /= center.w;
            // Don't render if chunk is behind the camera
            if (center.z < -CY / 2)
                continue;
            // Don't render if chunk is outside the screen
            if (fabsf(center.x) > 1 + fabsf(CY * 2 / center.w) || fabsf(center.y) > 1 + fabsf(CY * 2 / center.w))
                continue;

            iter->second->Render();
        }
    }
}