#include "SceneBase.h"

#include "../sync/Sync.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "SceneAssets.h"

struct RealScene: public SceneBase {
  void setup() final {
    // Set up the scene
    instances.emplace_back(ID_train);
    instances.emplace_back(ID_house1);
    instances.emplace_back(ID_train);
    for(int i = 0; i < 10; i++) {
      instances.emplace_back(ID_lampshade);
    }
    instances.emplace_back(ID_cushion);
    instances.emplace_back(ID_skybox);
  }

  void update(double time) final {
    // Update transforms and whatever else needs updating
    updateCamera();

    // just a bunch of random stuff to test the sync system
    auto rotY = syncVal("TestPart:Object:RotY");
    instances[1].transform = glm::rotate(glm::mat4(1.f), glm::radians(rotY), glm::vec3(0.f, 1.f, 0.f));
    instances[1].anim = syncVal("TestPart:Object:Frame");

    rotY = syncVal("TestPart:Object2:RotY");
    instances[2].transform = glm::rotate(glm::mat4(1.f), glm::radians(rotY), glm::vec3(0.f, 1.f, 0.f));
    instances[2].anim = syncVal("TestPart:Object:Frame");

    rotY = syncVal("TestPart:Object2:ShiftX");
    instances[3].transform = glm::translate(glm::mat4(1.f), glm::vec3(rotY, 0.f, 0.f));
    instances[3].anim = syncVal("TestPart:Object:Frame");

    for(int i = 0; i < 10; i++) {
      rotY = syncVal("TestPart:Object2:RotY");
      instances[4 + i].transform = glm::rotate(glm::mat4(1.f), glm::radians(rotY * i * 0.2f), glm::vec3(1.f, 1.f, 0.f));
      instances[4 + i].anim = syncVal("TestPart:Object:Frame");
    }
    // Scale cushion down
    // instances[13].transform = glm::scale(glm::mat4(1.f), glm::vec3(0.01f, 0.01f, 0.01f));
  
  }

  void teardown() final {
    // nothing to do
  }
};
