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
  CAM_platform,
  CAM_follow_bus,
  CAM_power_station3,
  CAM_titles,
};

struct SplineSegment {
  glm::vec3 pos;
  glm::vec3 dir;
};

glm::vec3 spline(SplineSegment const *seg, float t) {
  if (t <= 0.f) {
    return seg->pos;
  }

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
  if (t <= 0.f) {
    return 3.f * seg->dir;
  }

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
  {{-146.f,0.f,-60.f}, {-4.f,0.f,0.f}},
  {{-156.f,-9.5f,-60.f}, {-2.f,0.f,0.f}},
  {{-220.f,-9.5f,-60.f}, {-10.f,0.f,0.f}},
  {{-220.f,-9.5f,-79.f}, {10.f,0.f,0.f}},
  {{-176.f,-4.5f,-79.f}, {8.f,0.f,0.f}},
  {{-156.f,-9.5f,-70.f}, {2.f,0.f,0.f}},
  {{-146.f,0.f,-70.f}, {4.f,0.f,0.f}},
  {{-80.f,0.f,-60.f}, {10.f,0.f,0.f}},
  {{20.f,0.f,-60.f}, {20.f,0.f,0.f}},
  {{50.f,0.f,-40.f}, {0.f,0.f,15.f}},
  {{20.f,0.f,-10.f}, {0.f,0.f,10.f}},
  {{40.f,0.f,0.f}, {10.f,0.f,0.f}},
  {{60.f,0.f,40.f}, {0.f,0.f,10.f}},
  {{0.f,0.f,60.f}, {-10.f,0.f,0.f}}
};

