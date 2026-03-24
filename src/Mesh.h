#pragma once
#include <vector>
#include <cstdint>
#include <glad/glad.h>

#include "Perlin.h"
const int CHUNK_WIDTH = 16;   
const int CHUNK_HEIGHT = 256; 


inline Perlin worldGenerator(123762);

class ChunkMesh {
public: 
    int chunkX, chunkZ;
    unsigned int VAO, VBO;
    uint8_t chunkData[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_WIDTH];
    std::vector<float> meshVertices;
    ChunkMesh(int x, int z){
        VBO = 0;
        VAO = 0;
        chunkX = x;
        chunkZ = z;
    }

    void populateChunk() {
        int WATER_LEVEL = 64
        ;
        for(int x = 0; x < CHUNK_WIDTH; x++){
            for(int z = 0; z < CHUNK_WIDTH; z++){
                
                float globalX = (chunkX * CHUNK_WIDTH) + x;
                float globalZ = (chunkZ * CHUNK_WIDTH) + z;

                float scale = 0.05f;
                float noiseVal = (worldGenerator.noise(globalX * scale, globalZ * scale) + 1.0f) / 2.0f;
                float steepNoise = std::pow(noiseVal, 1.5f);
                int terrainHeight = 60.0+static_cast<int>(steepNoise * 40.0f);


                if (terrainHeight < 1) terrainHeight = 1;
                if (terrainHeight >= CHUNK_HEIGHT) terrainHeight = CHUNK_HEIGHT - 1;

                for(int y = 0; y < CHUNK_HEIGHT; y++){
                    if(y == terrainHeight) {
                        chunkData[x][y][z] = 1; 
                    }
                    else if (y==1){
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
                    if (chunkData[x][y][z] == 3) {
                        float gravelScale = 0.10f; 

                        float aNoise = worldGenerator.noise(
                            (globalX + 6000.0f) * gravelScale, 
                            (y + 6000.0f) * gravelScale, 
                            (globalZ + 6000.0f) * gravelScale
                        );

                        if (aNoise > 0.5f) {
                            chunkData[x][y][z] = 12; 
                        }
                    }
                    
                    if (y <= terrainHeight-(rand()%11) && chunkData[x][y][z] != 2) {
                        
                        float tunnelScale = 0.03f;
                        float tunnelRadius = 0.08f; 

                        float noise1 = worldGenerator.noise(globalX * tunnelScale, y * tunnelScale, globalZ * tunnelScale);
                        float noise2 = worldGenerator.noise((globalX + 1000) * tunnelScale, (y + 1000) * tunnelScale, (globalZ + 1000) * tunnelScale);

                        if (std::abs(noise1) < tunnelRadius && std::abs(noise2) < tunnelRadius && chunkData[x][y][z] != 11) {
                            chunkData[x][y][z] = 0; 
                        }
                    }

                }

            }
        }

        auto spawnVein = [&](int blockID, int maxBlocks, int minY, int maxY, int veinsPerChunk) {
            for (int v = 0; v < veinsPerChunk; v++) {
                
                int currentX = rand() % CHUNK_WIDTH;
                int currentZ = rand() % CHUNK_WIDTH;
                int currentY = minY + (rand() % (maxY - minY + 1));

                if (chunkData[currentX][currentY][currentZ] == 3) {
                    
                    for (int b = 0; b < maxBlocks; b++) {
                        
                        if (currentX >= 0 && currentX < CHUNK_WIDTH &&
                            currentY >= 0 && currentY < CHUNK_HEIGHT &&
                            currentZ >= 0 && currentZ < CHUNK_WIDTH &&
                            chunkData[currentX][currentY][currentZ] == 3) {
                            
                            chunkData[currentX][currentY][currentZ] = blockID;
                        }

                        int direction = rand() % 6;
                        if (direction == 0) currentX++;
                        if (direction == 1) currentX--;
                        if (direction == 2) currentY++;
                        if (direction == 3) currentY--;
                        if (direction == 4) currentZ++;
                        if (direction == 5) currentZ--;
                    }
                }
            }
        };

        spawnVein(13, 12, 0, 100, 20); 

        spawnVein(14, 8, 0, 80, 15);

        spawnVein(15, 8, 0, 32, 4);

        for(int x = 2; x < CHUNK_WIDTH - 2; x++){
            for(int z = 2; z < CHUNK_WIDTH - 2; z++){
                for(int y = 1; y < CHUNK_HEIGHT- 6; y++){
                    
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
            if (y < CHUNK_HEIGHT) {
                chunkData[rootX][y][rootZ] = 5; 
            }
    }

    int leafBottom = rootY + trunkHeight - 2;
        int leafTop = rootY + trunkHeight + 1;

        for (int y = leafBottom; y <= leafTop; y++) {
            for (int x = rootX - 2; x <= rootX + 2; x++) {
                for (int z = rootZ - 2; z <= rootZ + 1; z++) {
                    
                    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_WIDTH) {
                        
                        if (chunkData[x][y][z] != 5) {
                            chunkData[x][y][z] = 6; 
                        }
                    }
                }
            }
        }
}

    void buildMesh(std::map<std::pair<int, int>, ChunkMesh*>& activeChunks) {
        meshVertices.clear();

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
        for(int x = 0; x < CHUNK_WIDTH; x++){
            for(int y = 0; y < CHUNK_HEIGHT; y++){
                for(int z = 0; z < CHUNK_WIDTH; z++){
                    
                    int currentBlock = chunkData[x][y][z];
                    if(currentBlock == 0) continue;
                    auto checkNeighbor = [&](int nx, int ny, int nz) {
                    if (ny < 0 || ny >= CHUNK_HEIGHT) return true;

                    int neighborBlock = 0;

                    if (nx < 0 || nx >= CHUNK_WIDTH || nz < 0 || nz >= CHUNK_WIDTH) {
                        
                        int neighborChunkX = chunkX;
                        int neighborChunkZ = chunkZ;
                        int localX = nx;
                        int localZ = nz;

                        if (nx < 0) { neighborChunkX--; localX = CHUNK_WIDTH - 1; }
                        else if (nx >= CHUNK_WIDTH) { neighborChunkX++; localX = 0; }

                        if (nz < 0) { neighborChunkZ--; localZ = CHUNK_WIDTH - 1; }
                        else if (nz >= CHUNK_WIDTH) { neighborChunkZ++; localZ = 0; }

                        std::pair<int, int> neighborCoord(neighborChunkX, neighborChunkZ);

                        if (activeChunks.find(neighborCoord) == activeChunks.end()) {
                            return true;
                        }

                        neighborBlock = activeChunks[neighborCoord]->chunkData[localX][ny][localZ];
                    } 
                    else {
                        neighborBlock = chunkData[nx][ny][nz];
                    }

                    if(currentBlock == 11) return neighborBlock == 0;
                    
                    if (currentBlock == 1 || currentBlock == 3 || currentBlock == 4 || currentBlock == 5 || currentBlock == 6 || currentBlock == 7 || currentBlock == 8 || currentBlock == 9 || currentBlock == 10 || currentBlock == 12 ||currentBlock == 13 ||currentBlock == 14 ||currentBlock == 15)
                        return neighborBlock == 0 || neighborBlock == 2 || neighborBlock == 11;
                        
                    if (currentBlock == 2 ) return neighborBlock == 0; 
                    
                    return false;
                };

                    struct BlockDefinition {
                        float* top; float* bottom; float* right;
                        float* left; float* front; float* back;
                    };


                int blockID = chunkData[x][y][z];

                if (blockID == 0) {
                            continue; 
                }
                switch (blockID) {
                    case 1: //Grass
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
                    case 12: // LEAVES
                        if(checkNeighbor(x, y+1, z)) addFace(topFace, x, y, z, 3, 1); 
                        if(checkNeighbor(x, y-1, z)) addFace(bottomFace, x, y, z, 3, 1);
                        if(checkNeighbor(x, y, z+1)) addFace(frontFace, x, y, z, 3, 1);
                        if(checkNeighbor(x, y, z-1)) addFace(backFace, x, y, z, 3, 1);
                        if(checkNeighbor(x-1, y, z)) addFace(leftFace, x, y, z, 3, 1);
                        if(checkNeighbor(x+1, y, z)) addFace(rightFace, x, y, z, 3, 1);
                        break;
                    case 13: //coal
                        if(checkNeighbor(x, y+1, z)) addFace(topFace, x, y, z, 2, 2); 
                        if(checkNeighbor(x, y-1, z)) addFace(bottomFace, x, y, z, 2, 2);
                        if(checkNeighbor(x, y, z+1)) addFace(frontFace, x, y, z, 2, 2);
                        if(checkNeighbor(x, y, z-1)) addFace(backFace, x, y, z, 2, 2);
                        if(checkNeighbor(x-1, y, z)) addFace(leftFace, x, y, z, 2, 2);
                        if(checkNeighbor(x+1, y, z)) addFace(rightFace, x, y, z, 2, 2);
                        break;
                    case 14: //iron
                        if(checkNeighbor(x, y+1, z)) addFace(topFace, x, y, z, 1, 2); 
                        if(checkNeighbor(x, y-1, z)) addFace(bottomFace, x, y, z, 1, 2);
                        if(checkNeighbor(x, y, z+1)) addFace(frontFace, x, y, z, 1, 2);
                        if(checkNeighbor(x, y, z-1)) addFace(backFace, x, y, z, 1, 2);
                        if(checkNeighbor(x-1, y, z)) addFace(leftFace, x, y, z, 1, 2);
                        if(checkNeighbor(x+1, y, z)) addFace(rightFace, x, y, z, 1, 2);
                        break;
                    case 15: //diamonds
                        if(checkNeighbor(x, y+1, z)) addFace(topFace, x, y, z, 2, 3); 
                        if(checkNeighbor(x, y-1, z)) addFace(bottomFace, x, y, z, 2, 3);
                        if(checkNeighbor(x, y, z+1)) addFace(frontFace, x, y, z, 2, 3);
                        if(checkNeighbor(x, y, z-1)) addFace(backFace, x, y, z, 2, 3);
                        if(checkNeighbor(x-1, y, z)) addFace(leftFace, x, y, z, 2, 3);
                        if(checkNeighbor(x+1, y, z)) addFace(rightFace, x, y, z, 2, 3);
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
        
        float uOffset = atlasX * tileSize;
        
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