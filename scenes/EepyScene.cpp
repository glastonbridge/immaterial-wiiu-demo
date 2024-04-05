#include "SceneBase.h"

#include "../sync/Sync.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "SceneAssets.h"
#include "Spline.h"

namespace {
static const SplineSegment eepyTrack[] = {
  {{30.f, 0.f, 17.f}, {0.f, 0.f, -10.f}},
  {{20.f, 0.f, 1.5f}, {-4.f, 0.f, 0.f}},
  {{12.f, 0.f, 1.5f}, {-4.f, 0.f, 0.f}},
  {{-10.f, -3.f, -2.5f}, {-6.f, 0.f, -1.f}}
};
}

void EepyScene::setup() {
    auto const rot90 = glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));

    instances.emplace_back(ID_train);
    instances.emplace_back(ID_train);
    instances.emplace_back(ID_eepytime);
    instances.back().transform = glm::scale(glm::mat4(1.f), glm::vec3(8.f));

    instances.emplace_back(ID_mattress);
    instances.emplace_back(ID_pillow);
    instances.back().transform = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(20.f, 0.f, 10.f)), glm::vec3(2.5f)) * rot90;

    instances.emplace_back(ID_pillow);
    instances.back().transform = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(20.f, 0.f, -7.f)), glm::vec3(2.5f)) * rot90;
}

void EepyScene::update(double time) {
    auto const beattime = float(time) * (100.f/60.f);
    auto const beat = std::floor(beattime);
    auto const frac = beattime - beat;

    auto const bamp = syncVal("Train:Bounce");
    auto const bounce = bamp * (1.f - 4.f * (frac - 0.5f) * (frac - 0.5f));
    auto const sway = bamp * (frac * frac - 0.5f) * ((unsigned(beat) & 1) ? 0.2f : -0.2f);

    auto const t1 = std::min(syncVal("Train:Car1"), float(std::size(eepyTrack) - 1));
    auto const pos = spline(eepyTrack, t1);
    auto const dir = splineDir(eepyTrack, t1);

    instances[0].transform = glm::translate(glm::mat4(1.f), pos + glm::vec3(0.f, bounce, 0.f)) *
        glm::transpose(glm::lookAt(glm::vec3(0.f), -dir, glm::vec3(0.f, 1.f, 0.f))) *
        glm::rotate(glm::mat4(1.f), sway, glm::vec3(0.f, 0.f, 1.f));

    auto const couple = std::min(1.f, 0.5f / glm::length(dir));
    auto t2 = t1 - couple;
    for (int i = 0; i < 18; ++i) {
      auto const tdir = splineDir(eepyTrack, t2);
      auto const couple = std::min(1.f, 0.5f / glm::length(tdir));
      t2 -= couple;
    }

    auto const pos2 = spline(eepyTrack, t2);
    auto const dir2 = splineDir(eepyTrack, t2);
    instances[1].transform = glm::translate(glm::mat4(1.f), pos2 + glm::vec3(0.f, bounce, 0.f)) *
        glm::transpose(glm::lookAt(glm::vec3(0.f), dir2, glm::vec3(0.f, 1.f, 0.f))) *
        glm::rotate(glm::mat4(1.f), -sway, glm::vec3(0.f, 0.f, 1.f));

    instances[2].anim = syncVal("Eepy:Duvet");
    cameraOptions = glm::vec4(syncVal("Camera:FocalDist"),
                              syncVal("Camera:FocalLen"),
                              0.f,
                              syncVal("Global:FresnelPow"));
    processOptions = glm::vec4(syncVal("Global:Vignette"), syncVal("Global:Fade"), 0.0f, 0.0f);
    cameraProjection = glm::perspective(glm::radians(45.f), 1920.0f/1080.0f, 0.1f, 2000.f);
    cameraView = glm::lookAt(
        glm::vec3(24.f, 8.f, -6.f), glm::vec3(15.f, 0.f, 3.5f),
        glm::vec3(0.0f, 1.0f, 0.0f));
}

void EepyScene::teardown() {}
