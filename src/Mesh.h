#pragma once
#include <vector>
#include <cstdint>
#include <glad/glad.h>

#include "Perlin.h"
const int CHUNK_SIZE = 64;


inline Perlin worldGenerator(123762);

class ChunkMesh {
public: 
    int chunkX, chunkZ;
    unsigned int VAO, VBO;
     uint8_t chunkData[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
    std::vector<float> meshVertices;
    ChunkMesh(int x, int z){
        VBO = 0;
        VAO = 0;
        chunkX = x;
        chunkZ = z;
    }

    void populateChunk() {
        int WATER_LEVEL = 8
        ;
        for(int x = 0; x < CHUNK_SIZE; x++){
            for(int z = 0; z < CHUNK_SIZE; z++){
                
                float globalX = (chunkX * CHUNK_SIZE) + x;
                float globalZ = (chunkZ * CHUNK_SIZE) + z;

                float scale = 0.05f;
                float noiseVal = (worldGenerator.noise(globalX * scale, globalZ * scale) + 1.0f) / 2.0f;
                float steepNoise = std::pow(noiseVal, 1.5f);
                int terrainHeight = static_cast<int>(steepNoise * 40.0f);


                if (terrainHeight < 1) terrainHeight = 1;
                if (terrainHeight >= CHUNK_SIZE) terrainHeight = CHUNK_SIZE - 1;

                for(int y = 0; y < CHUNK_SIZE; y++){
                    if(y == terrainHeight) {
                        chunkData[x][y][z] = 1; 
                    }
                    else if (y==0){
                        chunkData[x][y][z] = 11;
                    }
                    else if(y<terrainHeight && y>=terrainHeight-2){
                        chunkData[x][y][z] = 4; 
                    }
                    else if (y<terrainHeight-2){
                        chunkData[x][y][z] = 3;
                    }
                    else if (y <= WATER_LEVEL) {
                        chunkData[x][y][z] = 2; 
                    }
                    
                    else {
                        chunkData[x][y][z] = 0; // Air
                    }

                    if (chunkData[x][y][z] == 3) {
                        float andesiteScale = 0.08f; 

                        float aNoise = worldGenerator.noise(
                            (globalX + 5000.0f) * andesiteScale, 
                            (y + 5000.0f) * andesiteScale, 
                            (globalZ + 5000.0f) * andesiteScale
                        );

                        if (aNoise > 0.5f) {
                            chunkData[x][y][z] = 10; 
                        }
                    }
                                    // 2. CARVE WORM TUNNELS
                    if (y <= terrainHeight-(rand()%11) && chunkData[x][y][z] != 2) {
                        
                        float tunnelScale = 0.03f;
                        float tunnelRadius = 0.08f; // How thick the tube is

                        // Sample two different "noise networks" by heavily offsetting the coordinates of the second one
                        float noise1 = worldGenerator.noise(globalX * tunnelScale, y * tunnelScale, globalZ * tunnelScale);
                        float noise2 = worldGenerator.noise((globalX + 1000) * tunnelScale, (y + 1000) * tunnelScale, (globalZ + 1000) * tunnelScale);

                        // If BOTH noises are very close to 0, we are inside the tube!
                        if (std::abs(noise1) < tunnelRadius && std::abs(noise2) < tunnelRadius && chunkData[x][y][z] != 11) {
                            chunkData[x][y][z] = 0; // Carve tunnel
                        }
                    }
                }

            }
        }
        for(int x = 2; x < CHUNK_SIZE - 2; x++){
            for(int z = 2; z < CHUNK_SIZE - 2; z++){
                for(int y = 1; y < CHUNK_SIZE - 6; y++){
                    
                    if (chunkData[x][y][z] == 1 && chunkData[x][y+1][z] == 0) {
                        
                        if (rand() % 200 < 1) {
                            SpawnTree(x, y + 1, z);
                        }
                    }
                }
            }
        }
    }
    void SpawnTree(int rootX, int rootY, int rootZ){
    int trunkHeight = 4 + (rand() % 3);
    for (int i = 0; i < trunkHeight; i++) {
            int y = rootY + i;
            if (y < CHUNK_SIZE) {
                chunkData[rootX][y][rootZ] = 5; 
            }
    }

    int leafBottom = rootY + trunkHeight - 2;
        int leafTop = rootY + trunkHeight + 1;

        for (int y = leafBottom; y <= leafTop; y++) {
            for (int x = rootX - 2; x <= rootX + 2; x++) {
                for (int z = rootZ - 2; z <= rootZ + 2; z++) {
                    
                    if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE) {
                        
                        if (chunkData[x][y][z] != 5) {
                            chunkData[x][y][z] = 6; 
                        }
                    }
                }
            }
        }
}

