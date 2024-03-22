#include "../renderer/RenderObject.h"
#include "../graphics/SceneObject.h"
#include "../graphics/MaterialCollection.h"

enum objectID : size_t {
    ID_train,
    ID_cushion,
    ID_house1,
    ID_lampshade,
};

enum materialID : size_t {
    ID_train_mat,
    ID_cushion_mat,
    ID_house1_mat,
    ID_lampshade_mat,
};

struct SceneAssets {
    std::vector<std::unique_ptr<SceneObject>> objects;
    std::vector<std::unique_ptr<SceneMaterial>> materials;

    SceneAssets();
};

SceneAssets* getSceneAssets();