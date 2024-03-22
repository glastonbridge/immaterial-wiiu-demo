#include "SceneAssets.h"

SceneAssets::SceneAssets() {
    // Load materials
    materials.push_back(std::make_unique<BoneMaterial>("assets/train_small.png"));
    materials.push_back(std::make_unique<BoneMaterial>("assets/train_small.png")); // no cushion texture, using train instead (for now)
    materials.push_back(std::make_unique<BoneMaterial>("assets/house1_small.png"));
    materials.push_back(std::make_unique<BoneMaterial>("assets/lampshade_small.png"));
    materials.push_back(std::make_unique<BoneMaterial>("assets/colorgrid.png"));

    // Load objects
    objects.push_back(LoadObject("assets/train.fbx", NULL, materials[ID_train_mat].get()));
    objects.push_back(LoadObject("assets/cushion.fbx", NULL, materials[ID_cushion_mat].get())); 
    objects.push_back(LoadObject("assets/house1.fbx", NULL, materials[ID_house1_mat].get()));
    objects.push_back(LoadObject("assets/lampshade.fbx", NULL, materials[ID_lampshade_mat].get()));
    objects.push_back(LoadObject("assets/gizmos.fbx", "Cube", materials[ID_colorgrid_mat].get()));
};

// Getter for the asset store
static SceneAssets* globalAssetStore = nullptr;
SceneAssets* getSceneAssets() {
    if (globalAssetStore == nullptr) {
        globalAssetStore = new SceneAssets();
    }
    return globalAssetStore;
}
