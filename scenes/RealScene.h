#include "SceneBase.h"

#include "../sync/Sync.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "SceneAssets.h"

struct SplineSegment {
  glm::vec3 pos;
  glm::vec3 dir;
};

glm::vec3 spline(SplineSegment const *seg, float t) {
  t = std::max(t, 0.0f);
  auto f = std::floor(t);
  auto h = size_t(f);
  auto g = t - f;

  auto const &seg0 = seg[h];
  auto const &seg1 = seg[h+1];
  auto const x0 = seg0.pos;
  auto const x1 = seg0.pos + seg0.dir;
  auto const x2 = seg1.pos - seg1.dir;
  auto const x3 = seg1.pos;
  float ig = 1.0f - g;
  float ig2 = ig * ig;
  float ig3 = ig2 * ig;

  return ig3 * x0 + (g * 3.f) * (ig2 * x1 + ig * g * x2) + (g * g * g) * x3;
}

glm::vec3 splineDir(SplineSegment const *seg, float t) {
  t = std::max(t, 0.0f);
  auto f = std::floor(t);
  auto h = size_t(f);
  auto g = t - f;

  auto const &seg0 = seg[h];
  auto const &seg1 = seg[h+1];
  auto const x0 = seg0.pos;
  auto const x1 = seg0.pos + seg0.dir;
  auto const x2 = seg1.pos - seg1.dir;
  auto const x3 = seg1.pos;
  float ig = g - 1.f;

  float k0 = -3.f * (ig * ig);
  float k1 = 3.f * ig * (3.f * g - 1.f);
  float k2 = (6.f - 9.f * g) * g;

  return k0 * x0 + k1 * x1 + k2 * x2 + (3.f * g * g) * x3;
}

static const SplineSegment track[] = {
  {{0.f,0.f,60.f}, {10.f,0.f,0.f}},
  {{60.f,0.f,40.f}, {0.f,0.f,-10.f}},
  {{40.f,0.f,0.f}, {-10.f,0.f,0.f}},
  {{20.f,0.f,-10.f}, {0.f,0.f,-10.f}},
  {{50.f,0.f,-40.f}, {0.f,0.f,-15.f}},
  {{20.f,0.f,-60.f}, {-10.f,0.f,0.f}},
  {{0.f,0.f,-60.f}, {-10.f,0.f,0.f}}
};

struct RealScene: public SceneBase {
  static glm::mat4 getCushionMat(glm::vec3 p, float ry = 0.0f, float rx = 0.0f, float rz = 0.0f) {
    return glm::scale(glm::translate(glm::mat4(1.f), p), glm::vec3(5.0f))
      * glm::rotate(glm::mat4(1.f), ry, glm::vec3(0.f, 1.f, 0.f))
      * glm::rotate(glm::mat4(1.f), rx, glm::vec3(1.f, 0.f, 0.f))
      * glm::rotate(glm::mat4(1.f), rz, glm::vec3(0.f, 0.f, 1.f));
  }

  void setup() final {
    // Set up the scene
    instances.emplace_back(ID_skybox);
    instances.emplace_back(ID_train);

    instances.emplace_back(ID_cushion);
    instances.back().transform = getCushionMat(glm::vec3(10.0f, 0.f, 70.f));

    instances.emplace_back(ID_cushion);
    instances.back().transform = getCushionMat(glm::vec3(40.0f, 0.f, 70.f), glm::radians(35.f));

    instances.emplace_back(ID_cushion);
    instances.back().transform = getCushionMat(glm::vec3(75.0f, 0.f, 40.f), glm::radians(83.f));

    instances.emplace_back(ID_cushion);
    instances.back().transform = getCushionMat(glm::vec3(76.0f, 6.f, 40.f), glm::radians(81.f));

    instances.emplace_back(ID_cushion);
    instances.back().transform = getCushionMat(glm::vec3(30.0f, 0.f, 35.f), glm::radians(12.f));

    instances.emplace_back(ID_cushion);
    instances.back().transform = getCushionMat(glm::vec3(24.0f, 0.f, 18.f), glm::radians(14.f));

    instances.emplace_back(ID_cushion);
    instances.back().transform = getCushionMat(glm::vec3(6.0f, 0.f, -15.f), glm::radians(102.f));

    instances.emplace_back(ID_cushion);
    instances.back().transform = getCushionMat(glm::vec3(26.0f, 0.f, -40.f), glm::radians(17.f));

    instances.emplace_back(ID_cushion);
    instances.back().transform = getCushionMat(glm::vec3(56.0f, 0.f, -18.f), glm::radians(17.f));

    //instances.emplace_back(ID_house1);
    //instances.emplace_back(ID_train);
    //for(int i = 0; i < 10; i++) {
    //  instances.emplace_back(ID_lampshade);
    //}
  }

  void update(double time) final {
    // Update transforms and whatever else needs updating
    //updateCamera();
    float t = std::min(float(time)*0.1f, float(std::size(track) - 1));
    auto const pos = spline(track, t);
    auto const dir = splineDir(track, t);

    cameraProjection = glm::perspective(glm::radians(syncVal("Camera:FoV")), 1920.0f/1080.0f, 0.1f, 2000.f);
    cameraView = glm::lookAt(
      glm::vec3(pos.x, 20.0f, pos.z + 10.0), pos,
      glm::vec3(0.0f, 1.0f, 0.0f)
    );

    instances[0].transform = glm::mat4(1.0f);

    instances[1].transform = glm::translate(glm::mat4(1.f), pos) *
        glm::transpose(glm::lookAt(glm::vec3(0.f), dir, glm::vec3(0.f, 1.f, 0.f)));
    //instances[1].transform = glm::lookAt(pos, dir, glm::vec3(0.f, 1.f, 0.f));
    //instances[2].transform = glm::rotate(glm::mat4(1.0f), glm::radians(float(time)*60.0f), glm::vec3(0.f, 1.f, 0.f));
    // Scale cushion down
    // instances[13].transform = glm::scale(glm::mat4(1.f), glm::vec3(0.01f, 0.01f, 0.01f));
  
  }

  void teardown() final {
    // nothing to do
  }
};
