#include <whb/sdcard.h>
#include <whb/file.h>
#include <whb/log.h>

#include <string>
#include <istream>
#include <sstream>
#include <vector>

#include <ufbx.h>
#include <glm/glm.hpp>

#include "../util/ourmalloc.h"

// #define DEBUG 1
#undef DEBUG

// hi I'm halcy and I like to reuse code from previous releases
int LoadUFBX(
        const std::string& path,
        const char* objectName, 
        std::vector<float>& vertices,
        std::vector<float>& texcoords, 
        std::vector<float>& normals,
        std::vector<float>& boneIndices, 
        std::vector<float>& boneWeights,
        std::vector<std::vector<glm::mat4>>& animFrames // nested: frame, bone, matrix
    ) {

    // Load data
    ufbx_load_opts opts = { 0 }; // Optional, pass NULL for defaults
    ufbx_error error; // Optional, pass NULL if you don't care about errors
    
    // sometimes broken mysteriously, so lets grab it all to memory first
    //ufbx_scene *scene = ufbx_load_file(path.c_str(), &opts, &error);
    
    // Variant WHB
    /*char *sdRootPath = WHBGetSdCardMountPath();
    char pathWithSd[256];
    size_t size;
    sprintf(pathWithSd, "%s/%s", sdRootPath, path.c_str());
    char* data = WHBReadWholeFile(pathWithSd, &size);*/

    // Variant C Stdlib
    FILE* file = fopen(path.c_str(), "rb");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = (char*)malloc(size);
    fread(data, 1, size, file);
    fclose(file);

    ufbx_scene *scene = ufbx_load_memory(data, size, &opts, &error);

    if (!scene) {
        free(data);
        WHBLogPrintf("Failed to load: %s", error.description.data);
        return(0);
    }

    // Go through all objects in the scene
    for (size_t i = 0; i < scene->nodes.count; i++) {
        ufbx_node *node = scene->nodes.data[i];
        if (node->is_root) continue;

        // Bail if not the right name
        #ifdef DEBUG
        WHBLogPrintf("Found object %s\n", node->name.data);
        #endif
        if(objectName != NULL) {
            if(strncmp(node->name.data, objectName, 255) != 0) {
                #ifdef DEBUG
                WHBLogPrintf("-> Have filter name and this is not it, skipping");
                #endif
                continue;
            }
        }
        
        #ifdef DEBUG
        WHBLogPrintf("-> Loading\n");
        #endif

        // We can only load meshes
        if (node->mesh) {
            // Mesh and skin objects for convenience
            ufbx_mesh* mesh = node->mesh;
            ufbx_skin_deformer* skin = mesh->skin_deformers.data[0];

            // Alloc VBO data struct
            int totalVerts = mesh->faces.count * 3;
            #ifdef DEBUG
            WHBLogPrintf("Allocated %d verts", totalVerts);
            #endif

            // Go through all faces
            size_t setVertIdx = 0;
            for (size_t faceIdx = 0; faceIdx < mesh->faces.count; faceIdx++) {
                // We support only triangles, please triangulate on export Or Else
                if(mesh->faces.data[faceIdx].num_indices != 3) {
                    // Complain and skip
                    WHBLogPrintf("Yikes! Non-tri face at %ld", faceIdx);
                    continue;
                }

                // Get the mesh data
                size_t faceFirstVertIdx = mesh->faces.data[faceIdx].index_begin;
                for (size_t vertIdx = faceFirstVertIdx; vertIdx < faceFirstVertIdx + 3; vertIdx++) {
                    #ifdef DEBUG
                    WHBLogPrintf("Vertex %ld", vertIdx);
                    #endif

                    // Standard geometry data
                    ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, vertIdx);
                    #ifdef DEBUG
                    WHBLogPrintf("  * pos: %f %f %f", pos.x, pos.y, pos.z);
                    #endif
                    ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, vertIdx);
                    #ifdef DEBUG
                    WHBLogPrintf("  * normal: %f %f %f", normal.x, normal.y, normal.z);
                    #endif
                    ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, vertIdx);
                    #ifdef DEBUG
                    WHBLogPrintf("  * uv: %f %f", uv.x, uv.y);
                    #endif

                    // Them bones. Of which we support two. More are possible at increased cost.
                    ufbx_vec2 boneIdx;
                    ufbx_vec2 boneWgt;
                    
                    boneIdx.x = 0.0;
                    boneIdx.y = 0.0;
                    boneWgt.x = 0.0;
                    boneWgt.y = 0.0;

                    if (skin != NULL) {
                        ufbx_skin_vertex* vertSkinData = &skin->vertices.data[mesh->vertex_indices.data[vertIdx]];
                        int numBones = vertSkinData->num_weights;
                        #ifdef DEBUG
                        WHBLogPrintf("  * got skin data, bones: %d", numBones);
                        #endif
                        if(numBones >= 1) {
                            boneIdx.x = skin->weights.data[vertSkinData->weight_begin].cluster_index;
                            boneWgt.x = skin->weights.data[vertSkinData->weight_begin].weight;
                            #ifdef DEBUG
                            WHBLogPrintf("  * bone 0: %f %f", boneIdx.x, boneWgt.x);
                            #endif
                        }
                        if(numBones >= 2) {
                            boneIdx.y = skin->weights.data[vertSkinData->weight_begin + 1].cluster_index;
                            boneWgt.y = skin->weights.data[vertSkinData->weight_begin + 1].weight;
                            #ifdef DEBUG
                            WHBLogPrintf("  * bone 1: %f %f", boneIdx.y, boneWgt.y);
                            #endif
                        }
                    }
                    else {
                        #ifdef DEBUG
                        WHBLogPrintf("  * no skin data");
                        #endif
                    }

                    vertices.push_back(pos.x);
                    vertices.push_back(pos.y);
                    vertices.push_back(pos.z);
                    texcoords.push_back(uv.x);
                    texcoords.push_back(1.0 - uv.y);
                    normals.push_back(normal.x);
                    normals.push_back(normal.y);
                    normals.push_back(normal.z);

                    boneIndices.push_back(boneIdx.x); // up to 4 bones per vert possible using this method. 
                    boneIndices.push_back(boneIdx.y);
                    boneWeights.push_back(boneWgt.x);
                    boneWeights.push_back(boneWgt.y);
                }
            }
            
            #ifdef DEBUG
            WHBLogPrintf("Loaded %ld verts", setVertIdx);
            #endif

            // Is there an animation?
            if(scene->anim_stacks.count >= 1 && skin != NULL) {
                if(scene->anim_stacks.count > 1) {
                    WHBLogPrintf("Warning: more than one animation stack (%ld), only the longest will be used", scene->anim_stacks.count);
                }

                int stack_use = 0;
                int stack_frames = 0;
                for(int i = scene->anim_stacks.count - 1; i >= 0; i--) {
                    ufbx_anim_stack* anim_stack = scene->anim_stacks.data[i];
                    int frames = (int)((anim_stack->anim.time_end - anim_stack->anim.time_begin) / (1.0 / 60.0) + 0.5);
                    if(frames > stack_frames) {
                        stack_use = i;
                        stack_frames = frames;
                    }
                }

                #ifdef DEBUG
                WHBLogPrintf("Loading animation, stack stack to use is %d", stack_use);
                #endif
                ufbx_anim_stack* anim_stack = scene->anim_stacks.data[stack_use];

                // We sample at 60fps
                #ifdef DEBUG
                WHBLogPrintf("  * anim time begin: %f", anim_stack->anim.time_begin);
                WHBLogPrintf("  * anim time end: %f", anim_stack->anim.time_end);
                #endif
                float frameDur = 1.0 / 60.0;
                size_t frameCount = (size_t)((anim_stack->anim.time_end - anim_stack->anim.time_begin) / frameDur + 0.5);
                size_t boneCount = skin->clusters.count; // TODO return these?
                #ifdef DEBUG
                WHBLogPrintf("  * frame count: %ld", frameCount);
                WHBLogPrintf("  * bone count: %ld", boneCount);
                #endif

                // Copy every frame
                bool havePrinted = false;
                for(size_t frame = 0; frame < frameCount; frame++) {
                    std::vector<glm::mat4> frameMats;

                    // Get every bones transform for the frame
                    for(size_t boneIdx = 0; boneIdx < boneCount; boneIdx++) {
                        ufbx_skin_cluster* cluster = skin->clusters.data[boneIdx];
                        if(!havePrinted) {
                            #ifdef DEBUG
                            WHBLogPrintf("  * bone %ld: %s", boneIdx, cluster->bone_node->name.data);
                            #endif
                        }
                        ufbx_node* bone = cluster->bone_node;
                        float frameTime = anim_stack->anim.time_begin + frame * frameDur;
                        ufbx_transform transform = ufbx_evaluate_transform(&anim_stack->anim, bone, frameTime);
                        ufbx_matrix transformMatLocal = ufbx_transform_to_matrix(&transform);
                        ufbx_matrix transformMat = ufbx_matrix_mul(&transformMatLocal, &cluster->geometry_to_bone);

                        #ifdef DEBUG
                        WHBLogPrintf("%f %f %f\n%f %f %f\n%f %f %f\n%f %f %f\n\n",
                            transformMat.v[0],
                            transformMat.v[1],
                            transformMat.v[2],
                            transformMat.v[3],
                            transformMat.v[4],
                            transformMat.v[5],
                            transformMat.v[6],
                            transformMat.v[7],
                            transformMat.v[8],
                            transformMat.v[9],
                            transformMat.v[10],
                            transformMat.v[11]
                        );
                        #endif

                        frameMats.push_back(glm::mat4(
                            transformMat.v[ 0], transformMat.v[ 1], transformMat.v[ 2], 0.0,
                            transformMat.v[ 3], transformMat.v[ 4], transformMat.v[ 5], 0.0,
                            transformMat.v[ 6], transformMat.v[ 7], transformMat.v[ 8], 0.0,
                            transformMat.v[ 9], transformMat.v[10], transformMat.v[11], 1.0
                        ));                     
                    }
                    animFrames.push_back(frameMats);
                    havePrinted = true;
                }
            }
            else {
                WHBLogPrintf("Yikes! Invalid animation stack count (= %ld) or no skin present!", scene->anim_stacks.count);
            }
        }
        else {
            WHBLogPrintf("Yikes! Object is meshless! This shouldn't happen!");
        }
    }
    WHBLogPrintf("loaded %i vertex elements, %i texcoord elements and %i normals, %d animation frames", vertices.size(), texcoords.size(), normals.size(), animFrames.size());
    free(data);
    return(1);
}

