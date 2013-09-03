#include <scenegraph/scene.h>
#include <scenegraph/transformnode.h>
#include <device.h>

TransformNode::TransformNode()
  : transform(glm::mat4())
{
}

TransformNode::TransformNode(const glm::mat4& transform)
  : transform(transform)
{
}

bool TransformNode::PreRender(Scene* const scene)
{
  scene->matrixStack.push(transform * scene->matrixStack.top());
  Device::SetWorldMatrix(transform);
  return true;
}

void TransformNode::PostRender(Scene* const scene)
{
  scene->matrixStack.pop();
  Device::SetWorldMatrix(scene->matrixStack.top());
}
