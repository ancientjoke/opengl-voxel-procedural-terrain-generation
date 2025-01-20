#include "../headers/chunk.h"

Chunk::Chunk(int i, int j, int k, utils::NoiseMap heightMap) {
    memset(blk, 0, sizeof(blk));
    elements = 0;
    changed = true;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Setting chunk position relative to map
    posX = i * CX;
    posY = j * CY;
    posZ = k * CZ;
    float maxHeightValue = (CY * SCY - 1);
    for (int x = 0; x < CX; x++) {
        for (int z = 0; z < CZ; z++) {
            // Get height value for every x, z position
            float heightmapValue = (heightMap.GetValue(x, z) + 1) / 2;
            float heightValue = heightmapValue * maxHeightValue - posY;
            if (heightValue > CY) heightValue = CY;
            // Create a bottom layer for the lowest chunk so no x, z position is empty
            if (posY == 0 && heightValue < 1) heightValue = 1;
            // Spawn cubes until it reaches height value
            for (int y = 0; y < heightValue; y++) {
                if((y + posY) == maxHeightValue)
                    blk[x][y][z] = BlockType_Snow;
                else if ((y + posY) > (maxHeightValue * 0.75))
                    blk[x][y][z] = BlockType_Rock;
                else if ((y + posY) > (maxHeightValue * 0.50))
                    blk[x][y][z] = BlockType_Dirt;
                else if ((y + posY) > (maxHeightValue * 0.15))
                    blk[x][y][z] = BlockType_Grass;
                else if ((y + posY) > (maxHeightValue * 0.05))
                    blk[x][y][z] = BlockType_Sand;
                else
                    blk[x][y][z] = BlockType_Shallow;
            }
        }
    }
}

Chunk::~Chunk() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

uint8_t Chunk::GetNeighbourBlock(Chunk* neighbour, int x, int y, int z) {
    if (neighbour)
        return neighbour->blk[x][y][z];
    return 0;
}

