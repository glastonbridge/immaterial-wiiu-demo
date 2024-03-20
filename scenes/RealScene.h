#include "SceneBase.h"

#include "../sync/Sync.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "../renderer/RenderObject.h"

#include "../graphics/SceneObject.h"

struct RealScene: public SceneBase {
  enum objectID : size_t {
    ID_train,
    //ID_cushion,
    //ID_house1,
    //ID_lampshade,
  };

  void setup() final {
    objects.push_back(LoadObject("assets/train.fbx", NULL));
    //objects.push_back(LoadObject("assets/cushion.fbx", NULL));
    //objects.push_back(LoadObject("assets/house1.fbx", NULL));
    //objects.push_back(LoadObject("assets/lampshade.fbx", NULL));

    instances.emplace_back(ID_train);
    instances.emplace_back(ID_train);
    instances.emplace_back(ID_train);
  }

  void update(double time) final {
    updateCamera();

    auto rotY = syncVal("TestPart:Object:RotY");
    instances[1].transform = glm::rotate(glm::mat4(1.f), glm::radians(rotY), glm::vec3(0.f, 1.f, 0.f));
    instances[1].anim = syncVal("TestPart:Object:Frame");

    instances[2].transform = glm::rotate(glm::mat4(1.f), glm::radians(rotY), glm::vec3(1.f, 0.f, 0.f));
  }

  void teardown() final {
    // nothing to do
  }
};
