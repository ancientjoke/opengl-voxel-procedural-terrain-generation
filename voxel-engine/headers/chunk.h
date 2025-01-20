#ifndef CHUNK_H
#define CHUNK_H

// Number of blocks inside a chunk
#define CX 16
#define CY 16
#define CZ 16

// Number of chunks rendered around the player
#define SCX 48
#define SCY 4
#define SCZ 48

#include <glad/glad.h> 
#include <glm/gtc/type_ptr.hpp>
#include "noiseutils.h"
#include <iostream>
#include <sstream>
#include <string>

struct CubeVertex {
    uint8_t x, y, z, w, nx, ny, nz;
    CubeVertex() {}
    CubeVertex(uint8_t x, uint8_t y, uint8_t z, uint8_t w, int8_t nx, int8_t ny, int8_t nz) : x(x), y(y), z(z), w(w), nx(nx), ny(ny), nz(nz) {}
};

enum BlockType: uint8_t {
    BlockType_Empty = 0,
    BlockType_Snow,
    BlockType_Rock,
    BlockType_Dirt,
    BlockType_Grass,
    BlockType_Sand,
    BlockType_Shallow,
};

class Chunk {
private:
    // uint8_t = 1 byte | type of unsigned integer of length 8 bits
    uint8_t blk[CX][CY][CZ];
    unsigned int VBO, VAO;
    int elements;
    bool changed;
public:
    Chunk *neighXN, *neighXP, *neighYN, *neighYP, *neighZN, *neighZP;
    // Global map position of this chunk 
    int posX, posY, posZ;
    Chunk(int i, int j, int k, utils::NoiseMap heightMap);
    ~Chunk();
    uint8_t GetNeighbourBlock(Chunk* neighbour, int x, int y, int z);
    void Update();
    void Render();
    void setNeighbours(Chunk* neighXN, Chunk* neighXP, Chunk* neighYN, Chunk* neighYP, Chunk* neighZN, Chunk* neighZP);
};
#endif