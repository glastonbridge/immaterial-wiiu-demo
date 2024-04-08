#include "Assets.h"

#include "../graphics/LoadUFBX.h"
#include "../graphics/ObjectFactory.h"
#include "../renderer/Renderer.h"
#include "../renderer/RenderMaterial.h"

namespace {
using RenderMaterial_p = std::unique_ptr<RenderMaterial>;

RenderMaterial_p createProjectedMaterial() {
  std::vector<AttribSpec> attribs;
  attribs.push_back(AttribSpec{"in_position", BufferType::VERTEX,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  attribs.push_back(AttribSpec{"in_color", BufferType::COLOR,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32});
  return std::make_unique<RenderMaterial>("shaders/projected.vert",
                                          "shaders/projected.frag", attribs);
}

RenderMaterial_p createTextureMaterial(const char *path) {
  std::string path_str(path);
  std::vector<AttribSpec> attribs;
  attribs.push_back(AttribSpec{"in_position", BufferType::VERTEX,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  attribs.push_back(AttribSpec{"in_color", BufferType::COLOR,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32});
  attribs.push_back(AttribSpec{"in_texcoord", BufferType::TEXCOORD,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32});
  attribs.push_back(AttribSpec{"in_normal", BufferType::NORMAL,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  auto renderMaterial = std::make_unique<RenderMaterial>(
      "shaders/projected.vert", "shaders/bones.frag", attribs);
  renderMaterial->setTexture(new RenderTexture(path_str));
  return renderMaterial;
}

RenderMaterial_p createBoneMaterial(const char *path) {
  std::string path_str(path);
  std::vector<AttribSpec> attribs;
  attribs.push_back(AttribSpec{"in_position", BufferType::VERTEX,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  attribs.push_back(AttribSpec{"in_texcoord", BufferType::TEXCOORD,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32});
  attribs.push_back(AttribSpec{"in_normal", BufferType::NORMAL,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  attribs.push_back(AttribSpec{"in_bone_idx", BufferType::BONE_IDX,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32});
  attribs.push_back(AttribSpec{"in_bone_weight", BufferType::BONE_WEIGHT,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32});
  auto renderMaterial = std::make_unique<RenderMaterial>(
      "shaders/bones.vert", "shaders/bones.frag", attribs);
  renderMaterial->setTexture(new RenderTexture(path_str));
  return renderMaterial;
}
}  // namespace

Assets::~Assets() = default;

Assets::Assets() {
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
}

void Assets::createModels(Renderer &renderer) const {

  // Load objects
  renderer.addModel(ModelFactory::load("assets/train.fbx", NULL,
                                        materials[ID_mat_train].get()));
  renderer.addModel(ModelFactory::load("assets/cushion.fbx", NULL,
                                        materials[ID_mat_cushion].get()));
  renderer.addModel(ModelFactory::load("assets/house1.fbx", NULL,
                                        materials[ID_mat_house1].get()));
  renderer.addModel(ModelFactory::load("assets/lampshade.fbx", NULL,
                                        materials[ID_mat_lampshade].get()));
  renderer.addModel(ModelFactory::load("assets/house2.fbx", NULL,
                                        materials[ID_mat_house2].get()));
  renderer.addModel(ModelFactory::load("assets/viaduct.fbx", NULL,
                                        materials[ID_mat_viaduct].get()));
  renderer.addModel(ModelFactory::load("assets/duvet_hills.fbx", NULL,
                                        materials[ID_mat_duvet].get()));
  renderer.addModel(
      ModelFactory::load("assets/cardboard_cloud.fbx", NULL,
                          materials[ID_mat_cardboard_cloud].get()));
  renderer.addModel(ModelFactory::load("assets/pillow.fbx", NULL,
                                        materials[ID_mat_pillow].get()));
  renderer.addModel(ModelFactory::load("assets/egg.fbx", "egg",
                                        materials[ID_mat_egg].get()));
  renderer.addModel(
      ModelFactory::load("assets/egg.fbx", NULL, materials[ID_mat_egg].get()));
  renderer.addModel(ModelFactory::load("assets/building.fbx", NULL,
                                        materials[ID_mat_building].get()));
  renderer.addModel(
      ModelFactory::load("assets/station_building.fbx", NULL,
                          materials[ID_mat_station_building].get()));
  renderer.addModel(ModelFactory::load("assets/cloud.fbx", NULL,
                                        materials[ID_mat_cloud].get()));
  renderer.addModel(ModelFactory::load("assets/sofa.fbx", NULL,
                                        materials[ID_mat_sofa].get()));
  renderer.addModel(ModelFactory::load("assets/tablecloth.fbx", NULL,
                                        materials[ID_mat_tablecloth].get()));
  renderer.addModel(ModelFactory::load("assets/egg_carton.fbx", NULL,
                                        materials[ID_mat_egg_carton].get()));
  renderer.addModel(ModelFactory::load("assets/carpet.fbx", NULL,
                                        materials[ID_mat_carpet].get()));

  // load the 3d font fbx once to be reused for text objects:
  std::vector<std::vector<float>> text_vertices;
  std::vector<std::vector<float>> text_normals;
  LoadTypeUFBX("assets/type.fbx", text_vertices, text_normals);

  // Our "font" has the following characters:
  // ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!:?;-."'*~& all letters will be drawn
  // in uppercase more characters can be added if really necessary
  renderer.addModel(ModelFactory::createText(
      materials[ID_mat_text].get(), "slipstream", text_vertices, text_normals));
  renderer.addModel(ModelFactory::createText(
      materials[ID_mat_text].get(), "& svatg", text_vertices, text_normals));
  renderer.addModel(ModelFactory::createText(
      materials[ID_mat_text].get(), "present", text_vertices, text_normals));
  renderer.addModel(ModelFactory::createText(
      materials[ID_mat_text].get(), "immaterial", text_vertices, text_normals));
  renderer.addModel(ModelFactory::createText(materials[ID_mat_text].get(),
                                              "mrs beanbag", text_vertices,
                                              text_normals));
  renderer.addModel(ModelFactory::createText(
      materials[ID_mat_text].get(), "aldroid", text_vertices, text_normals));
  renderer.addModel(ModelFactory::createText(
      materials[ID_mat_text].get(), "vurpo", text_vertices, text_normals));
  renderer.addModel(ModelFactory::createText(
      materials[ID_mat_text].get(), "halcy", text_vertices, text_normals));
  renderer.addModel(ModelFactory::createText(materials[ID_mat_text].get(),
                                              "raccoonviolet", text_vertices,
                                              text_normals));

  renderer.addModel(ModelFactory::load("assets/eepytime.fbx", "Cube",
                                        materials[ID_mat_duvet].get()));
  renderer.addModel(ModelFactory::load("assets/toast_rack_with_toast.fbx",
                                        NULL, materials[ID_mat_toast].get()));
  renderer.addModel(
      ModelFactory::load("assets/crossing_signs.fbx", NULL,
                          materials[ID_mat_crossing_signs].get()));
  renderer.addModel(ModelFactory::load("assets/tablecloth.fbx", NULL,
                                        materials[ID_mat_mattress].get()));
}
