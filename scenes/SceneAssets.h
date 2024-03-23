#pragma once

#include "../renderer/RenderObject.h"
#include "../graphics/SceneObject.h"
#include "../graphics/MaterialCollection.h"

enum objectID : size_t {
    ID_train,
    ID_cushion,
    ID_house1,
    ID_lampshade,
    ID_gizmos,
    ID_house2,
    ID_viaduct,
    ID_duvet_hills,
    _ID_ASSETS_MAX // keep at end
};

enum materialID : size_t {
    ID_mat_train,
    ID_mat_duvet,
    ID_mat_house1,
    ID_mat_lampshade,
    ID_mat_colorgrid,
    ID_mat_house2,
    ID_mat_viaduct
};

struct SceneAssets {
    std::vector<std::unique_ptr<SceneObject>> objects;
    std::vector<std::unique_ptr<SceneMaterial>> materials;

    SceneAssets() {
        // Load materials
        materials.push_back(std::make_unique<BoneMaterial>("assets/train_small.png"));
        materials.push_back(std::make_unique<BoneMaterial>("assets/duvet.png"));
        materials.push_back(std::make_unique<BoneMaterial>("assets/house1_small.png"));
        materials.push_back(std::make_unique<BoneMaterial>("assets/lampshade_small.png"));
        materials.push_back(std::make_unique<BoneMaterial>("assets/colorgrid.png"));
        materials.push_back(std::make_unique<BoneMaterial>("assets/house2_small.png"));
        materials.push_back(std::make_unique<BoneMaterial>("assets/viaduct.png"));

        // Load objects
        objects.push_back(LoadObject("assets/train.fbx", NULL, materials[ID_mat_train].get()));
        objects.push_back(LoadObject("assets/cushion.fbx", NULL, materials[ID_mat_duvet].get())); 
        objects.push_back(LoadObject("assets/house1.fbx", NULL, materials[ID_mat_house1].get()));
        objects.push_back(LoadObject("assets/lampshade.fbx", NULL, materials[ID_mat_lampshade].get()));
        objects.push_back(LoadObject("assets/gizmos.fbx", "Cube", materials[ID_mat_colorgrid].get()));
        objects.push_back(LoadObject("assets/house2.fbx", NULL, materials[ID_mat_house2].get()));
        objects.push_back(LoadObject("assets/viaduct.fbx", NULL, materials[ID_mat_viaduct].get()));
        objects.push_back(LoadObject("assets/duvet_hills.fbx", NULL, materials[ID_mat_duvet].get()));
    };
};

SceneAssets* getSceneAssets();