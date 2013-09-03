#if !defined(__TRANSFORM_NODE__)
#define __TRANSFORM_NODE__

#include <scenegraph/scene.h>
#include <scenegraph/scenenode.h>
#include <glm/glm.hpp>

struct TransformNode : public SceneNode
{
  glm::mat4 transform;

  TransformNode();
  TransformNode(const glm::mat4& transform);

  virtual bool PreRender(Scene* const scene);
  virtual void PostRender(Scene* const scene);
};

#endif // __TRANSFORM_NODE__
