#if ! defined(__WOODEN_CRATE__)
#define __WOODEN_CRATE__

#include <device.h>
#include <asset.h>
#include <scenegraph/scene.h>
#include <scenegraph/scenenode.h>
#include <boost/shared_ptr.hpp>

struct AssetNode : public SceneNode
{
  AssetNode(boost::shared_ptr<Asset> asset);

  const boost::shared_ptr<Asset> asset;

  virtual void Render(Scene* const scene);
};

#endif // __WOODEN_CRATE__