static const SplineSegment busRoute[] = {
  {{-55.f, 0.f, -88.f}, {-4.f, 0.f, 0.f}},
  {{-80.f, 0.f, -88.f}, {-6.f, 0.f, 0.f}},
  {{-90.f, 0.f, -100.f}, {-4.f, 0.f, 2.f}},
  {{-102.f, 0.f, -80.f}, {0.5f, 0.f, 4.f}},
  {{-100.f, 0.f, -69.f}, {0.f, 0.f, 2.f}},
  {{-120.f, 0.f, 0.f}, {0.f, 0.f, 12.f}},
  {{-105.f, 0.f, 40.f}, {8.f, 0.f, 0.f}},
  {{-55.f, 0.f, 30.f}, {10.f, 0.f, 0.f}}
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

    auto const eggmat = glm::scale(glm::transpose(rot90), glm::vec3(0.75f));

    instances.emplace_back(ID_egg);
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(-88.f, 0.f, -64.f)) * eggmat;

    instances.emplace_back(ID_egg);
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(-85.f, 0.f, -65.f)) * eggmat;

    instances.emplace_back(ID_egg_carton);
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(-110.f, 0.f, -70.f)) * eggmat;

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
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(-68.f, 0.f, -96.f));

    instances.emplace_back(ID_house1);
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(-66.f, 0.f, -80.f));

    instances.emplace_back(ID_station_building);
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(-80.f, 0.f, -70.f)) * rot90;

    instances.emplace_back(ID_sofa);
    instances.back().transform = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(-196.f, -30.f, -78.f)), glm::vec3(8.f));

    instances.emplace_back(ID_carpet);
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(-196.f, -30.f, -60.f));

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

    instances.emplace_back(ID_viaduct);
    instances.back().transform = glm::rotate(
      glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(-26.f, -18.f, -60.f)), glm::vec3(3.f)),
      glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));

    instances.emplace_back(ID_carpet);
    instances.back().transform = glm::translate(glm::mat4(1.f), glm::vec3(-26.f, -30.f, -100.f));

    instances.emplace_back(ID_tablecloth_land);
    instances.back().transform = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(-95.f, 0.f, -40.f)), glm::vec3(0.89f));

    instances.emplace_back(ID_cardboard_cloud);
    instances.back().transform = glm::rotate(glm::scale(
        glm::translate(glm::mat4(1.f),
            glm::vec3(65.f, 43.f, 48.f)),
            glm::vec3(1.f)),
            glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));

    instances.emplace_back(ID_cardboard_cloud);
    instances.back().transform = glm::rotate(glm::scale(
        glm::translate(glm::mat4(1.f),
            glm::vec3(70.f, 26.f, 72.f)),
            glm::vec3(1.f)),
            glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));

    instances.emplace_back(ID_string_slipstream);
    instances.back().transform = glm::rotate(glm::scale(
        glm::translate(glm::mat4(1.f),
            glm::vec3(50.f, 44.f, 40.f)),
            glm::vec3(0.2f)),
            glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));

    instances.emplace_back(ID_string_svatg);
    instances.back().transform = glm::rotate(glm::scale(
        glm::translate(glm::mat4(1.f),
            glm::vec3(50.f, 39.f, 46.f)),
            glm::vec3(0.2f)),
            glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));

    instances.emplace_back(ID_string_presents);
    instances.back().transform = glm::rotate(glm::scale(
        glm::translate(glm::mat4(1.f),
            glm::vec3(50.f, 35.f, 50.f)),
            glm::vec3(0.14f)),
            glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));

    instances.emplace_back(ID_string_title);
    instances.back().transform = glm::rotate(glm::scale(
        glm::translate(glm::mat4(1.f),
            glm::vec3(60.f, 20.f, 42.f)),
            glm::vec3(0.2f)),
            glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));

    instances.emplace_back(ID_string_beanbag);
    instances.back().transform = glm::rotate(glm::scale(
        glm::translate(glm::mat4(1.f),
            glm::vec3(-217.f, -26.f, -23.f)),
            glm::vec3(0.19f)),
            glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));

    instances.emplace_back(ID_string_aldroid);
    instances.back().transform = glm::rotate(glm::scale(
        glm::translate(glm::mat4(1.f),
            glm::vec3(-209.f, -26.f, -17.f)),
            glm::vec3(0.19f)),
            glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));

    instances.emplace_back(ID_string_vurpo);
    instances.back().transform = glm::scale(
        glm::translate(glm::mat4(1.f),
            glm::vec3(-95.f, 2.f, 30.f)),
            glm::vec3(0.19f));

    instances.emplace_back(ID_string_halcy);
    instances.back().transform = glm::scale(
        glm::translate(glm::mat4(1.f),
            glm::vec3(-45.f, -26.f, -50.f)),
            glm::vec3(0.34f));

    instances.emplace_back(ID_string_violet);
    instances.back().transform = glm::scale(
        glm::translate(glm::mat4(1.f),
            glm::vec3(11.f, 40.f, -75.f)),
            glm::vec3(0.4f));

    // Temporary: Sleepytime duvet
    //instances.emplace_back(ID_eepytime);
  }

  void update(double time) final {
    // Update transforms and whatever else needs updating
    //updateCamera();
    auto const beattime = float(time) * (100.f/60.f);
    auto const beat = std::floor(beattime);
    auto const frac = beattime - beat;

    auto const bamp = syncVal("Train:Bounce");
    auto const bounce = bamp * (1.f - 4.f * (frac - 0.5f) * (frac - 0.5f));
    auto const sway = bamp * (frac * frac - 0.5f) * ((unsigned(beat) & 1) ? 0.2f : -0.2f);

    auto const t1 = std::min(syncVal("Train:Car1"), float(std::size(track) - 1));
    auto const pos = spline(track, t1);
    auto const dir = splineDir(track, t1);

    instances[0].transform = glm::translate(glm::mat4(1.f), pos + glm::vec3(0.f, bounce, 0.f)) *
        glm::transpose(glm::lookAt(glm::vec3(0.f), -dir, glm::vec3(0.f, 1.f, 0.f))) *
        glm::rotate(glm::mat4(1.f), sway, glm::vec3(0.f, 0.f, 1.f));

    auto const couple = std::min(1.f, 0.5f / glm::length(dir));
    auto t2 = t1 - couple;
    for (int i = 0; i < 18; ++i) {
      auto const tdir = splineDir(track, t2);
      auto const couple = std::min(1.f, 0.5f / glm::length(tdir));
      t2 -= couple;
    }

    auto const pos2 = spline(track, t2);
    auto const dir2 = splineDir(track, t2);
    instances[1].transform = glm::translate(glm::mat4(1.f), pos2 + glm::vec3(0.f, bounce, 0.f)) *
        glm::transpose(glm::lookAt(glm::vec3(0.f), dir2, glm::vec3(0.f, 1.f, 0.f))) *
        glm::rotate(glm::mat4(1.f), -sway, glm::vec3(0.f, 0.f, 1.f));

    cameraOptions = glm::vec4(syncVal("Camera:FocalDist"),
                              syncVal("Camera:FocalLen"),
                              syncVal("Camera:Aperture"),
                              syncVal("Global:FresnelPow"));
    processOptions = glm::vec4(syncVal("Global:Vignette"), syncVal("Global:Fade"), 0.0f, 0.0f);

    auto const pEggRun = glm::vec3(-80.f, 0.f, -95.f + syncVal("Eggton:RunnyEggs"));
    auto const pSausage = glm::vec3(-75.f, 0.f, -110.f + syncVal("Eggton:Sausage"));

    // Cooling tower clouds
    for(int i = 0; i < 8; i++) {
      auto const beattime = float(time) * (25.f/60.f) + float(i)*.25f;
      auto const a = std::sqrt(beattime - std::floor(beattime) + float(i >> 2));
      auto const y = a * 12.f + 7.f;
      auto const s = a * 3.0f + 2.0f;
      instances[i + 2].transform = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(10.f + (i & 3) * 15.f, y, -75.f)), glm::vec3(s));
    }

    glm::vec3 busPos;
    // Runny eggs
    {
      auto const t = syncVal("Eggton:RunnyEggs");
      auto const rot90 = glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
      auto const eggmat = glm::scale(glm::transpose(rot90), glm::vec3(0.75f));
      for(int i = 0; i < 6; i++) {
        auto const beattime = (float(time) * 10.f + float(i)) / 3.f;
        auto const beat = std::floor(beattime);
        auto const frac = beattime - beat;

        auto const bounce = 1.f - 4.f * (frac - 0.5f) * (frac - 0.5f);
        auto const sway = (frac * frac - 0.5f) * ((unsigned(beat) & 1) ? 1.f : -1.f);

        auto const eggPos = RunnyEggs[i] + glm::vec3(0.f, bounce * .25f, 0.f) + pEggRun;
        instances[i + 10].transform = glm::translate(glm::mat4(1.f), eggPos) *
          glm::rotate(glm::mat4(1.f), sway * -0.2f, glm::vec3(0.f, 0.f, 1.f)) * eggmat;
      }

      // Egg walking sausage
      auto const beattime = float(time) * (10.f / 3.f);
      auto const beat = std::floor(beattime);
      auto const frac = beattime - beat;
      auto const bounce = 1.f - 4.f * (frac - 0.5f) * (frac - 0.5f);

      auto const sausage = float(time) * ((5.f * glm::pi<float>()) / 3.f);
      instances[16].transform = glm::translate(glm::mat4(1.f), pSausage) *
          glm::rotate(glm::mat4(1.f), std::sin(sausage) * 0.3f, glm::vec3(0.f, 1.f, 0.f)) *
          glm::rotate(glm::mat4(1.f), bounce * -0.4f, glm::vec3(1.f, 0.f, 0.f)) *
          glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 1.f)) * eggmat;

      // Station eggs
      instances[17].transform = glm::translate(glm::mat4(1.f), glm::vec3(-88.f, 0.f, -64.f)) *
              glm::transpose(glm::lookAt(glm::vec3(0.f), glm::vec3(-88.f, 0.f, -64.f) - pos, glm::vec3(0.f, 1.f, 0.f))) * eggmat;
      instances[18].transform = glm::translate(glm::mat4(1.f), glm::vec3(-85.f, 0.f, -65.f)) *
              glm::transpose(glm::lookAt(glm::vec3(0.f), glm::vec3(-85.f, 0.f, -65.f) - pos, glm::vec3(0.f, 1.f, 0.f))) * eggmat;

      // Egg bus
      auto const t1 = std::min(syncVal("Eggton:Bus"), float(std::size(busRoute) - 1));
      auto const pos = spline(busRoute, t1);
      auto const dir = splineDir(busRoute, t1);

      instances[19].transform = glm::translate(glm::mat4(1.f), pos) *
          glm::transpose(glm::lookAt(glm::vec3(0.f), -dir, glm::vec3(0.f, 1.f, 0.f))) * eggmat;

      busPos = pos;
    }


    // Temporary: Sleepytimt duvez
    //instances.back().transform = glm::scale(instances[0].transform, glm::vec3(5.0f));
    //instances.back().anim = float(time) * 24.0f;

    auto const cam = unsigned(syncVal("Scene:CamID"));
    switch (cam) {
      default: {
        auto const view = glm::vec3(syncVal("Camera:Pos:X"),
                                    syncVal("Camera:Pos:Y"),
                                    syncVal("Camera:Pos:Z"));
        cameraProjection = glm::perspective(glm::radians(45.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(pos + view), pos + glm::vec3(0.f, 1.f, 0.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_houses: {
        cameraProjection = glm::perspective(glm::radians(45.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(27.f, 12.0f, 15.f), glm::vec3(67.f, 0.8f, 3.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_power_station: {
        float const t = syncVal("Scene:CamParam");
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
        auto const lookdown = syncVal("Scene:CamParam");
        cameraProjection = glm::perspective(glm::radians(20.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(-26.f, 0.0f, 50.f), glm::vec3(-26.f, -lookdown, -80.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_eggton: {
        cameraProjection = glm::perspective(glm::radians(45.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(-90.f, 8.f, -75.f), pEggRun,
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_sausage: {
        cameraProjection = glm::perspective(glm::radians(syncVal("Scene:CamParam")), 1920.0f/1080.0f, 0.1f, 2000.f);

        auto const weight = syncVal("Scene:LookSausage");
        auto const target = (1.f - weight) * glm::vec3(-70.f, 2.f, -88.f) +
            weight * (pSausage + glm::vec3(0.f, 1.f, 1.f));
        cameraView = glm::lookAt(
          glm::vec3(-90.f, 8.f, -86.f), target,
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_station: {
        auto const p = (pos + pos2)*.5f;
        cameraProjection = glm::perspective(glm::radians(45.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          p + glm::vec3(0.f, 8.f, 20.f), p + glm::vec3(0.f, 0.f, -20.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_crossing: {
        cameraProjection = glm::perspective(glm::radians(60.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          pos + glm::vec3(0.f, 2.f, -2.f), pos + glm::vec3(1.f, 2.f, -4.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_sofa: {
        auto const lookdown = syncVal("Scene:CamParam");
        cameraProjection = glm::perspective(glm::radians(45.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(-196.f, -6.f, 0.f), glm::vec3(-196.f, -6.f - lookdown, -78.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_platform: {
        cameraProjection = glm::perspective(glm::radians(60.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(-74.f, 1.5f, -64.f), glm::vec3(-80.f, 1.5f, -62.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_follow_bus: {
        auto const view = glm::vec3(syncVal("Camera:Pos:X"),
                                    syncVal("Camera:Pos:Y"),
                                    syncVal("Camera:Pos:Z"));
        cameraProjection = glm::perspective(glm::radians(45.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(busPos + view), busPos,
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_power_station3: {
        auto const lookup = syncVal("Scene:CamParam");
        cameraProjection = glm::perspective(glm::radians(20.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(40.f, 3.0f, 0.f), glm::vec3(32.5f, 2.f + lookup, -75.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;

      case CAM_titles: {
        auto const lookdown = syncVal("Scene:CamParam");
        cameraProjection = glm::perspective(glm::radians(20.f), 1920.0f/1080.0f, 0.1f, 2000.f);
        cameraView = glm::lookAt(
          glm::vec3(-40.f, 40.f, 60.f), glm::vec3(40.f - lookdown, 40.f - lookdown, 60.f),
          glm::vec3(0.0f, 1.0f, 0.0f));
      } break;
    }
  }

  void teardown() final {
    // nothing to do
  }
};
