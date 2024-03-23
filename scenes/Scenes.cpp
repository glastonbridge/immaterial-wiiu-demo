#include "SceneBase.h"
#include "RealScene.h"
#include "GizmosScene.h"

enum sceneID : int { // int because we convert this from a float from the sync system and unsigned is dangerous
    SCENE_RealScene,
    SCENE_GizmosScene,
    _MAX_SCENES
};

SceneBase* getScene(int id) {
    sceneID id_conv = (sceneID)std::max(0, std::min(id, (int)_MAX_SCENES - 1));
    switch (id_conv) {
        case SCENE_RealScene:
            return new RealScene();
        case SCENE_GizmosScene:
            return new GizmosScene();
    }

    // should be unreachable
    return nullptr;
}