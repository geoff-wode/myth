#include <scenegraph/scene.h>
#include <scenegraph/transformnode.h>
#include <device.h>
#include <debug.h>

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
  scene->device->shaderVars.WorldMatrix = transform;
  return true;
}

void TransformNode::PostRender(Scene* const scene)
{
  scene->matrixStack.pop();
  scene->device->shaderVars.WorldMatrix = scene->matrixStack.top();
}
