#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include "Mesh.h"

struct RaycastResult {
    bool hit =false;
    int lx, ly, lz;
    glm::ivec3 normal; 
    ChunkMesh* chunk = nullptr; 
};

enum mouseClicks{
    BREAK_BLOCK
};

class Actions{
    public:

    RaycastResult getLookingAt(glm::vec3 camPos, glm::vec3 camFront, float reach, std::map<std::pair<int, int>, ChunkMesh*>& activeChunks){
        RaycastResult result;
        for(float step = 0.00f; step<reach; step+=0.05f){
            glm::vec3 currentPos = camPos + (camFront*step);

            int wx = glm::floor(currentPos.x);
            int wy = glm::floor(currentPos.y);
            int wz = glm::floor(currentPos.z);

            int cx = glm::floor((float)wx/CHUNK_SIZE); //get the chunk location
            int cz = glm::floor((float)wz/CHUNK_SIZE); 
            

            if(activeChunks.count({cx,cz})){ //then the chunk is active
                ChunkMesh*targetChunk = activeChunks[{cx,cz}];
            

            int lx = wx % CHUNK_SIZE; //get local coordinate in the chunk
            if(lx<0) lx+=CHUNK_SIZE;
            int lz = wz % CHUNK_SIZE;
            if(lz<0) lz+=CHUNK_SIZE;
            int ly = wy;

            if(ly>=0 &&ly<CHUNK_SIZE){
                if(targetChunk->chunkData[lx][ly][lz] != 0 && targetChunk->chunkData[lx][ly][lz] !=2){
                    result.hit = true;
                    result.chunk = targetChunk;
                    result.lx = lx;
                    result.ly = ly;
                    result.lz = lz;
                    return result;
                }

            }
        }
        }
        
        return result;
    }
    void breakBlock(RaycastResult ray,  std::map<std::pair<int, int>, ChunkMesh*>& activeChunks){
        
        if(!ray.hit||ray.chunk == nullptr){
            return;
        }
        ray.chunk->chunkData[ray.lx][ray.ly][ray.lz] = 0;
        ray.chunk->buildMesh();
        ray.chunk->memory();;
    }

};