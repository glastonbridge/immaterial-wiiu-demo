#include "SceneBase.h"

#include "../sync/Sync.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "SceneAssets.h"

struct DevroomScene : public SceneBase {
  void setup() final {
    // Scene that shows off all the assets in a square grid
    for (int i = 0; i < _ID_ASSETS_MAX; i++) {
      instances.emplace_back((objectID)i);
    }
  }

  void update(double time) final {
    // Update transforms and whatever else needs updating
    updateCamera();

    // Set up transforms to put objects in a grid
    // Also rotate them about the Y axis, and set animation frame
    float rotVal = time;
    float frameVal = time * 0.3f;
    float gridSpacing = 10.0f;
    int gridSize = int(sqrtf((float)_ID_ASSETS_MAX) + 1);
    for (int i = 0; i < _ID_ASSETS_MAX; i++) {
      int x = i / gridSize;
      int z = i % gridSize;
      instances[i].transform = glm::translate(
          glm::mat4(1.f),
          glm::vec3(x * gridSpacing - (gridSize / 2.0) * gridSpacing, 0.f,
                    z * gridSpacing - (gridSize / 2.0) * gridSpacing));
      instances[i].transform =
          glm::rotate(instances[i].transform, rotVal, glm::vec3(0.f, 1.f, 0.f));
      instances[i].anim = frameVal;
    }
  }

  void teardown() final {
    // nothing to do
  }
};
