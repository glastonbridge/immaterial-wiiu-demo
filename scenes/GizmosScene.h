#include "SceneBase.h"

#include "../sync/Sync.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "SceneAssets.h"

struct GizmosScene : public SceneBase {
  void setup() final {
    // Set up the scene
    instances.emplace_back(ID_gizmos);
  }

  void update(double time) final {
    // Update transforms and whatever else needs updating
    updateCamera();

    // just a bunch of random stuff to test the sync system
    auto rotY = syncVal("GizmosPart:Gizmos:RotY");
    auto scale = syncVal("GizmosPart:Gizmos:Scale");
    instances[0].transform =
        glm::scale(glm::rotate(glm::mat4(1.f), glm::radians(rotY),
                               glm::vec3(0.f, 1.f, 0.f)),
                   glm::vec3(scale, scale, scale));
    instances[0].anim = syncVal("GizmosPart:Gizmos:Frame");
  }

  void teardown() final {
    // nothing to do
  }
};
