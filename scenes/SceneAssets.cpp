#include "SceneAssets.h"

#include "../graphics/LoadUFBX.h"
#include "../graphics/MaterialCollection.h"
#include "../graphics/ObjectFactory.h"
#include "../renderer/RenderMaterial.h"

// Getter for the asset store
static SceneAssets *globalAssetStore = nullptr;
SceneAssets *getSceneAssets() {
  if (globalAssetStore == nullptr) {
    globalAssetStore = new SceneAssets();
  }
  return globalAssetStore;
}

void destroySceneAssets() {
  if (globalAssetStore != nullptr) {
    delete globalAssetStore;
    globalAssetStore = nullptr;
  }
}

SceneAssets::~SceneAssets() = default;

SceneAssets::SceneAssets() {
  // Load materials
  materials.push_back(createBoneMaterial("assets/train_small.png"));
  materials.push_back(createBoneMaterial("assets/duvet.png"));
  materials.push_back(createBoneMaterial("assets/house1_small.png"));
  materials.push_back(createBoneMaterial("assets/lampshade_small.png"));
  materials.push_back(createBoneMaterial("assets/house2_small.png"));
  materials.push_back(createBoneMaterial("assets/viaduct.png"));
  materials.push_back(createBoneMaterial("assets/cloud_basecolour.png"));
  materials.push_back(createBoneMaterial("assets/egg.png"));
  materials.push_back(createBoneMaterial("assets/building.png"));
  materials.push_back(createBoneMaterial("assets/station_building.png"));
  materials.push_back(createBoneMaterial("assets/cardboard_cloud.png"));
  materials.push_back(createBoneMaterial("assets/sofa.png"));
  materials.push_back(createBoneMaterial("assets/pillow.png"));
  materials.push_back(createBoneMaterial("assets/cushion.png"));
  materials.push_back(createBoneMaterial("assets/tablecloth.png"));
  materials.push_back(createBoneMaterial("assets/egg_carton.png"));
  materials.push_back(createBoneMaterial("assets/carpet.png"));
  materials.push_back(createTextureMaterial("assets/viaduct.png"));
  materials.push_back(createBoneMaterial("assets/toast_rack_with_toast.png"));
  materials.push_back(createBoneMaterial("assets/crossing_signs.png"));
  materials.push_back(createBoneMaterial("assets/mattress.png"));

  // Load objects
  objects.push_back(ObjectFactory::load("assets/train.fbx", NULL,
                                        materials[ID_mat_train].get()));
  objects.push_back(ObjectFactory::load("assets/cushion.fbx", NULL,
                                        materials[ID_mat_cushion].get()));
  objects.push_back(ObjectFactory::load("assets/house1.fbx", NULL,
                                        materials[ID_mat_house1].get()));
  objects.push_back(ObjectFactory::load("assets/lampshade.fbx", NULL,
                                        materials[ID_mat_lampshade].get()));
  objects.push_back(ObjectFactory::load("assets/house2.fbx", NULL,
                                        materials[ID_mat_house2].get()));
  objects.push_back(ObjectFactory::load("assets/viaduct.fbx", NULL,
                                        materials[ID_mat_viaduct].get()));
  objects.push_back(ObjectFactory::load("assets/duvet_hills.fbx", NULL,
                                        materials[ID_mat_duvet].get()));
  objects.push_back(
      ObjectFactory::load("assets/cardboard_cloud.fbx", NULL,
                          materials[ID_mat_cardboard_cloud].get()));
  objects.push_back(ObjectFactory::load("assets/pillow.fbx", NULL,
                                        materials[ID_mat_pillow].get()));
  objects.push_back(ObjectFactory::load("assets/egg.fbx", "egg",
                                        materials[ID_mat_egg].get()));
  objects.push_back(
      ObjectFactory::load("assets/egg.fbx", NULL, materials[ID_mat_egg].get()));
  objects.push_back(ObjectFactory::load("assets/building.fbx", NULL,
                                        materials[ID_mat_building].get()));
  objects.push_back(
      ObjectFactory::load("assets/station_building.fbx", NULL,
                          materials[ID_mat_station_building].get()));
  objects.push_back(ObjectFactory::load("assets/cloud.fbx", NULL,
                                        materials[ID_mat_cloud].get()));
  objects.push_back(ObjectFactory::load("assets/sofa.fbx", NULL,
                                        materials[ID_mat_sofa].get()));
  objects.push_back(ObjectFactory::load("assets/tablecloth.fbx", NULL,
                                        materials[ID_mat_tablecloth].get()));
  objects.push_back(ObjectFactory::load("assets/egg_carton.fbx", NULL,
                                        materials[ID_mat_egg_carton].get()));
  objects.push_back(ObjectFactory::load("assets/carpet.fbx", NULL,
                                        materials[ID_mat_carpet].get()));

  // load the 3d font fbx once to be reused for text objects:
  std::vector<std::vector<float>> text_vertices;
  std::vector<std::vector<float>> text_normals;
  LoadTypeUFBX("assets/type.fbx", text_vertices, text_normals);

  // Our "font" has the following characters:
  // ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!:?;-."'*~& all letters will be drawn
  // in uppercase more characters can be added if really necessary
  objects.push_back(ObjectFactory::createText(
      materials[ID_mat_text].get(), "slipstream", text_vertices, text_normals));
  objects.push_back(ObjectFactory::createText(
      materials[ID_mat_text].get(), "& svatg", text_vertices, text_normals));
  objects.push_back(ObjectFactory::createText(
      materials[ID_mat_text].get(), "present", text_vertices, text_normals));
  objects.push_back(ObjectFactory::createText(
      materials[ID_mat_text].get(), "immaterial", text_vertices, text_normals));
  objects.push_back(ObjectFactory::createText(materials[ID_mat_text].get(),
                                              "mrs beanbag", text_vertices,
                                              text_normals));
  objects.push_back(ObjectFactory::createText(
      materials[ID_mat_text].get(), "aldroid", text_vertices, text_normals));
  objects.push_back(ObjectFactory::createText(
      materials[ID_mat_text].get(), "vurpo", text_vertices, text_normals));
  objects.push_back(ObjectFactory::createText(
      materials[ID_mat_text].get(), "halcy", text_vertices, text_normals));
  objects.push_back(ObjectFactory::createText(materials[ID_mat_text].get(),
                                              "raccoonviolet", text_vertices,
                                              text_normals));

  objects.push_back(ObjectFactory::load("assets/eepytime.fbx", "Cube",
                                        materials[ID_mat_duvet].get()));
  objects.push_back(ObjectFactory::load("assets/toast_rack_with_toast.fbx",
                                        NULL, materials[ID_mat_toast].get()));
  objects.push_back(
      ObjectFactory::load("assets/crossing_signs.fbx", NULL,
                          materials[ID_mat_crossing_signs].get()));
  objects.push_back(ObjectFactory::load("assets/tablecloth.fbx", NULL,
                                        materials[ID_mat_mattress].get()));
}
