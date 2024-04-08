#pragma once

#include <memory>
#include <vector>
#include "../graphics/Model.h"

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

struct Renderer;
struct RenderMaterial;

struct Assets {
  std::vector<std::unique_ptr<RenderMaterial>> materials;

  Assets();
  ~Assets();

  std::vector<Model> createModels() const;
};
