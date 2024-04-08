#include "Renderer.h"
#include <cstdlib>
#include <gfd.h>
#include <gx2/draw.h>
#include <gx2/event.h>
#include <gx2/mem.h>
#include <gx2/registers.h>
#include <gx2/shaders.h>
#include <gx2/state.h>
#include <gx2r/buffer.h>
#include <gx2r/draw.h>
#include <stdio.h>
#include <string.h>
#include <whb/file.h>
#include <whb/gfx.h>
#include <whb/log.h>
#include <whb/log_udp.h>

#include <coreinit/systeminfo.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>

#include "../graphics/Model.h"
#include "../scenes/SceneBase.h"
#include "../util/memory.h"
#include "PostProcessing.h"
#include "RenderBuffer.h"
#include "RenderMaterial.h"
#include "RenderObject.h"
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

namespace {
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

/**
 * Generate a quad object.
 */
std::unique_ptr<RenderObject> createQuad(RenderMaterial *material) {
  auto obj = RenderObject::create();

  obj->setMaterial(material);
  obj->setAttribBuffer(BufferType::VERTEX, sPositionDataQuad, 4 * 3, 6);
  obj->setAttribBuffer(BufferType::TEXCOORD, sTexcoordData, 4 * 2, 6);

  return obj;
}
}  // namespace

Renderer::Renderer() {
  composeMaterial = createComposeMaterial();
  composeQuad = createQuad(composeMaterial.get());

  blurMaterial = createBlurMaterial();
  blurQuad = createQuad(blurMaterial.get());

  bufferA = std::make_unique<RenderBuffer>(false, 1280 / 2, 720 / 2);
  bufferB = std::make_unique<RenderBuffer>(false, 1280 / 2, 720 / 2);

  quadInstance = std::make_unique<RenderInstance>();
  view = std::make_unique<RenderView>();
  postView = std::make_unique<RenderView>();
}

void Renderer::renderFrame(SceneBase const &scene, RenderBuffer &rb) {
  // Render to offscreen buffer
  // WHBLogPrint("Binding render target");
  rb.bindTarget(true);

  // Turn on culling, depth test and write
  GX2SetCullOnlyControl(GX2_FRONT_FACE_CCW, GX2_DISABLE, GX2_ENABLE);
  GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_LESS);

  float *cameraProjection = (float *)glm::value_ptr(scene.cameraProjection);

  if (scene.instances.size() > instances.size()) {
    instances.resize(scene.instances.size());
  }
  auto rit = instances.begin();

  view->setUniformFloatMat(UniformType::CAMERA_PROJECTION,
                           (float *)glm::value_ptr(scene.cameraProjection), 16);
  view->setUniformFloatMat(UniformType::CAMERA_VIEW,
                           (float *)glm::value_ptr(scene.cameraView), 16);
  view->setExtraUniform(0, scene.cameraOptions);

  auto const numObjects = objectList.size();
  for (auto const &instance : scene.instances) {
    if (instance.id >= numObjects) {
      // just in case..
      continue;
    }
    auto const &object = *objectList[instance.id];
    auto &ri = *rit++;

    float *mat = (float *)glm::value_ptr(instance.transform);
    // WHBLogPrintf("Rendering object with transform %s",
    // glm::to_string(instance.transform).c_str());
    ri.setUniformFloatMat(UniformType::TRANSFORM, mat, 16);
    object.applyAnimation(instance.anim, ri);
    object.render(ri, *view);
  }
  rb.unbindTarget();

  // Apply blur
  for (int i = 0; i < 2; i++) {
    float scale = (float)(1 << (i));

    // Horizontal
    bufferA->bindTarget(false);
    GX2SetDepthOnlyControl(GX2_DISABLE, GX2_DISABLE, GX2_COMPARE_FUNC_LESS);

    if (i == 0) {
      rb.renderUsing(blurQuad->getMaterial()->group);
    } else {
      bufferB->renderUsing(blurQuad->getMaterial()->group);
    }
    postView->setExtraUniform(0, glm::vec4(scale, 0.0001f, 0.0f, 0.0f));
    blurQuad->render(*quadInstance, *postView);
    bufferA->unbindTarget();

    // Vertical
    bufferB->bindTarget(false);
    GX2SetDepthOnlyControl(GX2_DISABLE, GX2_DISABLE, GX2_COMPARE_FUNC_LESS);

    bufferA->renderUsing(blurQuad->getMaterial()->group);
    postView->setExtraUniform(0, glm::vec4(scale, 1.0001f, 0.0f, 0.0f));
    blurQuad->render(*quadInstance, *postView);
    bufferB->unbindTarget();
  }

  //  Render to screen with post processing
  WHBGfxBeginRender();
  WHBGfxBeginRenderTV();
  WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
  bufferB->renderUsing(composeQuad->getMaterial()->group);
  rb.renderUsing(composeQuad->getMaterial()->group, 1);
  postView->setExtraUniform(0, scene.processOptions);
  composeQuad->render(*quadInstance, *postView);
  WHBGfxFinishRenderTV();

  WHBGfxBeginRenderDRC();
  WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
  bufferB->renderUsing(composeQuad->getMaterial()->group, 0);
  rb.renderUsing(composeQuad->getMaterial()->group, 1);
  postView->setExtraUniform(0, scene.processOptions);
  composeQuad->render(*quadInstance, *postView);
  WHBGfxFinishRenderDRC();

  WHBGfxFinishRender();
}

Renderer::~Renderer() { WHBLogPrintf("Exiting..."); }

void Renderer::addModel(Model const &model) {
  objectList.push_back(RenderObject::create(model));
}

void Renderer::addModel(Model &&model) {
  objectList.push_back(RenderObject::create(std::move(model)));
}

void Renderer::reserve(size_t n) {
  if (instances.size() < n) {
    instances.resize(n);
  }
}