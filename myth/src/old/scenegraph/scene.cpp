#include <scenegraph/scene.h>
#include <scenegraph/scenenode.h>
#include <debug.h>

Scene::Scene()
{
}

Scene::Scene(boost::shared_ptr<Device> device)
  : device(device)
{
}

void Scene::Load()
{
  root->Load(this);
  //root->LogChildren(0);
}

void Scene::Update(float elapsedMilliseconds)
{
  root->Update(this, elapsedMilliseconds);
}

void Scene::Render()
{
  // Initialise the matrix stack with the identity matrix...
  matrixStack.push(glm::mat4());

  if (root->PreRender(this))
  {
    root->Render(this);
    root->PostRender(this);
  }
}
