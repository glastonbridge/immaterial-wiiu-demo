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

#include "../graphics/MaterialCollection.h"
#include "../graphics/ObjectFactory.h"
#include "../scenes/SceneBase.h"
#include "../util/memory.h"
#include "RenderMaterial.h"
#include "RenderObject.h"
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../scenes/SceneAssets.h"

Renderer::Renderer() {
  composeMaterial = createComposeMaterial();
  composeQuad = ObjectFactory::createQuad(composeMaterial.get());

  blurMaterial = createBlurMaterial();
  blurQuad = ObjectFactory::createQuad(blurMaterial.get());

  bufferA = std::make_unique<RenderBuffer>(false, 1280 / 2, 720 / 2);
  bufferB = std::make_unique<RenderBuffer>(false, 1280 / 2, 720 / 2);

  quadInstance = std::make_unique<RenderInstance>();
}

void Renderer::renderFrame(const SceneBase &scene) {
  // Asset loader that just loads all assets at startup
  // we can make this better if it turns out we're low on RAM, but we're
  // probably not
  SceneAssets *assets = getSceneAssets();

  // Render to offscreen buffer
  // WHBLogPrint("Binding render target");
  scene.renderBuffer->bindTarget(true);

  // Turn on culling, depth test and write
  GX2SetCullOnlyControl(GX2_FRONT_FACE_CCW, GX2_DISABLE, GX2_ENABLE);
  GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_LESS);

  float *cameraProjection = (float *)glm::value_ptr(scene.cameraProjection);

  instances.resize(scene.instances.size());
  auto rit = instances.begin();

  for (auto const &instance : scene.instances) {
    auto &ri = *rit++;
    auto &object = *assets->objects[instance.id];
    
    object.setUniformFloatMat(UniformType::CAMERA_PROJECTION,
                              (float *)glm::value_ptr(scene.cameraProjection),
                              16);
    object.setUniformFloatMat(UniformType::CAMERA_VIEW,
                              (float *)glm::value_ptr(scene.cameraView), 16);
    object.setExtraUniform(0, scene.cameraOptions);

    float *mat = (float *)glm::value_ptr(instance.transform);
    // WHBLogPrintf("Rendering object with transform %s",
    // glm::to_string(instance.transform).c_str());
    ri.setUniformFloatMat(UniformType::TRANSFORM, mat, 16);
    instance.applyAnimation(object, ri);
    object.render(ri);
  }
  scene.renderBuffer->unbindTarget();

  // Apply blur
  for (int i = 0; i < 2; i++) {
    float scale = (float)(1 << (i));

    // Horizontal
    bufferA->bindTarget(false);
    GX2SetDepthOnlyControl(GX2_DISABLE, GX2_DISABLE, GX2_COMPARE_FUNC_LESS);

    if (i == 0) {
      scene.renderBuffer->renderUsing(blurQuad->getMaterial()->group);
    } else {
      bufferB->renderUsing(blurQuad->getMaterial()->group);
    }
    blurQuad->setExtraUniform(0, glm::vec4(scale, 0.0001f, 0.0f, 0.0f));
    blurQuad->render(*quadInstance);
    bufferA->unbindTarget();

    // Vertical
    bufferB->bindTarget(false);
    GX2SetDepthOnlyControl(GX2_DISABLE, GX2_DISABLE, GX2_COMPARE_FUNC_LESS);

    bufferA->renderUsing(blurQuad->getMaterial()->group);
    blurQuad->setExtraUniform(0, glm::vec4(scale, 1.0001f, 0.0f, 0.0f));
    blurQuad->render(*quadInstance);
    bufferB->unbindTarget();
  }

  //  Render to screen with post processing
  WHBGfxBeginRender();
  WHBGfxBeginRenderTV();
  WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
  bufferB->renderUsing(composeQuad->getMaterial()->group);
  scene.renderBuffer->renderUsing(composeQuad->getMaterial()->group, 1);
  composeQuad->setExtraUniform(0, scene.processOptions);
  composeQuad->render(*quadInstance);
  WHBGfxFinishRenderTV();

  WHBGfxBeginRenderDRC();
  WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
  bufferB->renderUsing(composeQuad->getMaterial()->group, 0);
  scene.renderBuffer->renderUsing(composeQuad->getMaterial()->group, 1);
  composeQuad->setExtraUniform(0, scene.processOptions);
  composeQuad->render(*quadInstance);
  WHBGfxFinishRenderDRC();

  WHBGfxFinishRender();
}

Renderer::~Renderer() { WHBLogPrintf("Exiting..."); }
