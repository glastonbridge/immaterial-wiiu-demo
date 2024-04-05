
#include "LoadUFBX.h"
#include "SceneMaterial.h"
#include <gx2/enum.h>
#include <malloc.h>
#include <memory>

#include "../renderer/RenderObject.h"
#include "MaterialCollection.h"
#include "SceneObject.h"
#include <whb/log.h>

static const float sPositionDataQuad[] = {
    // tri 1
    -1.0f,
    1.0f,
    0.0f,
    1.0f,
    1.0f,
    0.0f,
    -1.0f,
    -1.0f,
    0.0f,

    // tri 2
    1.0f,
    1.0f,
    0.0f,
    -1.0f,
    -1.0f,
    0.0f,
    1.0f,
    -1.0f,
    0.0f,
};

static const float sTexcoordData[] = {
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
};

SceneObject::SceneObject() {}

SceneObject::~SceneObject() {}

RenderObject *SceneObject::getRenderObject() { return renderObject.get(); }

struct SceneObjectImpl : public SceneObject {
  SceneObjectImpl() { renderObject.reset(new RenderObject()); }
  void setMaterial(SceneMaterial *material) {
    renderObject->setMaterial(material->getRenderMaterial());
  }
};

void SceneObject::setAnimationFrame(float frame) {
  // Bail if we don't have any frames
  if (this->animFrames.size() == 0) {
    glm::mat4 boneFrameMat = glm::mat4(1.0f);
    this->getRenderObject()->setUniformFloatMat(
        UniformType::BONE_TRANSFORM, glm::value_ptr(boneFrameMat), 4 * 4);
    return;
  }

  // Alloc buffer if we need one
  size_t numBones = this->animFrames[0].size();
  if (this->boneMatInterpBuffer == nullptr) {
    WHBLogPrintf("Allocating bone interp buffer for %d bones", numBones);
    this->boneMatInterpBuffer.reset(new float[4 * 4 * numBones]);
  }

  // Figure out where in the animation we are
  size_t numFrames = this->animFrames.size();
  int animPos = (int)frame;
  float animPosRemainder = frame - (float)animPos;
  animPos = animPos % numFrames;
  int animPosNext = (animPos + 1) % numFrames;
  WHBLogPrintf("animPos: %d, animPosNext: %d, animPosRemainder: %f", animPos,
               animPosNext, animPosRemainder);

  // Copy all the bone mats into the array, interpolating linearly between two
  // adjacent frames
  for (int i = 0; i < numBones; i++) {
    glm::mat4 boneFrameMat =
        this->animFrames[animPos][i] * (1.0f - animPosRemainder) +
        this->animFrames[animPosNext][i] * animPosRemainder;
    memcpy(this->boneMatInterpBuffer.get() + (i * 4 * 4),
           glm::value_ptr(boneFrameMat), 4 * 4 * sizeof(float));
  }

  // Bones to shader buffer
  WHBLogPrintf("Setting bone transform uniform");
  this->getRenderObject()->setUniformFloatMat(
      UniformType::BONE_TRANSFORM, this->boneMatInterpBuffer.get(), 4 * 4 * numBones);
}

/**
 * Load an object from a file.
 */
