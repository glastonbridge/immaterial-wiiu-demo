#include "MaterialCollection.h"
#include "../renderer/RenderMaterial.h"

RenderMaterial_p createProjectedMaterial() {
  std::vector<AttribSpec> attribs;
  attribs.push_back(AttribSpec{"in_position", BufferType::VERTEX,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  attribs.push_back(AttribSpec{"in_color", BufferType::COLOR,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32});
  return std::make_unique<RenderMaterial>("shaders/projected.vert",
                                     "shaders/projected.frag", attribs);
}

RenderMaterial_p createComposeMaterial() {
  std::vector<AttribSpec> attribs;
  attribs.push_back(AttribSpec{"in_position", BufferType::VERTEX,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  attribs.push_back(AttribSpec{"in_texcoord", BufferType::TEXCOORD,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32});
  return std::make_unique<RenderMaterial>("shaders/billboard.vert",
                                     "shaders/compose.frag", attribs);
}

RenderMaterial_p createBlurMaterial() {
  std::vector<AttribSpec> attribs;
  attribs.push_back(AttribSpec{"in_position", BufferType::VERTEX,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
  attribs.push_back(AttribSpec{"in_texcoord", BufferType::TEXCOORD,
                               GX2_ATTRIB_FORMAT_FLOAT_32_32});
  return std::make_unique<RenderMaterial>("shaders/billboard.vert",
                                     "shaders/blur.frag", attribs);
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
  auto renderMaterial =
      std::make_unique<RenderMaterial>("shaders/projected.vert", "shaders/bones.frag", attribs);
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
  auto renderMaterial = std::make_unique<RenderMaterial>("shaders/bones.vert",
                                          "shaders/bones.frag", attribs);
  renderMaterial->setTexture(new RenderTexture(path_str));
  return renderMaterial;
}
