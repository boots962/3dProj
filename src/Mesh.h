#pragma once
#include <vector>
#include <cstdint>
#include <glad/glad.h>

#include "Perlin.h"
const int CHUNK_SIZE = 64;



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
                float noiseVal = (Perlin::noise(globalX * scale, globalZ * scale) + 1.0f) / 2.0f;
                float steepNoise = std::pow(noiseVal, 1.5f);
                int terrainHeight = static_cast<int>(steepNoise * 40.0f);


                if (terrainHeight < 1) terrainHeight = 1;
                if (terrainHeight >= CHUNK_SIZE) terrainHeight = CHUNK_SIZE - 1;

                for(int y = 0; y < CHUNK_SIZE; y++){
                    if(y == terrainHeight) {
                        chunkData[x][y][z] = 1; 
                    }
                    else if(y<terrainHeight && y>terrainHeight-2){
                        chunkData[x][y][z] = 4; 
                    }
                    else if (y<terrainHeight-2){
                        chunkData[x][y][z] = 3;
                    }
                    else if (y <= WATER_LEVEL) {
                        chunkData[x][y][z] = 2; 
                    } else {
                        chunkData[x][y][z] = 0; // Air
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

        float topFace[] = {
            -0.5f, 0.5f,  0.5f,   0.3f, 0.8f, 0.4f,  // Green
             0.5f, 0.5f,  0.5f,   0.3f, 0.8f, 0.4f,  
             0.5f, 0.5f, -0.5f,   0.3f, 0.8f, 0.4f,  
             0.5f, 0.5f, -0.5f,   0.3f, 0.8f, 0.4f,  
            -0.5f, 0.5f, -0.5f,   0.3f, 0.8f, 0.4f,  
            -0.5f, 0.5f,  0.5f,   0.3f, 0.8f, 0.4f   
        };

        float bottomFace[] = {
            -0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  // Brown
             0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  
             0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  
             0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  
            -0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  
            -0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f   
        };

        float frontFace[] = {
            -0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  // Bottom Left (Brown)
             0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  // Bottom Right (Brown)
             0.5f,  0.5f,  0.5f,  0.3f, 0.8f, 0.4f,  // Top Right (Green)
             0.5f,  0.5f,  0.5f,  0.3f, 0.8f, 0.4f,  // Top Right (Green)
            -0.5f,  0.5f,  0.5f,  0.3f, 0.8f, 0.4f,  // Top Left (Green)
            -0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f   // Bottom Left (Brown)
        };
        float backFace[] = {
             0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  // Bottom Left (Brown)
            -0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  // Bottom Right (Brown)
            -0.5f,  0.5f, -0.5f,  0.3f, 0.8f, 0.4f,  // Top Right (Green)
            -0.5f,  0.5f, -0.5f,  0.3f, 0.8f, 0.4f,  // Top Right (Green)
             0.5f,  0.5f, -0.5f,  0.3f, 0.8f, 0.4f,  // Top Left (Green)
             0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f   // Bottom Left (Brown)
        };
        
        float leftFace[] = {
            -0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  // Bottom Left (Brown)
            -0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  // Bottom Right (Brown)
            -0.5f,  0.5f,  0.5f,  0.3f, 0.8f, 0.4f,  // Top Right (Green)
            -0.5f,  0.5f,  0.5f,  0.3f, 0.8f, 0.4f,  // Top Right (Green)
            -0.5f,  0.5f, -0.5f,  0.3f, 0.8f, 0.4f,  // Top Left (Green)
            -0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f   // Bottom Left (Brown)
        };
        
        float rightFace[] = {
             0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  // Bottom Left (Brown)
             0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  // Bottom Right (Brown)
             0.5f,  0.5f, -0.5f,  0.3f, 0.8f, 0.4f,  // Top Right (Green)
             0.5f,  0.5f, -0.5f,  0.3f, 0.8f, 0.4f,  // Top Right (Green)
             0.5f,  0.5f,  0.5f,  0.3f, 0.8f, 0.4f,  // Top Left (Green)
             0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f   // Bottom Left (Brown)
        };
        float wTopFace[] = {
            -0.5f, 0.45f,  0.5f,   0.2f, 0.4f, 0.9f,  0.5f, 0.45f,  0.5f,   0.2f, 0.4f, 0.9f,  
             0.5f, 0.45f, -0.5f,   0.2f, 0.4f, 0.9f,  0.5f, 0.45f, -0.5f,   0.2f, 0.4f, 0.9f,  
            -0.5f, 0.45f, -0.5f,   0.2f, 0.4f, 0.9f, -0.5f, 0.45f,  0.5f,   0.2f, 0.4f, 0.9f   
        };
        float wBottomFace[] = {
            -0.5f, -0.55f, -0.5f,  0.1f, 0.2f, 0.6f,  0.5f, -0.55f, -0.5f,  0.1f, 0.2f, 0.6f,  
             0.5f, -0.55f,  0.5f,  0.1f, 0.2f, 0.6f,  0.5f, -0.55f,  0.5f,  0.1f, 0.2f, 0.6f,  
            -0.5f, -0.55f,  0.5f,  0.1f, 0.2f, 0.6f, -0.5f, -0.55f, -0.5f,  0.1f, 0.2f, 0.6f   
        };
        float wFrontFace[] = {
            -0.5f, -0.55f,  0.5f,  0.1f, 0.2f, 0.6f,  0.5f, -0.55f,  0.5f,  0.1f, 0.2f, 0.6f,  
             0.5f,  0.45f,  0.5f,  0.2f, 0.4f, 0.9f,  0.5f,  0.45f,  0.5f,  0.2f, 0.4f, 0.9f,  
            -0.5f,  0.45f,  0.5f,  0.2f, 0.4f, 0.9f, -0.5f, -0.55f,  0.5f,  0.1f, 0.2f, 0.6f   
        };
        float wBackFace[] = {
             0.5f, -0.55f, -0.5f,  0.1f, 0.2f, 0.6f, -0.5f, -0.55f, -0.5f,  0.1f, 0.2f, 0.6f,  
            -0.5f,  0.45f, -0.5f,  0.2f, 0.4f, 0.9f, -0.5f,  0.45f, -0.5f,  0.2f, 0.4f, 0.9f,  
             0.5f,  0.45f, -0.5f,  0.2f, 0.4f, 0.9f,  0.5f, -0.55f, -0.5f,  0.1f, 0.2f, 0.6f   
        };
        float wLeftFace[] = {
            -0.5f, -0.55f, -0.5f,  0.1f, 0.2f, 0.6f, -0.5f, -0.55f,  0.5f,  0.1f, 0.2f, 0.6f,  
            -0.5f,  0.45f,  0.5f,  0.2f, 0.4f, 0.9f, -0.5f,  0.45f,  0.5f,  0.2f, 0.4f, 0.9f,  
            -0.5f,  0.45f, -0.5f,  0.2f, 0.4f, 0.9f, -0.5f, -0.55f, -0.5f,  0.1f, 0.2f, 0.6f   
        };
        float wRightFace[] = {
             0.5f, -0.55f,  0.5f,  0.1f, 0.2f, 0.6f,  0.5f, -0.55f, -0.5f,  0.1f, 0.2f, 0.6f,  
             0.5f,  0.45f, -0.5f,  0.2f, 0.4f, 0.9f,  0.5f,  0.45f, -0.5f,  0.2f, 0.4f, 0.9f,  
             0.5f,  0.45f,  0.5f,  0.2f, 0.4f, 0.9f,  0.5f, -0.55f,  0.5f,  0.1f, 0.2f, 0.6f   
        }; 

        float StopFace[] = {
            -0.5f, 0.5f,  0.5f,   0.5f, 0.5f, 0.5f,  // Gray
             0.5f, 0.5f,  0.5f,   0.5f, 0.5f, 0.5f,  
             0.5f, 0.5f, -0.5f,   0.5f, 0.5f, 0.5f,  
             0.5f, 0.5f, -0.5f,   0.5f, 0.5f, 0.5f,  
            -0.5f, 0.5f, -0.5f,   0.5f, 0.5f, 0.5f,  
            -0.5f, 0.5f,  0.5f,   0.5f, 0.5f, 0.5f   
        };

        float SbottomFace[] = {
            -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  // Gray
             0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  
             0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f,  
             0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f,  
            -0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f,  
            -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f   
        };

        float SfrontFace[] = {
            -0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f,  // Gray
             0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f,  
             0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f,  
             0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f,  
            -0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f,  
            -0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f   
        };

        float SbackFace[] = {
             0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  // Gray
            -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  
            -0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  
            -0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  
             0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  
             0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f   
        };
        
        float SleftFace[] = {
            -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  // Gray
            -0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f,  
            -0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f,  
            -0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f,  
            -0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  
            -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f   
        };
        
        float SrightFace[] = {
             0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f,  // Gray
             0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  
             0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  
             0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  
             0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f,  
             0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f   
        };

        float Dirt_topFace[] = {
            -0.5f, 0.5f,  0.5f,   0.4f, 0.2f, 0.1f,  // Brown
             0.5f, 0.5f,  0.5f,   0.4f, 0.2f, 0.1f,  
             0.5f, 0.5f, -0.5f,   0.4f, 0.2f, 0.1f,  
             0.5f, 0.5f, -0.5f,   0.4f, 0.2f, 0.1f,  
            -0.5f, 0.5f, -0.5f,   0.4f, 0.2f, 0.1f,  
            -0.5f, 0.5f,  0.5f,   0.4f, 0.2f, 0.1f   
        };

        float Dirt_bottomFace[] = {
            -0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  // Brown
             0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  
             0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  
             0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  
            -0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  
            -0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f   
        };

        float Dirt_frontFace[] = {
            -0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  // Brown
             0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  
             0.5f,  0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  
             0.5f,  0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  
            -0.5f,  0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  
            -0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f   
        };

        float Dirt_backFace[] = {
             0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  // Brown
            -0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  
            -0.5f,  0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  
            -0.5f,  0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  
             0.5f,  0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  
             0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f   
        };
        
        float Dirt_leftFace[] = {
            -0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  // Brown
            -0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  
            -0.5f,  0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  
            -0.5f,  0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  
            -0.5f,  0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  
            -0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f   
        };
        
        float Dirt_rightFace[] = {
             0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  // Brown
             0.5f, -0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  
             0.5f,  0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  
             0.5f,  0.5f, -0.5f,  0.4f, 0.2f, 0.1f,  
             0.5f,  0.5f,  0.5f,  0.4f, 0.2f, 0.1f,  
             0.5f, -0.5f,  0.5f,  0.4f, 0.2f, 0.1f   
        };

        float wdTopFace[] = {
            -0.5f, 0.5f,  0.5f,   0.35f, 0.2f, 0.1f,  0.5f, 0.5f,  0.5f,   0.35f, 0.2f, 0.1f,  
             0.5f, 0.5f, -0.5f,   0.35f, 0.2f, 0.1f,  0.5f, 0.5f, -0.5f,   0.35f, 0.2f, 0.1f,  
            -0.5f, 0.5f, -0.5f,   0.35f, 0.2f, 0.1f, -0.5f, 0.5f,  0.5f,   0.35f, 0.2f, 0.1f   
        };
        float wdBottomFace[] = {
            -0.5f, -0.5f, -0.5f,  0.35f, 0.2f, 0.1f,  0.5f, -0.5f, -0.5f,  0.35f, 0.2f, 0.1f,  
             0.5f, -0.5f,  0.5f,  0.35f, 0.2f, 0.1f,  0.5f, -0.5f,  0.5f,  0.35f, 0.2f, 0.1f,  
            -0.5f, -0.5f,  0.5f,  0.35f, 0.2f, 0.1f, -0.5f, -0.5f, -0.5f,  0.35f, 0.2f, 0.1f   
        };
        float wdFrontFace[] = {
            -0.5f, -0.5f,  0.5f,  0.35f, 0.2f, 0.1f,  0.5f, -0.5f,  0.5f,  0.35f, 0.2f, 0.1f,  
             0.5f,  0.5f,  0.5f,  0.35f, 0.2f, 0.1f,  0.5f,  0.5f,  0.5f,  0.35f, 0.2f, 0.1f,  
            -0.5f,  0.5f,  0.5f,  0.35f, 0.2f, 0.1f, -0.5f, -0.5f,  0.5f,  0.35f, 0.2f, 0.1f   
        };
        float wdBackFace[] = {
             0.5f, -0.5f, -0.5f,  0.35f, 0.2f, 0.1f, -0.5f, -0.5f, -0.5f,  0.35f, 0.2f, 0.1f,  
            -0.5f,  0.5f, -0.5f,  0.35f, 0.2f, 0.1f, -0.5f,  0.5f, -0.5f,  0.35f, 0.2f, 0.1f,  
             0.5f,  0.5f, -0.5f,  0.35f, 0.2f, 0.1f,  0.5f, -0.5f, -0.5f,  0.35f, 0.2f, 0.1f   
        };
        float wdLeftFace[] = {
            -0.5f, -0.5f, -0.5f,  0.35f, 0.2f, 0.1f, -0.5f, -0.5f,  0.5f,  0.35f, 0.2f, 0.1f,  
            -0.5f,  0.5f,  0.5f,  0.35f, 0.2f, 0.1f, -0.5f,  0.5f,  0.5f,  0.35f, 0.2f, 0.1f,  
            -0.5f,  0.5f, -0.5f,  0.35f, 0.2f, 0.1f, -0.5f, -0.5f, -0.5f,  0.35f, 0.2f, 0.1f   
        };
        float wdRightFace[] = {
             0.5f, -0.5f,  0.5f,  0.35f, 0.2f, 0.1f,  0.5f, -0.5f, -0.5f,  0.35f, 0.2f, 0.1f,  
             0.5f,  0.5f, -0.5f,  0.35f, 0.2f, 0.1f,  0.5f,  0.5f, -0.5f,  0.35f, 0.2f, 0.1f,  
             0.5f,  0.5f,  0.5f,  0.35f, 0.2f, 0.1f,  0.5f, -0.5f,  0.5f,  0.35f, 0.2f, 0.1f   
        };

      float lfTopFace[] = {
            -0.5f, 0.5f,  0.5f,   0.1f, 0.35f, 0.1f,  0.5f, 0.5f,  0.5f,   0.1f, 0.35f, 0.1f,  
             0.5f, 0.5f, -0.5f,   0.1f, 0.35f, 0.1f,  0.5f, 0.5f, -0.5f,   0.1f, 0.35f, 0.1f,  
            -0.5f, 0.5f, -0.5f,   0.1f, 0.35f, 0.1f, -0.5f, 0.5f,  0.5f,   0.1f, 0.35f, 0.1f   
        };
        float lfBottomFace[] = {
            -0.5f, -0.5f, -0.5f,  0.1f, 0.35f, 0.1f,  0.5f, -0.5f, -0.5f,  0.1f, 0.35f, 0.1f,  
             0.5f, -0.5f,  0.5f,  0.1f, 0.35f, 0.1f,  0.5f, -0.5f,  0.5f,  0.1f, 0.35f, 0.1f,  
            -0.5f, -0.5f,  0.5f,  0.1f, 0.35f, 0.1f, -0.5f, -0.5f, -0.5f,  0.1f, 0.35f, 0.1f   
        };
        float lfFrontFace[] = {
            -0.5f, -0.5f,  0.5f,  0.1f, 0.35f, 0.1f,  0.5f, -0.5f,  0.5f,  0.1f, 0.35f, 0.1f,  
             0.5f,  0.5f,  0.5f,  0.1f, 0.35f, 0.1f,  0.5f,  0.5f,  0.5f,  0.1f, 0.35f, 0.1f,  
            -0.5f,  0.5f,  0.5f,  0.1f, 0.35f, 0.1f, -0.5f, -0.5f,  0.5f,  0.1f, 0.35f, 0.1f   
        };
        float lfBackFace[] = {
             0.5f, -0.5f, -0.5f,  0.1f, 0.35f, 0.1f, -0.5f, -0.5f, -0.5f,  0.1f, 0.35f, 0.1f,  
            -0.5f,  0.5f, -0.5f,  0.1f, 0.35f, 0.1f, -0.5f,  0.5f, -0.5f,  0.1f, 0.35f, 0.1f,  
             0.5f,  0.5f, -0.5f,  0.1f, 0.35f, 0.1f,  0.5f, -0.5f, -0.5f,  0.1f, 0.35f, 0.1f   
        };
        float lfLeftFace[] = {
            -0.5f, -0.5f, -0.5f,  0.1f, 0.35f, 0.1f, -0.5f, -0.5f,  0.5f,  0.1f, 0.35f, 0.1f,  
            -0.5f,  0.5f,  0.5f,  0.1f, 0.35f, 0.1f, -0.5f,  0.5f,  0.5f,  0.1f, 0.35f, 0.1f,  
            -0.5f,  0.5f, -0.5f,  0.1f, 0.35f, 0.1f, -0.5f, -0.5f, -0.5f,  0.1f, 0.35f, 0.1f   
        };
        float lfRightFace[] = {
             0.5f, -0.5f,  0.5f,  0.1f, 0.35f, 0.1f,  0.5f, -0.5f, -0.5f,  0.1f, 0.35f, 0.1f,  
             0.5f,  0.5f, -0.5f,  0.1f, 0.35f, 0.1f,  0.5f,  0.5f, -0.5f,  0.1f, 0.35f, 0.1f,  
             0.5f,  0.5f,  0.5f,  0.1f, 0.35f, 0.1f,  0.5f, -0.5f,  0.5f,  0.1f, 0.35f, 0.1f   
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
                        
                        if (currentBlock == 1 || currentBlock == 3 ||currentBlock ==4 ||currentBlock==5 ||currentBlock==6)
                             return neighbor == 0 || neighbor ==2 ; // Grass renders against Air(0) and Water(2)
                        if (currentBlock == 2) return neighbor == 0; // Water renders ONLY against Air(0)
                        return false;
                    };

                    struct BlockDefinition {
                        float* top; float* bottom; float* right;
                        float* left; float* front; float* back;
                    };

                    BlockDefinition blockRegistry[] = {
                        {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},                // ID 0: Air (Keeps indexes aligned)
                        {topFace, bottomFace, rightFace, leftFace, frontFace, backFace},       // ID 1: Grass
                        {wTopFace, wBottomFace, wRightFace, wLeftFace, wFrontFace, wBackFace}, // ID 2: Water
                        {StopFace, SbottomFace, SrightFace, SleftFace, SfrontFace, SbackFace},
                        {Dirt_topFace, Dirt_bottomFace, Dirt_rightFace, Dirt_leftFace, Dirt_frontFace, Dirt_backFace},
                        {wdTopFace, wdBottomFace, wdRightFace, wdLeftFace, wdFrontFace, wdBackFace}, // ID 4: Wood
                         {lfTopFace, lfBottomFace, lfRightFace, lfLeftFace, lfFrontFace, lfBackFace}  // ID 3: Stone
                    };

                    BlockDefinition currentFaces = blockRegistry[currentBlock];
                    float alpha = 1.00f;
                    if(currentBlock == 2) alpha = 0.50f;
                    else if(currentBlock == 6) alpha = 0.70f;

                    if (checkNeighbor(x, y + 1, z)) addFace(currentFaces.top, x, y, z, alpha);
                    if (checkNeighbor(x, y - 1, z)) addFace(currentFaces.bottom, x, y, z, alpha);
                    if (checkNeighbor(x + 1, y, z)) addFace(currentFaces.right, x, y, z, alpha);
                    if (checkNeighbor(x - 1, y, z)) addFace(currentFaces.left, x, y, z, alpha);
                    if (checkNeighbor(x, y, z + 1)) addFace(currentFaces.front, x, y, z, alpha);
                    if (checkNeighbor(x, y, z - 1)) addFace(currentFaces.back, x, y, z, alpha);
                    
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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
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
    void addFace(float* faceVertices, int x, int y, int z, float alpha) {
        for (int i = 0; i < 36; i += 6) {
            meshVertices.push_back(faceVertices[i]     + x); // X
            meshVertices.push_back(faceVertices[i + 1] + y); // Y
            meshVertices.push_back(faceVertices[i + 2] + z); // Z
            meshVertices.push_back(faceVertices[i + 3]);     // R
            meshVertices.push_back(faceVertices[i + 4]);     // G
            meshVertices.push_back(faceVertices[i + 5]);     // B
            meshVertices.push_back(alpha);     // B
        }
    }
};