std::unique_ptr<SceneObject> LoadObject(const char *path, const char *name,
                                        SceneMaterial *material) {
  WHBLogPrintf("Loading object %s from %s", name, path);
  std::unique_ptr<SceneObjectImpl> _impl;
  _impl.reset(new SceneObjectImpl());

  std::vector<float> vertices;
  std::vector<float> texcoords;
  std::vector<float> normals;
  std::vector<float> boneIndices;
  std::vector<float> boneWeights;

  WHBLogPrintf("Call LoadUFBX...");
  int success = LoadUFBX(path, name, vertices, texcoords, normals, boneIndices,
                         boneWeights, _impl->animFrames);
  if (!success) {
    WHBLogPrintf("Failed to load UFBX for the first time, trying again.");
    LoadUFBX(path, name, vertices, texcoords, normals, boneIndices, boneWeights,
             _impl->animFrames);
  }

  std::vector<float> vertexColors;
  for (uint32_t i = 0; i < normals.size() * 4 / 3; ++i) {
    vertexColors.push_back(0);
  }

  WHBLogPrintf("Set attrib buffer...");
  _impl->setMaterial(material);
  _impl->getRenderObject()->setAttribBuffer(BufferType::VERTEX, vertices.data(),
                                            4 * 3, vertices.size() / 3);
  _impl->getRenderObject()->setAttribBuffer(
      BufferType::COLOR, vertexColors.data(), 4 * 4, vertexColors.size() / 4);
  _impl->getRenderObject()->setAttribBuffer(
      BufferType::TEXCOORD, texcoords.data(), 4 * 2, texcoords.size() / 2);
  _impl->getRenderObject()->setAttribBuffer(BufferType::NORMAL, normals.data(),
                                            4 * 3, normals.size() / 3);
  _impl->getRenderObject()->setAttribBuffer(
      BufferType::BONE_IDX, boneIndices.data(), 2 * 4, boneIndices.size() / 2);
  _impl->getRenderObject()->setAttribBuffer(BufferType::BONE_WEIGHT,
                                            boneWeights.data(), 2 * 4,
                                            boneWeights.size() / 2);

  return _impl;
}

/**
 * Generate a quad object.
 */
std::unique_ptr<SceneObject> LoadQuad(SceneMaterial *material) {
  std::unique_ptr<SceneObjectImpl> _impl;
  _impl.reset(new SceneObjectImpl());

  _impl->setMaterial(material);
  _impl->getRenderObject()->setAttribBuffer(BufferType::VERTEX,
                                            sPositionDataQuad, 4 * 3, 6);
  _impl->getRenderObject()->setAttribBuffer(BufferType::TEXCOORD, sTexcoordData,
                                            4 * 2, 6);

  return _impl;
}

// this is not real kerning, so it's always going to look at least a bit janky
// this is just the offset from the start of one character to the start of the
// next one Adjust if necessary
const float x_widths[47] = {
    // the letters
    22.f, 19.f, 21.f, 21.f, 18.f, 17.f, 24.f, 22.f, 11.f, 17.f, 21.f, 17.f,
    28.f, 23.f, 24.f, 19.f, 25.f, 20.f, 19.f, 19.f, 23.f, 22.f, 30.f, 21.f,
    22.f, 22.f,
    // the numbers
    16.f, 19.f, 19.f, 21.f, 19.f, 19.f, 19.f, 19.f, 19.f, 19.f,
    // the symbols
    13.f, 13.f, 20.f, 13.f, 16.f, 13.f, 15.f, 10.f, 16.f, 18.f, 20.f};

/**
 * Generate a text object.
 */
