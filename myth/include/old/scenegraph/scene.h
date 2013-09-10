#if ! defined(__SCENE__)
#define __SCENE__

#include <device.h>
#include <stack>
#include <glm/glm.hpp>
#include <boost/shared_ptr.hpp>

// Defines a complete 'scene' containing objects of various sorts.
struct Scene
{
  std::stack<glm::mat4> matrixStack;
  boost::shared_ptr<struct SceneNode> root;

  boost::shared_ptr<Device> device;

  Scene();
  Scene(boost::shared_ptr<Device> device);

  void Load();
  void Update(float elapsedMilliseconds);
  void Render();
};

#endif // __SCENE__