int LoadTypeUFBX(
    const std::string &path,
    std::vector<std::vector<float>> &v_vertices,
    std::vector<std::vector<float>> &v_normals)
{

    // Load data
    ufbx_load_opts opts = {0}; // Optional, pass NULL for defaults
    ufbx_error error;          // Optional, pass NULL if you don't care about errors

    // sometimes broken mysteriously, so lets grab it all to memory first
    // ufbx_scene *scene = ufbx_load_file(path.c_str(), &opts, &error);

    // Variant WHB
    /*char *sdRootPath = WHBGetSdCardMountPath();
    char pathWithSd[256];
    size_t size;
    sprintf(pathWithSd, "%s/%s", sdRootPath, path.c_str());
    char* data = WHBReadWholeFile(pathWithSd, &size);*/

    // Variant C Stdlib
    FILE *file = fopen(path.c_str(), "rb");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *data = (char *)malloc(size);
    fread(data, 1, size, file);
    fclose(file);

    ufbx_scene *scene = ufbx_load_memory(data, size, &opts, &error);

    if (!scene)
    {
        free(data);
        WHBLogPrintf("Failed to load: %s", error.description.data);
        return (0);
    }

    // Go through all objects in the scene
    for (size_t i = 0; i < scene->nodes.count; i++)
    {
        ufbx_node *node = scene->nodes.data[i];
        if (node->is_root)
            continue;
        
        std::vector<float> vertices;
        std::vector<float> normals;
        float min_x = FLT_MAX, min_y = FLT_MAX, min_z = FLT_MAX;
        float max_x = -FLT_MAX, max_y = -FLT_MAX, max_z = -FLT_MAX;

        #ifdef DEBUG
        WHBLogPrintf("Found object %s\n", node->name.data);
        WHBLogPrintf("-> Loading\n");
        #endif

        // We can only load meshes
        if (node->mesh) {
            // Mesh and skin objects for convenience
            ufbx_mesh *mesh = node->mesh;
            ufbx_skin_deformer *skin = mesh->skin_deformers.data[0];

            // Alloc VBO data struct
            int totalVerts = mesh->faces.count * 3;
            #ifdef DEBUG
            WHBLogPrintf("Allocated %d verts", totalVerts);
            #endif

            // Go through all faces
            size_t setVertIdx = 0;
            for (size_t faceIdx = 0; faceIdx < mesh->faces.count; faceIdx++)
            {
                // We support only triangles, please triangulate on export Or Else
                if (mesh->faces.data[faceIdx].num_indices != 3)
                {
                    // Complain and skip
                    WHBLogPrintf("Yikes! Non-tri face at %ld", faceIdx);
                    continue;
                }

                // Get the mesh data
                size_t faceFirstVertIdx = mesh->faces.data[faceIdx].index_begin;
                for (size_t vertIdx = faceFirstVertIdx; vertIdx < faceFirstVertIdx + 3; vertIdx++)
                {
                    #ifdef DEBUG
                    WHBLogPrintf("Vertex %ld", vertIdx);
                    #endif

                    // Standard geometry data
                    ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, vertIdx);
                    #ifdef DEBUG
                    WHBLogPrintf("  * pos: %f %f %f", pos.x, pos.y, pos.z);
                    #endif
                    ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, vertIdx);
                    #ifdef DEBUG
                    WHBLogPrintf("  * normal: %f %f %f", normal.x, normal.y, normal.z);
                    #endif
                    ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, vertIdx);
                    #ifdef DEBUG
                    WHBLogPrintf("  * uv: %f %f", uv.x, uv.y);
                    #endif

                    min_x = std::min((float)pos.x, min_x);
                    max_x = std::max((float)pos.x, max_x);

                    min_y = std::min((float)pos.y, min_y);
                    max_y = std::max((float)pos.y, max_y);

                    min_z = std::min((float)pos.z, min_z);
                    max_z = std::max((float)pos.z, max_z);

                    vertices.push_back(pos.x);
                    vertices.push_back(pos.y);
                    vertices.push_back(pos.z);
                    normals.push_back(normal.x);
                    normals.push_back(normal.y);
                    normals.push_back(normal.z);
                }
            }

            #ifdef DEBUG
            WHBLogPrintf("Loaded %ld verts", setVertIdx);
            #endif

            float center_x = min_x;
            float center_y = (min_y + max_y) / 2.0;
            float center_z = (min_z + max_z) / 2.0;
            for (int i=0; i<vertices.size(); i+=3) {
                vertices[i] -= center_x;
                vertices[i+1] -= center_y;
                vertices[i+2] -= center_z;
            }

            v_vertices.push_back(vertices);
            v_normals.push_back(normals);
        }
        else {
            WHBLogPrintf("Yikes! Object is meshless! This shouldn't happen!");
        }
    }
    free(data);
    return (1);
}
