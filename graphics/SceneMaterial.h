#pragma once

#include <memory>
#include <vector>
#include "../renderer/RenderMaterial.h"
#include "../renderer/RenderTexture.h"
#include <gx2/shaders.h>

struct SceneMaterial {
  SceneMaterial() {};
  virtual ~SceneMaterial() {};
  virtual RenderMaterial* getRenderMaterial() const {
    return renderMaterial.get();
  }

protected:
  std::unique_ptr<RenderMaterial> renderMaterial;
};


// TODO copy these somewhere else
struct ProjectedMaterial: public SceneMaterial {
  ProjectedMaterial() {
    std::vector<AttribSpec> attribs;
    attribs.push_back(AttribSpec {"in_position", BufferType::VERTEX, GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    attribs.push_back(AttribSpec {"in_color", BufferType::COLOR, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32});
    renderMaterial.reset(new RenderMaterial(
      "shaders/projected.vert",
      "shaders/projected.frag",
      attribs
    ));
  }
};

struct BillboardMaterial: public SceneMaterial {
  BillboardMaterial() {
    std::vector<AttribSpec> attribs;
    attribs.push_back(AttribSpec {"in_position", BufferType::VERTEX, GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    attribs.push_back(AttribSpec {"in_color", BufferType::COLOR, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32});
    renderMaterial.reset(new RenderMaterial(
      "shaders/billboard.vert",
      "shaders/billboard.frag",
      attribs
    ));
  }
};


struct TextureMaterial: public SceneMaterial {
  TextureMaterial(const char* path) {
    std::string path_str(path);
    std::vector<AttribSpec> attribs;
    attribs.push_back(AttribSpec {"in_position", BufferType::VERTEX, GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    attribs.push_back(AttribSpec {"in_color", BufferType::COLOR, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32});
    attribs.push_back(AttribSpec {"in_texcoord", BufferType::TEXCOORD, GX2_ATTRIB_FORMAT_FLOAT_32_32});
    attribs.push_back(AttribSpec {"in_normal", BufferType::NORMAL, GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    renderMaterial.reset(new RenderMaterial(
      "shaders/projected.vert",
      "shaders/textured.frag",
      attribs
    ));
    renderMaterial->setTexture(new RenderTexture(path_str));
  }
};

struct BoneMaterial: public SceneMaterial {
  BoneMaterial(const char* path) {
    std::string path_str(path);
    std::vector<AttribSpec> attribs;
    attribs.push_back(AttribSpec {"in_position", BufferType::VERTEX, GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    attribs.push_back(AttribSpec {"in_texcoord", BufferType::TEXCOORD, GX2_ATTRIB_FORMAT_FLOAT_32_32});
    attribs.push_back(AttribSpec {"in_normal", BufferType::NORMAL, GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    attribs.push_back(AttribSpec {"in_bone_idx", BufferType::BONE_IDX, GX2_ATTRIB_FORMAT_FLOAT_32_32});
    attribs.push_back(AttribSpec {"in_bone_weight", BufferType::BONE_WEIGHT, GX2_ATTRIB_FORMAT_FLOAT_32_32});
    renderMaterial.reset(new RenderMaterial(
      "shaders/bones.vert",
      "shaders/bones.frag",
      attribs
    ));
    renderMaterial->setTexture(new RenderTexture(path_str)); // this leaks, doesn't it?
  }
};

