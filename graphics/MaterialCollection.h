#pragma once

#include "SceneMaterial.h"

struct ProjectedMaterial : public SceneMaterial {
  ProjectedMaterial() {
    std::vector<AttribSpec> attribs;
    attribs.push_back(AttribSpec{"in_position", BufferType::VERTEX,
                                 GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    attribs.push_back(AttribSpec{"in_color", BufferType::COLOR,
                                 GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32});
    renderMaterial.reset(new RenderMaterial("shaders/projected.vert",
                                            "shaders/projected.frag", attribs));
  }
};

struct ComposeMaterial : public SceneMaterial {
  ComposeMaterial() {
    std::vector<AttribSpec> attribs;
    attribs.push_back(AttribSpec{"in_position", BufferType::VERTEX,
                                 GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    attribs.push_back(AttribSpec{"in_texcoord", BufferType::TEXCOORD,
                                 GX2_ATTRIB_FORMAT_FLOAT_32_32});
    renderMaterial.reset(new RenderMaterial("shaders/billboard.vert",
                                            "shaders/compose.frag", attribs));
  }
};

struct BlurMaterial : public SceneMaterial {
  BlurMaterial() {
    std::vector<AttribSpec> attribs;
    attribs.push_back(AttribSpec{"in_position", BufferType::VERTEX,
                                 GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    attribs.push_back(AttribSpec{"in_texcoord", BufferType::TEXCOORD,
                                 GX2_ATTRIB_FORMAT_FLOAT_32_32});
    renderMaterial.reset(new RenderMaterial("shaders/billboard.vert",
                                            "shaders/blur.frag", attribs));
  }
};

struct TextureMaterial : public SceneMaterial {
  TextureMaterial(const char *path) {
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
    renderMaterial.reset(new RenderMaterial("shaders/projected.vert",
                                            "shaders/bones.frag", attribs));
    renderMaterial->setTexture(new RenderTexture(path_str));
  }
};

struct BoneMaterial : public SceneMaterial {
  BoneMaterial(const char *path) {
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
    renderMaterial.reset(new RenderMaterial("shaders/bones.vert",
                                            "shaders/bones.frag", attribs));
    renderMaterial->setTexture(new RenderTexture(path_str));
  }
};

struct SkyboxMaterial : public SceneMaterial {
  SkyboxMaterial(const char *path) {
    std::string path_str(path);
    std::vector<AttribSpec> attribs;
    attribs.push_back(AttribSpec{"in_position", BufferType::VERTEX,
                                 GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    renderMaterial.reset(new RenderMaterial("shaders/skybox.vert",
                                            "shaders/skybox.frag", attribs));
    renderMaterial->setTexture(new RenderTexture(path_str, true));
  }
};