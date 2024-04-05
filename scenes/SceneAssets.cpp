#include "SceneAssets.h"

// Getter for the asset store
static SceneAssets *globalAssetStore = nullptr;
SceneAssets *getSceneAssets() {
  if (globalAssetStore == nullptr) {
    globalAssetStore = new SceneAssets();
  }
  return globalAssetStore;
}

void destroySceneAssets() {
  if (globalAssetStore != nullptr) {
    delete globalAssetStore;
    globalAssetStore = nullptr;
  }
}
