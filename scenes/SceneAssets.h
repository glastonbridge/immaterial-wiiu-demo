#pragma once

#include "../renderer/RenderObject.h"
#include "../graphics/SceneObject.h"
#include "../graphics/MaterialCollection.h"

enum materialID : size_t {
    ID_mat_train,
    ID_mat_duvet,
    ID_mat_house1,
    ID_mat_lampshade,
    ID_mat_colorgrid,
    ID_mat_house2,
    ID_mat_viaduct,
    ID_mat_skybox,
    ID_mat_cloud,
    ID_mat_egg,
    ID_mat_building,
    ID_mat_station_building,
};

enum objectID : size_t {
    ID_train,
    ID_cushion,
    ID_house1,
    ID_lampshade,
    ID_gizmos,
    ID_house2,
    ID_viaduct,
    ID_duvet_hills,
    ID_skybox,
    ID_cardboard_cloud,
    ID_pillow,
    ID_egg_egg,
    ID_egg_hands,
    ID_egg_sausage,
    ID_egg_sausageleash,
    ID_building,
    ID_station_building,
    ID_cloud,
    ID_sofa,
    _ID_ASSETS_MAX // keep at end
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
        materials.push_back(std::make_unique<SkyboxMaterial>("assets/sky_"));
        materials.push_back(std::make_unique<BoneMaterial>("assets/cloud_basecolour.png"));
        materials.push_back(std::make_unique<BoneMaterial>("assets/egg.png"));
        materials.push_back(std::make_unique<BoneMaterial>("assets/building.png"));
        materials.push_back(std::make_unique<BoneMaterial>("assets/station_building.png"));
        
        // Load objects
        objects.push_back(LoadObject("assets/train.fbx", NULL, materials[ID_mat_train].get()));
        objects.push_back(LoadObject("assets/cushion.fbx", NULL, materials[ID_mat_duvet].get()));  // placeholder mat
        objects.push_back(LoadObject("assets/house1.fbx", NULL, materials[ID_mat_house1].get()));
        objects.push_back(LoadObject("assets/lampshade.fbx", NULL, materials[ID_mat_lampshade].get()));
        objects.push_back(LoadObject("assets/gizmos.fbx", "Cube", materials[ID_mat_colorgrid].get()));
        objects.push_back(LoadObject("assets/house2.fbx", NULL, materials[ID_mat_house2].get()));
        objects.push_back(LoadObject("assets/viaduct.fbx", NULL, materials[ID_mat_viaduct].get()));
        objects.push_back(LoadObject("assets/duvet_hills.fbx", NULL, materials[ID_mat_duvet].get())); 
        objects.push_back(LoadObject("assets/box.fbx", "Cube", materials[ID_mat_skybox].get()));
        objects.push_back(LoadObject("assets/cardboard_cloud.fbx", NULL, materials[ID_mat_duvet].get())); // placeholder mat
        objects.push_back(LoadObject("assets/pillow.fbx", NULL, materials[ID_mat_duvet].get())); // placeholder mat
        objects.push_back(LoadObject("assets/egg.fbx", "egg", materials[ID_mat_egg].get())); 
        objects.push_back(LoadObject("assets/egg.fbx", "egg_hands", materials[ID_mat_egg].get()));
        objects.push_back(LoadObject("assets/egg.fbx", "sausage", materials[ID_mat_egg].get())); 
        objects.push_back(LoadObject("assets/egg.fbx", "sausage_leash", materials[ID_mat_egg].get()));
        objects.push_back(LoadObject("assets/building.fbx", NULL, materials[ID_mat_building].get()));
        objects.push_back(LoadObject("assets/station_building.fbx", NULL, materials[ID_mat_station_building].get())); // fbx has wrong UVs currently
        objects.push_back(LoadObject("assets/cloud.fbx", NULL, materials[ID_mat_cloud].get()));
        objects.push_back(LoadObject("assets/sofa.fbx", NULL, materials[ID_mat_duvet].get())); // placeholder mat
    };
};

SceneAssets* getSceneAssets();