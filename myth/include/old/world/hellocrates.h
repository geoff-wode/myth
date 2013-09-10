#if ! defined(__HELLO_CRATE__)
#define __HELLO_CRATE__

#include <scenegraph/scenenode.h>
#include <boost/shared_ptr.hpp>

namespace HelloCrates
{
  boost::shared_ptr<SceneNode> Create();
}

#endif // __HELLO_CRATE__
