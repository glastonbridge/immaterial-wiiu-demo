#include "PostProcessing.h"
#include "../renderer/RenderMaterial.h"

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