// Where this chunk's mesh is created
void Chunk::Update() {
    changed = false;

    CubeVertex vertex[CX * CY * CZ * 2];
    int i = 0;

    for (int x = 0; x < CX; x++) {
        for (int y = 0; y < CY; y++) {
            for (int z = 0; z < CZ; z++) {
                uint8_t type = blk[x][y][z];

                // Empty block?
                if (!type)
                    continue;

                // Culling unvisible faces below

                // View from negative x
                if ((x == 0 && !GetNeighbourBlock(neighXN, CX - 1, y, z)) || 
                    (x > 0 && !blk[x - 1][y][z])) {
                        // bottom triangle
                        vertex[i++] = CubeVertex(x    , y    , z    , type, -1, 0, 0); // bottom left
                        vertex[i++] = CubeVertex(x    , y    , z + 1, type, -1, 0, 0); // bottom right
                        vertex[i++] = CubeVertex(x    , y + 1, z    , type, -1, 0, 0); // top left
                        // top triangle
                        vertex[i++] = CubeVertex(x    , y + 1, z    , type, -1, 0, 0); // top left
                        vertex[i++] = CubeVertex(x    , y    , z + 1, type, -1, 0, 0); // bottom right
                        vertex[i++] = CubeVertex(x    , y + 1, z + 1, type, -1, 0, 0); // top right
                }
                // View from positive x
                if ((x == CX - 1 && !GetNeighbourBlock(neighXP, 0, y, z)) ||
                    (x < CX - 1 && !blk[x + 1][y][z])) {
                        // bottom triangle
                        vertex[i++] = CubeVertex(x + 1, y    , z    , type, 1, 0, 0); // bottom left
                        vertex[i++] = CubeVertex(x + 1, y + 1, z    , type, 1, 0, 0); // bottom right
                        vertex[i++] = CubeVertex(x + 1, y    , z + 1, type, 1, 0, 0); // top left
                        // top triangle
                        vertex[i++] = CubeVertex(x + 1, y + 1, z    , type, 1, 0, 0); // top left
                        vertex[i++] = CubeVertex(x + 1, y + 1, z + 1, type, 1, 0, 0); // bottom right
                        vertex[i++] = CubeVertex(x + 1, y    , z + 1, type, 1, 0, 0); // top right
                }
                // View from negative y
                if ((y == 0 && !GetNeighbourBlock(neighYN, x, CY - 1, z)) ||
                    (y > 0 && !blk[x][y - 1][z])) {
                        // bottom triangle
                        vertex[i++] = CubeVertex(x    , y    , z    , type, 0, -1, 0); // bottom left
                        vertex[i++] = CubeVertex(x + 1, y    , z    , type, 0, -1, 0); // bottom right
                        vertex[i++] = CubeVertex(x    , y    , z + 1, type, 0, -1, 0); // top left
                        // top triangle
                        vertex[i++] = CubeVertex(x + 1, y    , z    , type, 0, -1, 0); // top left
                        vertex[i++] = CubeVertex(x + 1, y    , z + 1, type, 0, -1, 0); // bottom right
                        vertex[i++] = CubeVertex(x    , y    , z + 1, type, 0, -1, 0); // top right
                }
                // View from positive y
                if ((y == CY - 1 && !GetNeighbourBlock(neighYP, x, 0, z)) ||
                    (y < CY - 1 && !blk[x][y + 1][z])) {
                        // bottom triangle
                        vertex[i++] = CubeVertex(x    , y + 1, z    , type, 0, 1, 0); // bottom left
                        vertex[i++] = CubeVertex(x    , y + 1, z + 1, type, 0, 1, 0); // bottom right
                        vertex[i++] = CubeVertex(x + 1, y + 1, z    , type, 0, 1, 0); // top left
                        // top triangle
                        vertex[i++] = CubeVertex(x + 1, y + 1, z    , type, 0, 1, 0); // top left
                        vertex[i++] = CubeVertex(x    , y + 1, z + 1, type, 0, 1, 0); // bottom right
                        vertex[i++] = CubeVertex(x + 1, y + 1, z + 1, type, 0, 1, 0); // top right
                }
                // View from negative z
                if ((z == 0 && !GetNeighbourBlock(neighZN, x, y, CZ - 1)) ||
                    (z > 0 && !blk[x][y][z - 1])) {
                        // bottom triangle
                        vertex[i++] = CubeVertex(x    , y    , z    , type, 0, 0, -1); // bottom left
                        vertex[i++] = CubeVertex(x    , y + 1, z    , type, 0, 0, -1); // bottom right
                        vertex[i++] = CubeVertex(x + 1, y    , z    , type, 0, 0, -1); // top left
                        // top triangle
                        vertex[i++] = CubeVertex(x    , y + 1, z    , type, 0, 0, -1); // top left
                        vertex[i++] = CubeVertex(x + 1, y + 1, z    , type, 0, 0, -1); // bottom right
                        vertex[i++] = CubeVertex(x + 1, y    , z    , type, 0, 0, -1); // top right
                }
                // View from positive z
                if ((z == CZ - 1 && !GetNeighbourBlock(neighZP, x, y, 0)) ||
                    (z < CZ - 1 && !blk[x][y][z + 1])) {
                        // bottom triangle
                        vertex[i++] = CubeVertex(x    , y    , z + 1, type, 0, 0, 1); // bottom left
                        vertex[i++] = CubeVertex(x + 1, y    , z + 1, type, 0, 0, 1); // bottom right
                        vertex[i++] = CubeVertex(x    , y + 1, z + 1, type, 0, 0, 1); // top left
                        // top triangle
                        vertex[i++] = CubeVertex(x    , y + 1, z + 1, type, 0, 0, 1); // top left
                        vertex[i++] = CubeVertex(x + 1, y    , z + 1, type, 0, 0, 1); // bottom right
                        vertex[i++] = CubeVertex(x + 1, y + 1, z + 1, type, 0, 0, 1); // top right
                }
            }
        }
    }
    elements = i;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, elements * sizeof(*vertex), vertex, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(CubeVertex), (void*)0);
    glEnableVertexAttribArray(0);
    // cube type attribute
    glVertexAttribIPointer(1, 1, GL_BYTE, sizeof(CubeVertex), (void*)(3 * sizeof(uint8_t)));
    glEnableVertexAttribArray(1);
    // normal attribute
    glVertexAttribPointer(2, 3, GL_BYTE, GL_FALSE, sizeof(CubeVertex), (void*)(4 * sizeof(uint8_t)));
    glEnableVertexAttribArray(2);
}

void Chunk::Render() {
    if (changed)
        Update();

    // If this chunk is empty, we don't need to draw anything.
    if (!elements)
        return;

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, elements);
}

void Chunk::setNeighbours(Chunk* cXN, Chunk* cXP, Chunk* cYN, Chunk* cYP, Chunk* cZN, Chunk* cZP) {
    // If neighbours changed, we need to rebuild our mesh
    if (neighXN != cXN) {
        neighXN = cXN;
        changed = true;
    }
    if (neighXP != cXP) {
        neighXP = cXP;
        changed = true;
    }
    if (neighYN != cYN) {
        neighYN = cYN;
        changed = true;
    }
    if (neighYP != cYP) {
        neighYP = cYP;
        changed = true;
    }
    if (neighZN != cZN) {
        neighZN = cZN;
        changed = true;
    }
    if (neighZP != cZP) {
        neighZP = cZP;
        changed = true;
    } 
}