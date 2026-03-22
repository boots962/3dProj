#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include "Mesh.h"

struct RaycastResult {
    bool hit =false;
    int lx, ly, lz;
    int px, py, pz;
    glm::ivec3 normal; 
    ChunkMesh* chunk = nullptr; 
};

enum mouseClicks{
    BREAK_BLOCK
};

class Actions{
    public:
    int block_type = 1;

    RaycastResult getLookingAt(glm::vec3 camPos, glm::vec3 camFront, float reach, std::map<std::pair<int, int>, ChunkMesh*>& activeChunks){
        RaycastResult result;
        int prev_wx = glm::floor(camPos.x);
        int prev_wy = glm::floor(camPos.y);
        int prev_wz = glm::floor(camPos.z);
        for(float step = 0.00f; step<reach; step+=0.005f){
            glm::vec3 currentPos = camPos + (camFront*step);

            int wx = glm::floor(currentPos.x);
            int wy = glm::floor(currentPos.y);
            int wz = glm::floor(currentPos.z);

            int cx = glm::floor((float)wx/CHUNK_WIDTH); //get the chunk location
            int cz = glm::floor((float)wz/CHUNK_WIDTH); 
            

            if(activeChunks.count({cx,cz})){ //then the chunk is active
                ChunkMesh*targetChunk = activeChunks[{cx,cz}];
            

            int lx = wx % CHUNK_WIDTH; //get local coordinate in the chunk
            if(lx<0) lx+=CHUNK_WIDTH;
            int lz = wz % CHUNK_WIDTH;
            if(lz<0) lz+=CHUNK_WIDTH;
            int ly = wy;

            if(ly>=0 &&ly<CHUNK_HEIGHT){
                if(targetChunk->chunkData[lx][ly][lz] != 0 && targetChunk->chunkData[lx][ly][lz] !=2 && targetChunk->chunkData[lx][ly][lz]!=11){
                    result.hit = true;
                    result.chunk = targetChunk;
                    result.lx = lx;
                    result.ly = ly;
                    result.lz = lz;

                    result.px = prev_wx;
                    result.py = prev_wy;
                    result.pz = prev_wz;
                    return result;

                }

            }
          
            prev_wx = wx;
            prev_wy = wy;
            prev_wz = wz;
        }
           
        }
        
        return result;
    }
    void breakBlock(RaycastResult ray, std::map<std::pair<int, int>, ChunkMesh*>& activeChunks){
        
        if(!ray.hit || ray.chunk == nullptr){
            return;
        }
        
        // 1. Update and rebuild the chunk we actually clicked
        ray.chunk->chunkData[ray.lx][ray.ly][ray.lz] = 0;
        ray.chunk->buildMesh(activeChunks);
        ray.chunk->memory();

        // 2. CHECK CHUNK BORDERS AND UPDATE NEIGHBORS
        int cx = ray.chunk->chunkX;
        int cz = ray.chunk->chunkZ;

        // Helper lambda to safely rebuild a neighbor if it exists
        auto updateNeighbor = [&](int neighborX, int neighborZ) {
            if (activeChunks.count({neighborX, neighborZ})) {
                activeChunks[{neighborX, neighborZ}]->buildMesh(activeChunks);
                activeChunks[{neighborX, neighborZ}]->memory();
            }
        };

        if (ray.lx == 0) updateNeighbor(cx - 1, cz);                   // Block was on the Left edge
        if (ray.lx == CHUNK_WIDTH - 1) updateNeighbor(cx + 1, cz);     // Block was on the Right edge
        if (ray.lz == 0) updateNeighbor(cx, cz - 1);                   // Block was on the Back edge
        if (ray.lz == CHUNK_WIDTH - 1) updateNeighbor(cx, cz + 1);     // Block was on the Front edge
    }

    void placeBlock(RaycastResult ray, std::map<std::pair<int, int>, ChunkMesh*>& activeChunks ){
        if(!ray.hit || ray.chunk == nullptr){
            return;
        }

        int cx = glm::floor((float)ray.px / CHUNK_WIDTH);
        int cz = glm::floor((float)ray.pz / CHUNK_WIDTH);

        if(activeChunks.count({cx, cz})){
            ChunkMesh* targetChunk = activeChunks[{cx, cz}];

            int localX = ray.px % CHUNK_WIDTH; 
            if(localX < 0) localX += CHUNK_WIDTH;
            
            int localZ = ray.pz % CHUNK_WIDTH;
            if(localZ < 0) localZ += CHUNK_WIDTH;
            
            int localY = ray.py;

            if(localY >= 0 && localY < CHUNK_HEIGHT){
                
                // 1. Update and rebuild the chunk we placed the block in
                targetChunk->chunkData[localX][localY][localZ] = block_type;
                targetChunk->buildMesh(activeChunks); 
                targetChunk->memory();

                // 2. CHECK CHUNK BORDERS AND UPDATE NEIGHBORS
                auto updateNeighbor = [&](int neighborX, int neighborZ) {
                    if (activeChunks.count({neighborX, neighborZ})) {
                        activeChunks[{neighborX, neighborZ}]->buildMesh(activeChunks);
                        activeChunks[{neighborX, neighborZ}]->memory();
                    }
                };

                if (localX == 0) updateNeighbor(cx - 1, cz);
                if (localX == CHUNK_WIDTH - 1) updateNeighbor(cx + 1, cz);
                if (localZ == 0) updateNeighbor(cx, cz - 1);
                if (localZ == CHUNK_WIDTH - 1) updateNeighbor(cx, cz + 1);
            }
        }
    }


};