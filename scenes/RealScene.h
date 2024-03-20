#include "SceneBase.h"

#include "../sync/Sync.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "../renderer/RenderObject.h"

#include "../graphics/SceneObject.h"
#include "../graphics/MaterialCollection.h"

struct RealScene: public SceneBase {
  enum objectID : size_t {
    ID_train,
    //ID_cushion,
    ID_house1,
    ID_lampshade,
  };

  enum materialID : size_t {
    ID_train_mat,
    //ID_cushion_mat,
    ID_house1_mat,
    ID_lampshade_mat,
  };

  void setup() final {
    materials.push_back(std::make_unique<BoneMaterial>("assets/train_small.png"));
    //materials.push_back(std::make_unique<BoneMaterial>("assets/cushion.png"));  // Cushion needs triangulation, will crash if loaded
    materials.push_back(std::make_unique<BoneMaterial>("assets/house1_small.png"));
    materials.push_back(std::make_unique<BoneMaterial>("assets/lampshade_small.png"));

    objects.push_back(LoadObject("assets/train.fbx", NULL, materials[ID_train_mat].get()));
    //objects.push_back(LoadObject("assets/cushion.fbx", NULL, materials[ID_cushion_mat].get())); // Cushion needs triangulation, will crash if loaded
    objects.push_back(LoadObject("assets/house1.fbx", NULL, materials[ID_house1_mat].get()));
    objects.push_back(LoadObject("assets/lampshade.fbx", NULL, materials[ID_lampshade_mat].get()));

    instances.emplace_back(ID_train);
    instances.emplace_back(ID_house1);
    instances.emplace_back(ID_train);
    instances.emplace_back(ID_lampshade);
  }

  void update(double time) final {
    updateCamera();

    auto rotY = syncVal("TestPart:Object:RotY");
    instances[1].transform = glm::rotate(glm::mat4(1.f), glm::radians(rotY), glm::vec3(0.f, 1.f, 0.f));
    instances[1].anim = syncVal("TestPart:Object:Frame");

    rotY = syncVal("TestPart:Object2:RotY");
    instances[2].transform = glm::rotate(glm::mat4(1.f), glm::radians(rotY), glm::vec3(0.f, 1.f, 0.f));
    instances[2].anim = syncVal("TestPart:Object:Frame");

    rotY = syncVal("TestPart:Object2:ShiftX");
    instances[3].transform = glm::translate(glm::mat4(1.f), glm::vec3(rotY, 0.f, 0.f));
    instances[3].anim = syncVal("TestPart:Object:Frame");
  }

  void teardown() final {
    // nothing to do
  }
};
