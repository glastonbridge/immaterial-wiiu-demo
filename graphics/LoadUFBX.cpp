#include <whb/sdcard.h>
#include <whb/file.h>
#include <whb/log.h>

#include <string>
#include <istream>
#include <sstream>
#include <vector>

#include <ufbx.h>

// #define DEBUG 1

// hi I'm halcy and I like to reuse code from previous releases
void LoadUFBX(
        const std::string& path,
        const char* objectName, 
        std::vector<float>& vertices,
         std::vector<float>& texcoords, 
         std::vector<float>& normals,
         std::vector<int>& boneIndices, 
         std::vector<float>& boneWeights,
         std::vector<float>& animFrames // TODO maybe we could sample live here instead of storing at target rate? also, return these as mat3x4s?
    ) {

    // Load data
    ufbx_load_opts opts = { 0 }; // Optional, pass NULL for defaults
    ufbx_error error; // Optional, pass NULL if you don't care about errors
    ufbx_scene *scene = ufbx_load_file(path.c_str(), &opts, &error);
    if (!scene) {
        WHBLogPrintf("Failed to load: %s\n", error.description.data);
        return;
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
                WHBLogPrintf("-> Have filter name and this is not it, skipping\n");
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
            WHBLogPrintf("Allocated %d verts\n", totalVerts);
            #endif

            // Go through all faces
            size_t setVertIdx = 0;
            for (size_t faceIdx = 0; faceIdx < mesh->faces.count; faceIdx++) {
                // We support only triangles, please triangulate on export Or Else
                if(mesh->faces.data[faceIdx].num_indices != 3) {
                    // Complain and skip
                    WHBLogPrintf("Yikes! Non-tri face at %ld\n", faceIdx);
                    continue;
                }

                // Get the mesh data
                size_t faceFirstVertIdx = mesh->faces.data[faceIdx].index_begin;
                for (size_t vertIdx = faceFirstVertIdx; vertIdx < faceFirstVertIdx + 3; vertIdx++) {
                    #ifdef DEBUG
                    WHBLogPrintf("Vertex %ld\n", vertIdx);
                    #endif

                    // Standard geometry data
                    ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, vertIdx);
                    #ifdef DEBUG
                    WHBLogPrintf("  * pos: %f %f %f\n", pos.x, pos.y, pos.z);
                    #endif
                    ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, vertIdx);
                    #ifdef DEBUG
                    WHBLogPrintf("  * normal: %f %f %f\n", normal.x, normal.y, normal.z);
                    #endif
                    ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, vertIdx);
                    #ifdef DEBUG
                    WHBLogPrintf("  * uv: %f %f\n", uv.x, uv.y);
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
                        WHBLogPrintf("  * got skin data, bones: %d\n", numBones);
                        #endif
                        if(numBones >= 1) {
                            boneIdx.x = skin->weights.data[vertSkinData->weight_begin].cluster_index;
                            boneWgt.x = skin->weights.data[vertSkinData->weight_begin].weight;
                            #ifdef DEBUG
                            WHBLogPrintf("  * bone 0: %f %f\n", boneIdx.x, boneWgt.x);
                            #endif
                        }
                        if(numBones >= 2) {
                            boneIdx.y = skin->weights.data[vertSkinData->weight_begin + 1].cluster_index;
                            boneWgt.y = skin->weights.data[vertSkinData->weight_begin + 1].weight;
                            #ifdef DEBUG
                            WHBLogPrintf("  * bone 1: %f %f\n", boneIdx.y, boneWgt.y);
                            #endif
                        }
                    }
                    else {
                        #ifdef DEBUG
                        WHBLogPrintf("  * no skin data\n");
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

                    boneIndices.push_back(boneIdx.x); // TODO how 2 index into the bone array in GLSL?
                    boneIndices.push_back(boneIdx.y);
                    boneWeights.push_back(boneWgt.x);
                    boneWeights.push_back(boneWgt.y);
                }
            }
            
            #ifdef DEBUG
            WHBLogPrintf("Loaded %ld verts\n", setVertIdx);
            #endif

            // Is there an animation?
            if(scene->anim_stacks.count >= 1) {
                if(scene->anim_stacks.count > 1) {
                    WHBLogPrintf("Warning: more than one animation stack (%ld), only the longest will be used\n", scene->anim_stacks.count);
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
                WHBLogPrintf("Loading animation\n");
                #endif
                ufbx_anim_stack* anim_stack = scene->anim_stacks.data[stack_use];

                // We sample at 60fps
                float frameDur = 1.0 / 60.0;
                size_t frameCount = (size_t)((anim_stack->anim.time_end - anim_stack->anim.time_begin) / frameDur + 0.5);
                size_t boneCount = skin->clusters.count; // TODO return these?
                #ifdef DEBUG
                WHBLogPrintf("  * frame count: %ld\n", frameCount);
                WHBLogPrintf("  * bone count: %ld\n", boneCount);
                #endif

                // Copy every frame
                bool havePrinted = false;
                for(size_t frame = 0; frame < frameCount; frame++) {
                    // Get every bones transform for the frame
                    for(size_t boneIdx = 0; boneIdx < boneCount; boneIdx++) {
                        ufbx_skin_cluster* cluster = skin->clusters.data[boneIdx];
                        if(!havePrinted) {
                            WHBLogPrintf("  * bone %ld: %s\n", boneIdx, cluster->bone_node->name.data);
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

                        // Transpose
                        ufbx_matrix tempMat;
                        for (int i = 0; i < 4; ++i) {
                            for (int j = 0; j < 3; ++j) {
                                int idx_in = i * 3 + j;
                                int idx_out = j * 4 + i;
                                tempMat.v[idx_out] = transformMat.v[idx_in];
                            }
                        }

                        // Do some funny coordinate swaps to make the homogenous part end up at the left side of the matrix
                        // because that's what the C3D matrices want.
                        // TODO: we're on wiiu now, redo this
                        ufbx_matrix tempMat2;
                        tempMat2.v[ 0] = tempMat.v[ 3];
                        tempMat2.v[ 1] = tempMat.v[ 0];
                        tempMat2.v[ 2] = tempMat.v[ 1];
                        tempMat2.v[ 3] = tempMat.v[ 2];

                        tempMat2.v[ 4] = tempMat.v[ 7];
                        tempMat2.v[ 5] = tempMat.v[ 4];
                        tempMat2.v[ 6] = tempMat.v[ 5];
                        tempMat2.v[ 7] = tempMat.v[ 6];

                        tempMat2.v[ 8] = tempMat.v[11];
                        tempMat2.v[ 9] = tempMat.v[ 8];
                        tempMat2.v[10] = tempMat.v[ 9];
                        tempMat2.v[11] = tempMat.v[10];

                        // Now swap second and third column as well as the first and third row
                        // because it is illegal for modeling tools and engines to ever use the
                        // same coordinate system.
                        tempMat.v[ 0] = tempMat2.v[ 4];
                        tempMat.v[ 1] = tempMat2.v[ 7];
                        tempMat.v[ 2] = tempMat2.v[ 6];
                        tempMat.v[ 3] = tempMat2.v[ 5];

                        tempMat.v[ 4] = tempMat2.v[ 8];
                        tempMat.v[ 5] = tempMat2.v[11];
                        tempMat.v[ 6] = tempMat2.v[10];
                        tempMat.v[ 7] = tempMat2.v[ 9];

                        tempMat.v[ 8] = tempMat2.v[ 0];
                        tempMat.v[ 9] = tempMat2.v[ 3];
                        tempMat.v[10] = tempMat2.v[ 2];
                        tempMat.v[11] = tempMat2.v[ 1];

                        // Finally, we have done it and other than a rotation by 90 degrees around z (on device y. the one pointing up.)
                        // we now have things pointing the same direction as in the modeling tools, assuming a camera at
                        // negative y and looking at the origin.
                        for(int i = 0; i < 12; i++) {
                            animFrames.push_back(tempMat.v[i]); // really we should be doing nested stuff here
                            // objectNew.animFrames[FBX_FRAME_IDX(frame, boneIdx, i, boneCount)] = tempMat.v[i]; 
                        }

                        #ifdef DEBUG
                        WHBLogPrintf("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n\n",
                            tempMat.v[0], tempMat.v[1], tempMat.v[2], tempMat.v[3],
                            tempMat.v[4], tempMat.v[5], tempMat.v[6], tempMat.v[7],
                            tempMat.v[8], tempMat.v[9], tempMat.v[10], tempMat.v[11]
                        );
                        #endif                        
                    }
                    havePrinted = true;
                }
            }
            else {
                WHBLogPrintf("Yikes! Invalid animation stack count (= %ld) or no skin present!\n", scene->anim_stacks.count);
            }
        }
        else {
            WHBLogPrintf("Yikes! Object is meshless! This shouldn't happen!\n");
        }
    }
    WHBLogPrintf("loaded %i vertex elements, %i texcoord elements and %i normals", vertices.size(), texcoords.size(), normals.size());
}
