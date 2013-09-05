#include <debug.h>
#include <scenegraph/scene.h>
#include <scenegraph/scenenode.h>
#include <boost/foreach.hpp>

void SceneNode::Load(Scene* const scene)
{
  BOOST_FOREACH(boost::shared_ptr<SceneNode> child, children)
  {
    child->Load(scene);
  }
}

void SceneNode::Update(Scene* const scene, float elapsedMillisconds)
{
  BOOST_FOREACH(boost::shared_ptr<SceneNode> child, children)
  {
    child->Update(scene, elapsedMillisconds);
  }
}

bool SceneNode::PreRender(Scene* const scene)
{
  return true;
}

void SceneNode::Render(Scene* const scene)
{
  BOOST_FOREACH(boost::shared_ptr<SceneNode> child, children)
  {
    if (child->PreRender(scene))
    {
      child->Render(scene);
      child->PostRender(scene);
    }
  }
}

void SceneNode::PostRender(Scene* const scene)
{
}


void SceneNode::LogChildren(size_t indent)
{
  LOG("%*.c%s\n", indent*2, ' ', TypeName());
  BOOST_FOREACH(boost::shared_ptr<SceneNode> child, children)
  {
    child->LogChildren(indent+1);
  }
}
