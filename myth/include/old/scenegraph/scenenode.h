#if ! defined(__SCENE_NODE__)
#define __SCENE_NODE__

#include <list>
#include <boost/shared_ptr.hpp>

struct Scene;

// Declares a generic node in scene.
struct SceneNode
{
  std::list<boost::shared_ptr<SceneNode>> children;

  virtual void Load(Scene* const scene);
  virtual void Update(Scene* const scene, float elapsedMillisconds);
  virtual bool PreRender(Scene* const scene);
  virtual void Render(Scene* const scene);
  virtual void PostRender(Scene* const scene);

  virtual const char* TypeName() const { return "SceneNode"; }

  void LogChildren(size_t indent);
};


#endif // __SCENE_NODE__
