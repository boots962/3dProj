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
    void breakBlock(RaycastResult ray,  std::map<std::pair<int, int>, ChunkMesh*>& activeChunks){
        
        if(!ray.hit||ray.chunk == nullptr){
            return;
        }
        ray.chunk->chunkData[ray.lx][ray.ly][ray.lz] = 0;
        ray.chunk->buildMesh();
        ray.chunk->memory();;
    }

    void placeBlock(RaycastResult ray,std::map<std::pair<int, int>, ChunkMesh*>& activeChunks ){
        if(!ray.hit||ray.chunk == nullptr){
            return;
        }
        ray.chunk->chunkData[ray.px][ray.py][ray.pz] = block_type;;
        ray.chunk->buildMesh();
        ray.chunk->memory();;
    }


};