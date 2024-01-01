#include <vector>

struct Object;

struct SceneBase {
  virtual void setup()=0;
  virtual void update(double time)=0;
  std::vector<Object*> objects;
};
