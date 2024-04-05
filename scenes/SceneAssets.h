#pragma once

#include "../graphics/LoadUFBX.h"
#include "../graphics/MaterialCollection.h"
#include "../graphics/ObjectFactory.h"
#include "../renderer/RenderObject.h"

enum materialID : size_t {
  ID_mat_train,
  ID_mat_duvet,
  ID_mat_house1,
  ID_mat_lampshade,
  ID_mat_house2,
  ID_mat_viaduct,
  ID_mat_cloud,
  ID_mat_egg,
  ID_mat_building,
  ID_mat_station_building,
  ID_mat_cardboard_cloud,
  ID_mat_sofa,
  ID_mat_pillow,
  ID_mat_cushion,
  ID_mat_tablecloth,
  ID_mat_egg_carton,
  ID_mat_carpet,
  ID_mat_text,
  ID_mat_toast,
  ID_mat_crossing_signs,
  ID_mat_mattress
};

enum objectID : size_t {
  ID_train,
  ID_cushion,
  ID_house1,
  ID_lampshade,
  ID_house2,
  ID_viaduct,
  ID_duvet_hills,
  ID_cardboard_cloud,
  ID_pillow,
  ID_egg,
  ID_egg_walking_sausage,
  ID_building,
  ID_station_building,
  ID_cloud,
  ID_sofa,
  ID_tablecloth_land,
  ID_egg_carton,
  ID_carpet,

  ID_string_slipstream,
  ID_string_svatg,
  ID_string_presents,
  ID_string_title,
  ID_string_beanbag,
  ID_string_aldroid,
  ID_string_vurpo,
  ID_string_halcy,
  ID_string_violet,

  ID_eepytime,
  ID_toast,
  ID_crossing_signs,
  ID_mattress,
  _ID_ASSETS_MAX // keep at end
};

struct SceneAssets {
  std::vector<std::unique_ptr<RenderObject>> objects;
  std::vector<std::unique_ptr<SceneMaterial>> materials;

