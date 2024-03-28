#include "SceneBase.h"

#include "../sync/Sync.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "SceneAssets.h"

enum Cameras {
  CAM_follow,
  CAM_houses,
  CAM_power_station,
  CAM_power_station2,
  CAM_bridge,
  CAM_eggton,
  CAM_sausage,
  CAM_station,
  CAM_crossing,
  CAM_sofa,
};

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
  {{-80.f,0.f,-60.f}, {-10.f,0.f,0.f}},
  {{-150.f,0.f,-60.f}, {-1.f,0.f,0.f}},
  {{-160.f,-9.5f,-60.f}, {-1.f,0.f,0.f}},
  {{-240.f,-9.5f,-60.f}, {-10.f,0.f,0.f}}
};

static const glm::vec3 RunnyEggs[] = {
  {-1.0f, 0.f, -4.f},
  { 1.2f, 0.f, -2.5f},
  {-0.3f, 0.f, -1.7f},
  { 0.8f, 0.f, 0.2f},
  {-1.2f, 0.f, 2.3f},
  { 0.6f, 0.f, 4.f}
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
    auto const rot90 = glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));

    instances.emplace_back(ID_train);
    instances.emplace_back(ID_train);

    for(int i = 0; i < 8; i++) {
      instances.emplace_back(ID_cloud);
    }

    for(int i = 0; i < 6; i++) {
      instances.emplace_back(ID_egg);
    }

    instances.emplace_back(ID_egg_walking_sausage);

    for(int i = 0; i < 4; i++) {
      instances.emplace_back(ID_lampshade);
      instances.back().transform = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(10.f + i * 15.f, 0.f, -75.f)), glm::vec3(5.f)) * rot90;
    }

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

    instances.emplace_back(ID_house1);
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(60.f, 0.f, -5.f));

    instances.emplace_back(ID_house1);
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(67.f, 0.8f, 5.f));

    instances.emplace_back(ID_house1);
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(74.f, 2.2f, 15.f));

    instances.emplace_back(ID_building);
    instances.back().transform = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(12.f, 0.f, -94.f)),
      glm::vec3(-1.f, 1.f, -1.f));

    instances.emplace_back(ID_duvet_hills);

    instances.emplace_back(ID_house1);
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(-68.f, 0.f, -92.f));

    instances.emplace_back(ID_house1);
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(-66.f, 0.f, -80.f));

    instances.emplace_back(ID_station_building);
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(-80.f, 0.f, -70.f)) * rot90;

    auto const eggmat = glm::scale(glm::transpose(rot90), glm::vec3(0.75f));

    instances.emplace_back(ID_egg);
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(-88.f, 0.f, -64.f)) * eggmat;

    instances.emplace_back(ID_egg);
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(-85.f, 0.f, -65.f)) * eggmat;

    instances.emplace_back(ID_sofa);
    instances.back().transform = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(-200.f, -30.f, -78.f)), glm::vec3(8.f));

    instances.emplace_back(ID_house2);
    instances.back().transform = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(-173.f, -9.5f, -73.f)), glm::vec3(1.f)) * rot90;

    instances.emplace_back(ID_house2);
    instances.back().transform = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(-184.f, -6.f, -71.f)), glm::vec3(1.f)) * rot90;

    instances.emplace_back(ID_house2);
    instances.back().transform = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(-198.f, -6.f, -72.f)), glm::vec3(1.f)) * rot90;

    instances.emplace_back(ID_house2);
    instances.back().transform = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(-218.f, -9.5f, -69.f)), glm::vec3(1.f)) * rot90;

    instances.emplace_back(ID_viaduct);
    instances.back().transform = glm::rotate(
      glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(-26.f, 0.f, -60.f)), glm::vec3(3.f)),
      glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));

    instances.emplace_back(ID_tablecloth_land);
    instances.back().transform = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(-98.f, 0.f, -70.f)), glm::vec3(0.89f));

    //instances.emplace_back(ID_skybox);
  }

  void update(double time) final {
    // Update transforms and whatever else needs updating
    //updateCamera();
    auto const beattime = float(time) * (100.f/60.f);
    auto const beat = std::floor(beattime);
    auto const frac = beattime - beat;

    auto const bounce = 1.f - 4.f * (frac - 0.5f) * (frac - 0.5f);
    auto const sway = (frac * frac - 0.5f) * ((unsigned(beat) & 1) ? 1.f : -1.f);

    float t = std::min(float(time)*0.1f, float(std::size(track) - 1));
    auto const pos = spline(track, t);
    auto const dir = splineDir(track, t);

    instances[0].transform = glm::translate(glm::mat4(1.f), pos + glm::vec3(0.f, bounce, 0.f)) *
        glm::transpose(glm::lookAt(glm::vec3(0.f), -dir, glm::vec3(0.f, 1.f, 0.f))) *
        glm::rotate(glm::mat4(1.f), sway * 0.2f, glm::vec3(0.f, 0.f, 1.f));

    auto const couple = 10.0f / glm::length(dir);
    auto const pos2 = spline(track, t - couple);
    auto const dir2 = splineDir(track, t - couple);
    instances[1].transform = glm::translate(glm::mat4(1.f), pos2 + glm::vec3(0.f, bounce, 0.f)) *
        glm::transpose(glm::lookAt(glm::vec3(0.f), dir2, glm::vec3(0.f, 1.f, 0.f))) *
        glm::rotate(glm::mat4(1.f), sway * -0.2f, glm::vec3(0.f, 0.f, 1.f));

    cameraOptions = glm::vec4(46.f, 4.f, 0.4f, 1.5f);
    processOptions = glm::vec4(1.f, 1.0f, 0.0f, 0.0f);

    auto const cam = CAM_follow;
    switch (cam) {
      default: {
        cameraProjection = glm::perspective(glm::radians(45.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(pos.x, 20.0f, pos.z + 40.0), pos,
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_houses: {
        cameraProjection = glm::perspective(glm::radians(45.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(27.f, 12.0f, 15.f), glm::vec3(67.f, 0.8f, 3.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_power_station: {
        float const t = time;
        cameraProjection = glm::perspective(glm::radians(45.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(40.f - t, 0.2f, -62.f), glm::vec3(50.f - t, 8.f, -75.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_power_station2: {
        cameraProjection = glm::perspective(glm::radians(20.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(-80.f, 16.0f, -60.f), glm::vec3(50.f, 0.5f, -80.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_bridge: {
        cameraProjection = glm::perspective(glm::radians(20.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(-26.f, 0.0f, 50.f), glm::vec3(-26.f, 0.0f, -80.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_eggton: {
        auto const t = std::min(float(time), 20.f);
        cameraProjection = glm::perspective(glm::radians(45.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(-90.f, 8.f, -75.f), glm::vec3(-80.f, 1.f, -95.f + t),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_sausage: {
        auto const t = std::min(float(time), 20.f);
        cameraProjection = glm::perspective(glm::radians(20.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(-90.f, 8.f, -75.f), glm::vec3(-75.f, 1.f, -70.f - t),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_station: {
        auto const p = (pos + pos2)*.5f;
        cameraProjection = glm::perspective(glm::radians(45.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          p + glm::vec3(0.f, 15.f, 20.f), p + glm::vec3(0.f, 0.f, -20.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_crossing: {
        cameraProjection = glm::perspective(glm::radians(60.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          pos + glm::vec3(0.f, 2.f, -2.f), pos + glm::vec3(1.f, 2.f, -4.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_sofa: {
        cameraProjection = glm::perspective(glm::radians(45.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(-200.f, -6.f, 0.f), glm::vec3(-200.f, -6.f, -78.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;
    }

    // Cooling tower clouds
    for(int i = 0; i < 8; i++) {
      auto const beattime = float(time) * (25.f/60.f) + float(i)*.25f;
      auto const a = std::sqrt(beattime - std::floor(beattime) + float(i >> 2));
      auto const y = a * 12.f + 7.f;
      auto const s = a * 3.0f + 2.0f;
      instances[i + 2].transform = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(10.f + (i & 3) * 15.f, y, -75.f)), glm::vec3(s));
    }

    // Runny eggs
    {
      auto const t = std::min(float(time), 20.f);
      auto const rot90 = glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
      auto const eggmat = glm::scale(glm::transpose(rot90), glm::vec3(0.75f));
      for(int i = 0; i < 6; i++) {
        auto const beattime = (float(time) * 10.f + float(i)) / 3.f;
        auto const beat = std::floor(beattime);
        auto const frac = beattime - beat;

        auto const bounce = 1.f - 4.f * (frac - 0.5f) * (frac - 0.5f);
        auto const sway = (frac * frac - 0.5f) * ((unsigned(beat) & 1) ? 1.f : -1.f);

        auto const eggPos = RunnyEggs[i] + glm::vec3(-80.f, bounce * .25f, -95.f + t);
        instances[i + 10].transform = glm::translate(glm::mat4(1.f), eggPos) *
          glm::rotate(glm::mat4(1.f), sway * -0.2f, glm::vec3(0.f, 0.f, 1.f)) * eggmat;
      }

      auto const beattime = float(time) * (10.f / 3.f);
      auto const beat = std::floor(beattime);
      auto const frac = beattime - beat;
      auto const bounce = 1.f - 4.f * (frac - 0.5f) * (frac - 0.5f);

      auto const eggPos = glm::vec3(-75.f, 0.f, -70.f - t);
      auto const sausage = float(time) * ((5.f * glm::pi<float>()) / 3.f);
      instances[16].transform = glm::translate(glm::mat4(1.f), eggPos) *
          glm::rotate(glm::mat4(1.f), std::sin(sausage) * 0.3f, glm::vec3(0.f, 1.f, 0.f)) *
          glm::rotate(glm::mat4(1.f), bounce * 0.4f, glm::vec3(1.f, 0.f, 0.f)) *
          glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -1.f)) *
          glm::scale(rot90, glm::vec3(0.75f));
    }
  }

  void teardown() final {
    // nothing to do
  }
};