std::unique_ptr<SceneObject> LoadText(SceneMaterial *material, std::string text,
                                      std::vector<std::vector<float>> vertices,
                                      std::vector<std::vector<float>> normals) {
  std::unique_ptr<SceneObjectImpl> _impl;
  _impl.reset(new SceneObjectImpl());

  std::vector<float> stringVertices;
  std::vector<float> stringVertexColors;
  std::vector<float> stringTexCoords;
  std::vector<float> stringNormals;

  float x_position = 0;

  for (char &c : text) {
    uint32_t character_index;
    float y_offset = 0;
    switch (c) {
    case ' ':
      x_position += 10.f;
      continue;
      break;
    case 'A':
    case 'a':
      character_index = 0;
      break;
    case 'B':
    case 'b':
      character_index = 1;
      break;
    case 'C':
    case 'c':
      character_index = 2;
      break;
    case 'D':
    case 'd':
      character_index = 3;
      break;
    case 'E':
    case 'e':
      character_index = 4;
      break;
    case 'F':
    case 'f':
      character_index = 5;
      break;
    case 'G':
    case 'g':
      character_index = 6;
      break;
    case 'H':
    case 'h':
      character_index = 7;
      break;
    case 'I':
    case 'i':
      character_index = 8;
      break;
    case 'J':
    case 'j':
      character_index = 9;
      break;
    case 'K':
    case 'k':
      character_index = 10;
      break;
    case 'L':
    case 'l':
      character_index = 11;
      break;
    case 'M':
    case 'm':
      character_index = 12;
      break;
    case 'N':
    case 'n':
      character_index = 13;
      break;
    case 'O':
    case 'o':
      character_index = 14;
      break;
    case 'P':
    case 'p':
      character_index = 15;
      break;
    case 'Q':
    case 'q':
      character_index = 16;
      break;
    case 'R':
    case 'r':
      character_index = 17;
      break;
    case 'S':
    case 's':
      character_index = 18;
      break;
    case 'T':
    case 't':
      character_index = 19;
      break;
    case 'U':
    case 'u':
      character_index = 20;
      break;
    case 'V':
    case 'v':
      character_index = 21;
      break;
    case 'W':
    case 'w':
      character_index = 22;
      break;
    case 'X':
    case 'x':
      character_index = 23;
      break;
    case 'Y':
    case 'y':
      character_index = 24;
      break;
    case 'Z':
    case 'z':
      character_index = 25;
      break;
    case '1':
      character_index = 26;
      break;
    case '2':
      character_index = 27;
      break;
    case '3':
      character_index = 28;
      break;
    case '4':
      character_index = 29;
      break;
    case '5':
      character_index = 30;
      break;
    case '6':
      character_index = 31;
      break;
    case '7':
      character_index = 32;
      break;
    case '8':
      character_index = 33;
      break;
    case '9':
      character_index = 34;
      break;
    case '0':
      character_index = 35;
      break;
    case '!':
      character_index = 36;
      break;
    case ':':
      character_index = 37;
      break;
    case '?':
      character_index = 38;
      break;
    case ';':
      character_index = 39;
      break;
    case '-':
      character_index = 40;
      break;
    case '.':
      character_index = 41;
      y_offset = -6.f;
      break;
    case '"':
      character_index = 42;
      y_offset = 6.f;
      break;
    case '\'':
      character_index = 43;
      y_offset = 6.f;
      break;
    case '*':
      character_index = 44;
      break;
    case '~':
      character_index = 45;
      break;
    case '&':
      character_index = 46;
      break;
    default:
      WHBLogPrintf("We don't have character %c in our font! skipping", c);
      continue;
      break;
    }

    for (uint32_t i = 0; i < vertices[character_index].size(); i += 3) {
      stringVertices.push_back(vertices[character_index][i] + x_position);
      stringVertices.push_back(vertices[character_index][i + 1] + y_offset);
      stringVertices.push_back(vertices[character_index][i + 2]);

      stringTexCoords.push_back(0.375f);
      stringTexCoords.push_back(0.125f);
    }
    for (uint32_t i = 0; i < normals[character_index].size() * 4 / 3; ++i) {
      stringVertexColors.push_back(0);
    }
    for (float &n : normals[character_index]) {
      stringNormals.push_back(n);
    }
    x_position += x_widths[character_index];
  }

  _impl->setMaterial(material);
  _impl->getRenderObject()->setAttribBuffer(BufferType::VERTEX,
                                            stringVertices.data(), 4 * 3,
                                            stringVertices.size() / 3);
  _impl->getRenderObject()->setAttribBuffer(BufferType::COLOR,
                                            stringVertexColors.data(), 4 * 4,
                                            stringVertexColors.size() / 4);
  _impl->getRenderObject()->setAttribBuffer(BufferType::TEXCOORD,
                                            stringTexCoords.data(), 4 * 2,
                                            stringTexCoords.size() / 2);
  _impl->getRenderObject()->setAttribBuffer(BufferType::NORMAL,
                                            stringNormals.data(), 4 * 3,
                                            stringNormals.size() / 3);

  return _impl;
}