  SceneAssets() {
    // Load materials
    materials.push_back(
        std::make_unique<BoneMaterial>("assets/train_small.png"));
    materials.push_back(std::make_unique<BoneMaterial>("assets/duvet.png"));
    materials.push_back(
        std::make_unique<BoneMaterial>("assets/house1_small.png"));
    materials.push_back(
        std::make_unique<BoneMaterial>("assets/lampshade_small.png"));
    materials.push_back(
        std::make_unique<BoneMaterial>("assets/house2_small.png"));
    materials.push_back(std::make_unique<BoneMaterial>("assets/viaduct.png"));
    materials.push_back(
        std::make_unique<BoneMaterial>("assets/cloud_basecolour.png"));
    materials.push_back(std::make_unique<BoneMaterial>("assets/egg.png"));
    materials.push_back(std::make_unique<BoneMaterial>("assets/building.png"));
    materials.push_back(
        std::make_unique<BoneMaterial>("assets/station_building.png"));
    materials.push_back(
        std::make_unique<BoneMaterial>("assets/cardboard_cloud.png"));
    materials.push_back(std::make_unique<BoneMaterial>("assets/sofa.png"));
    materials.push_back(std::make_unique<BoneMaterial>("assets/pillow.png"));
    materials.push_back(std::make_unique<BoneMaterial>("assets/cushion.png"));
    materials.push_back(
        std::make_unique<BoneMaterial>("assets/tablecloth.png"));
    materials.push_back(
        std::make_unique<BoneMaterial>("assets/egg_carton.png"));
    materials.push_back(std::make_unique<BoneMaterial>("assets/carpet.png"));
    materials.push_back(
        std::make_unique<TextureMaterial>("assets/viaduct.png"));
    materials.push_back(
        std::make_unique<BoneMaterial>("assets/toast_rack_with_toast.png"));
    materials.push_back(
        std::make_unique<BoneMaterial>("assets/crossing_signs.png"));
    materials.push_back(std::make_unique<BoneMaterial>("assets/mattress.png"));

    // Load objects
    objects.push_back(
        ObjectFactory::load("assets/train.fbx", NULL,
                            materials[ID_mat_train]->getRenderMaterial()));
    objects.push_back(
        ObjectFactory::load("assets/cushion.fbx", NULL,
                            materials[ID_mat_cushion]->getRenderMaterial()));
    objects.push_back(
        ObjectFactory::load("assets/house1.fbx", NULL,
                            materials[ID_mat_house1]->getRenderMaterial()));
    objects.push_back(
        ObjectFactory::load("assets/lampshade.fbx", NULL,
                            materials[ID_mat_lampshade]->getRenderMaterial()));
    objects.push_back(
        ObjectFactory::load("assets/house2.fbx", NULL,
                            materials[ID_mat_house2]->getRenderMaterial()));
    objects.push_back(
        ObjectFactory::load("assets/viaduct.fbx", NULL,
                            materials[ID_mat_viaduct]->getRenderMaterial()));
    objects.push_back(
        ObjectFactory::load("assets/duvet_hills.fbx", NULL,
                            materials[ID_mat_duvet]->getRenderMaterial()));
    objects.push_back(ObjectFactory::load(
        "assets/cardboard_cloud.fbx", NULL,
        materials[ID_mat_cardboard_cloud]->getRenderMaterial()));
    objects.push_back(
        ObjectFactory::load("assets/pillow.fbx", NULL,
                            materials[ID_mat_pillow]->getRenderMaterial()));
    objects.push_back(ObjectFactory::load(
        "assets/egg.fbx", "egg", materials[ID_mat_egg]->getRenderMaterial()));
    objects.push_back(ObjectFactory::load(
        "assets/egg.fbx", NULL, materials[ID_mat_egg]->getRenderMaterial()));
    objects.push_back(
        ObjectFactory::load("assets/building.fbx", NULL,
                            materials[ID_mat_building]->getRenderMaterial()));
    objects.push_back(ObjectFactory::load(
        "assets/station_building.fbx", NULL,
        materials[ID_mat_station_building]->getRenderMaterial()));
    objects.push_back(
        ObjectFactory::load("assets/cloud.fbx", NULL,
                            materials[ID_mat_cloud]->getRenderMaterial()));
    objects.push_back(ObjectFactory::load(
        "assets/sofa.fbx", NULL, materials[ID_mat_sofa]->getRenderMaterial()));
    objects.push_back(
        ObjectFactory::load("assets/tablecloth.fbx", NULL,
                            materials[ID_mat_tablecloth]->getRenderMaterial()));
    objects.push_back(
        ObjectFactory::load("assets/egg_carton.fbx", NULL,
                            materials[ID_mat_egg_carton]->getRenderMaterial()));
    objects.push_back(
        ObjectFactory::load("assets/carpet.fbx", NULL,
                            materials[ID_mat_carpet]->getRenderMaterial()));

    // load the 3d font fbx once to be reused for text objects:
    std::vector<std::vector<float>> text_vertices;
    std::vector<std::vector<float>> text_normals;
    LoadTypeUFBX("assets/type.fbx", text_vertices, text_normals);

    // Our "font" has the following characters:
    // ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!:?;-."'*~& all letters will be drawn
    // in uppercase more characters can be added if really necessary
    objects.push_back(
        ObjectFactory::createText(materials[ID_mat_text]->getRenderMaterial(),
                                  "slipstream", text_vertices, text_normals));
    objects.push_back(
        ObjectFactory::createText(materials[ID_mat_text]->getRenderMaterial(),
                                  "& svatg", text_vertices, text_normals));
    objects.push_back(
        ObjectFactory::createText(materials[ID_mat_text]->getRenderMaterial(),
                                  "present", text_vertices, text_normals));
    objects.push_back(
        ObjectFactory::createText(materials[ID_mat_text]->getRenderMaterial(),
                                  "immaterial", text_vertices, text_normals));
    objects.push_back(
        ObjectFactory::createText(materials[ID_mat_text]->getRenderMaterial(),
                                  "mrs beanbag", text_vertices, text_normals));
    objects.push_back(
        ObjectFactory::createText(materials[ID_mat_text]->getRenderMaterial(),
                                  "aldroid", text_vertices, text_normals));
    objects.push_back(
        ObjectFactory::createText(materials[ID_mat_text]->getRenderMaterial(),
                                  "vurpo", text_vertices, text_normals));
    objects.push_back(
        ObjectFactory::createText(materials[ID_mat_text]->getRenderMaterial(),
                                  "halcy", text_vertices, text_normals));
    objects.push_back(ObjectFactory::createText(
        materials[ID_mat_text]->getRenderMaterial(), "raccoonviolet",
        text_vertices, text_normals));

    objects.push_back(
        ObjectFactory::load("assets/eepytime.fbx", "Cube",
                            materials[ID_mat_duvet]->getRenderMaterial()));
    objects.push_back(
        ObjectFactory::load("assets/toast_rack_with_toast.fbx", NULL,
                            materials[ID_mat_toast]->getRenderMaterial()));
    objects.push_back(ObjectFactory::load(
        "assets/crossing_signs.fbx", NULL,
        materials[ID_mat_crossing_signs]->getRenderMaterial()));
    objects.push_back(
        ObjectFactory::load("assets/tablecloth.fbx", NULL,
                            materials[ID_mat_mattress]->getRenderMaterial()));
  };
};

SceneAssets *getSceneAssets();
void destroySceneAssets();