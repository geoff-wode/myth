#include <scenegraph/scene.h>
#include <scenegraph/scenenode.h>

Scene::Scene()
{
}

void Scene::Load()
{
  root->Load(this);
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