    void buildMesh() {
        meshVertices.clear();

        // --- GENERIC CUBE FACES (X, Y, Z, U, V) ---
        float topFace[] = {
            0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
            1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
            1.0f, 1.0f, 0.0f,   1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,   1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
            0.0f, 1.0f, 1.0f,   0.0f, 1.0f
        };
        float bottomFace[] = {
            0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
            1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
            1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
            0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
            0.0f, 0.0f, 0.0f,   0.0f, 0.0f
        };
        float frontFace[] = {
            0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
            1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
            1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,   0.0f, 0.0f
        };
        float backFace[] = {
            1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
            0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
            1.0f, 1.0f, 0.0f,   0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,   0.0f, 0.0f
        };
        float leftFace[] = {
            0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
            0.0f, 1.0f, 1.0f,   1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,   1.0f, 1.0f,
            0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
            0.0f, 0.0f, 0.0f,   0.0f, 0.0f
        };
        float rightFace[] = {
            1.0f, 0.0f, 1.0f,   0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
            1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,   0.0f, 0.0f
        };
        // --- WATER CUBE FACES (Y is dipped to 0.85f) ---
        float wTopFace[] = {
            0.0f, 0.85f, 1.0f,   0.0f, 1.0f,
            1.0f, 0.85f, 1.0f,   1.0f, 1.0f,
            1.0f, 0.85f, 0.0f,   1.0f, 0.0f,
            1.0f, 0.85f, 0.0f,   1.0f, 0.0f,
            0.0f, 0.85f, 0.0f,   0.0f, 0.0f,
            0.0f, 0.85f, 1.0f,   0.0f, 1.0f
        };
        float wBottomFace[] = {
            0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
            1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
            1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
            0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
            0.0f, 0.0f, 0.0f,   0.0f, 0.0f
        };
        float wFrontFace[] = {
            0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
            1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
            1.0f, 0.85f, 1.0f,   1.0f, 1.0f,
            1.0f, 0.85f, 1.0f,   1.0f, 1.0f,
            0.0f, 0.85f, 1.0f,   0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,   0.0f, 0.0f
        };
        float wBackFace[] = {
            1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
            0.0f, 0.85f, 0.0f,   1.0f, 1.0f,
            0.0f, 0.85f, 0.0f,   1.0f, 1.0f,
            1.0f, 0.85f, 0.0f,   0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,   0.0f, 0.0f
        };
        float wLeftFace[] = {
            0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
            0.0f, 0.85f, 1.0f,   1.0f, 1.0f,
            0.0f, 0.85f, 1.0f,   1.0f, 1.0f,
            0.0f, 0.85f, 0.0f,   0.0f, 1.0f,
            0.0f, 0.0f, 0.0f,   0.0f, 0.0f
        };
        float wRightFace[] = {
            1.0f, 0.0f, 1.0f,   0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
            1.0f, 0.85f, 0.0f,   1.0f, 1.0f,
            1.0f, 0.85f, 0.0f,   1.0f, 1.0f,
            1.0f, 0.85f, 1.0f,   0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,   0.0f, 0.0f
        };
        //meshing
        for(int x = 0; x < CHUNK_SIZE; x++){
            for(int y = 0; y < CHUNK_SIZE; y++){
                for(int z = 0; z < CHUNK_SIZE; z++){
                    
                    int currentBlock = chunkData[x][y][z];
                    if(currentBlock == 0) continue; // skip air
                    auto checkNeighbor = [&](int nx, int ny, int nz) {
                        if (nx < 0 || nx >= CHUNK_SIZE || ny < 0 || ny >= CHUNK_SIZE || nz < 0 || nz >= CHUNK_SIZE) return true;
                        
                        int neighbor = chunkData[nx][ny][nz];
                        if(currentBlock == 11) return neighbor == 0;
                        if (currentBlock == 1 || currentBlock == 3 ||currentBlock ==4 ||currentBlock==5 ||currentBlock==6 ||currentBlock == 7
                        || currentBlock == 8 ||currentBlock == 9 ||currentBlock==10)
                             return neighbor == 0 || neighbor ==2 ||neighbor == 11; // Grass renders against Air(0) and Water(2)
                        
                        if (currentBlock == 2 ) return neighbor == 0; // Water renders ONLY against Air(0)
                        
                        return false;
                    };

                    struct BlockDefinition {
                        float* top; float* bottom; float* right;
                        float* left; float* front; float* back;
                    };

                    // Inside your 3D for-loop (x, y, z):

                int blockID = chunkData[x][y][z];

                if (blockID == 0) {
                    continue; // Skip air blocks entirely!
                }

                // Draw the correct textures based on the ID
                // Draw the correct textures based on the ID, but ONLY if the face is touching air/water!
                switch (blockID) {
                    case 1: // GRASS
                        if(checkNeighbor(x, y+1, z)) addFace(topFace, x, y, z, 8, 2);    
                        if(checkNeighbor(x, y-1, z)) addFace(bottomFace, x, y, z, 2, 0); 
                        if(checkNeighbor(x, y, z+1)) addFace(frontFace, x, y, z, 3, 0);  
                        if(checkNeighbor(x, y, z-1)) addFace(backFace, x, y, z, 3, 0);   
                        if(checkNeighbor(x-1, y, z)) addFace(leftFace, x, y, z, 3, 0);   
                        if(checkNeighbor(x+1, y, z)) addFace(rightFace, x, y, z, 3, 0);  
                        break;

                    case 2: // WATER 
                        if(checkNeighbor(x, y+1, z)) addFace(wTopFace, x, y, z, 13, 12); 
                        if(checkNeighbor(x, y-1, z)) addFace(wBottomFace, x, y, z, 13, 12);
                        if(checkNeighbor(x, y, z+1)) addFace(wFrontFace, x, y, z, 13, 12);
                        if(checkNeighbor(x, y, z-1)) addFace(wBackFace, x, y, z, 13, 12);
                        if(checkNeighbor(x-1, y, z)) addFace(wLeftFace, x, y, z, 13, 12);
                        if(checkNeighbor(x+1, y, z)) addFace(wRightFace, x, y, z, 13, 12);
                        break;

                    case 3: // STONE
                        if(checkNeighbor(x, y+1, z)) addFace(topFace, x, y, z, 1, 0); 
                        if(checkNeighbor(x, y-1, z)) addFace(bottomFace, x, y, z, 1, 0);
                        if(checkNeighbor(x, y, z+1)) addFace(frontFace, x, y, z, 1, 0);
                        if(checkNeighbor(x, y, z-1)) addFace(backFace, x, y, z, 1, 0);
                        if(checkNeighbor(x-1, y, z)) addFace(leftFace, x, y, z, 1, 0);
                        if(checkNeighbor(x+1, y, z)) addFace(rightFace, x, y, z, 1, 0);
                        break;

                    case 4: // DIRT
                        if(checkNeighbor(x, y+1, z)) addFace(topFace, x, y, z, 2, 0); 
                        if(checkNeighbor(x, y-1, z)) addFace(bottomFace, x, y, z, 2, 0);
                        if(checkNeighbor(x, y, z+1)) addFace(frontFace, x, y, z, 2, 0);
                        if(checkNeighbor(x, y, z-1)) addFace(backFace, x, y, z, 2, 0);
                        if(checkNeighbor(x-1, y, z)) addFace(leftFace, x, y, z, 2, 0);
                        if(checkNeighbor(x+1, y, z)) addFace(rightFace, x, y, z, 2, 0);
                        break;

                    case 5: // WOOD LOG
                        if(checkNeighbor(x, y+1, z)) addFace(topFace, x, y, z, 5, 1);    
                        if(checkNeighbor(x, y-1, z)) addFace(bottomFace, x, y, z, 5, 1); 
                        if(checkNeighbor(x, y, z+1)) addFace(frontFace, x, y, z, 4, 1);  
                        if(checkNeighbor(x, y, z-1)) addFace(backFace, x, y, z, 4, 1);
                        if(checkNeighbor(x-1, y, z)) addFace(leftFace, x, y, z, 4, 1);
                        if(checkNeighbor(x+1, y, z)) addFace(rightFace, x, y, z, 4, 1);
                        break;

                    case 6: // LEAVES
                        if(checkNeighbor(x, y+1, z)) addFace(topFace, x, y, z, 5, 3); 
                        if(checkNeighbor(x, y-1, z)) addFace(bottomFace, x, y, z, 5, 3);
                        if(checkNeighbor(x, y, z+1)) addFace(frontFace, x, y, z, 5, 3);
                        if(checkNeighbor(x, y, z-1)) addFace(backFace, x, y, z, 5, 3);
                        if(checkNeighbor(x-1, y, z)) addFace(leftFace, x, y, z, 5, 3);
                        if(checkNeighbor(x+1, y, z)) addFace(rightFace, x, y, z, 5, 3);
                        break;
                    case 7: // oak_plank
                        if(checkNeighbor(x, y+1, z)) addFace(topFace, x, y, z, 4, 0); 
                        if(checkNeighbor(x, y-1, z)) addFace(bottomFace, x, y, z, 4, 0);
                        if(checkNeighbor(x, y, z+1)) addFace(frontFace, x, y, z, 4, 0);
                        if(checkNeighbor(x, y, z-1)) addFace(backFace, x, y, z, 4, 0);
                        if(checkNeighbor(x-1, y, z)) addFace(leftFace, x, y, z, 4, 0);
                        if(checkNeighbor(x+1, y, z)) addFace(rightFace, x, y, z, 4, 0);
                        break;
                    case 8: // oak_plank
                        if(checkNeighbor(x, y+1, z)) addFace(topFace, x, y, z, 1, 3); 
                        if(checkNeighbor(x, y-1, z)) addFace(bottomFace, x, y, z, 1, 3);
                        if(checkNeighbor(x, y, z+1)) addFace(frontFace, x, y, z, 1, 3);
                        if(checkNeighbor(x, y, z-1)) addFace(backFace, x, y, z, 1, 3);
                        if(checkNeighbor(x-1, y, z)) addFace(leftFace, x, y, z, 1, 3);
                        if(checkNeighbor(x+1, y, z)) addFace(rightFace, x, y, z, 1, 3);
                        break;
                    case 9: // cobblestone
                        if(checkNeighbor(x, y+1, z)) addFace(topFace, x, y, z, 0, 1); 
                        if(checkNeighbor(x, y-1, z)) addFace(bottomFace, x, y, z, 0, 1);
                        if(checkNeighbor(x, y, z+1)) addFace(frontFace, x, y, z, 0, 1);
                        if(checkNeighbor(x, y, z-1)) addFace(backFace, x, y, z, 0, 1);
                        if(checkNeighbor(x-1, y, z)) addFace(leftFace, x, y, z, 0, 1);
                        if(checkNeighbor(x+1, y, z)) addFace(rightFace, x, y, z, 0, 1);
                        break;
                    case 10: // cobblestone
                        if(checkNeighbor(x, y+1, z)) addFace(topFace, x, y, z, 0, 0); 
                        if(checkNeighbor(x, y-1, z)) addFace(bottomFace, x, y, z, 0, 0);
                        if(checkNeighbor(x, y, z+1)) addFace(frontFace, x, y, z, 0, 0);
                        if(checkNeighbor(x, y, z-1)) addFace(backFace, x, y, z, 0, 0);
                        if(checkNeighbor(x-1, y, z)) addFace(leftFace, x, y, z, 0, 0);
                        if(checkNeighbor(x+1, y, z)) addFace(rightFace, x, y, z, 0, 0);
                        break;
                    case 11: // WATER 
                        if(checkNeighbor(x, y+1, z)) addFace(wTopFace, x, y, z, 15, 15); 
                        if(checkNeighbor(x, y-1, z)) addFace(wBottomFace, x, y, z, 15, 15);
                        if(checkNeighbor(x, y, z+1)) addFace(wFrontFace, x, y, z, 15, 15);
                        if(checkNeighbor(x, y, z-1)) addFace(wBackFace, x, y, z, 15, 15);
                        if(checkNeighbor(x-1, y, z)) addFace(wLeftFace, x, y, z, 15, 15);
                        if(checkNeighbor(x+1, y, z)) addFace(wRightFace, x, y, z, 15, 15);
                        break;
                    }
                    
                }
            }
        }
    }
    
    void memory() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, meshVertices.size() * sizeof(float), meshVertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    ~ChunkMesh() {
        if (VAO != 0) {
            
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
        }
    }
private:
    void addFace(float* faceVertices, int x, int y, int z, int atlasX, int atlasY) {
        float tileSize = 1.0f / 16.0f; 
        
        // The X axis is normal
        float uOffset = atlasX * tileSize;
        
        // THE FIX: We invert the Y axis so (0,0) starts at the TOP of the image
        float vOffset = (15 - atlasY) * tileSize;

        for (int i = 0; i < 30; i += 5) {
            meshVertices.push_back(faceVertices[i]     + x); 
            meshVertices.push_back(faceVertices[i + 1] + y); 
            meshVertices.push_back(faceVertices[i + 2] + z); 
            
            meshVertices.push_back((faceVertices[i + 3] * tileSize) + uOffset); 
            meshVertices.push_back((faceVertices[i + 4] * tileSize) + vOffset); 
        }
    }
};