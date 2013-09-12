#if ! defined(__FULL_SCREEN_QUAD__)
#define __FULL_SCREEN_QUAD__

#include <device.h>
#include <vertexarray.h>
#include <modelmesh.h>
#include <boost/shared_ptr.hpp>
#include <buffers/vertexbuffer.h>

// Renders a texture-mapped full-screen quad using whatever render state is currently active.
class FullScreenQuad : public ModelMesh
{
public:
  FullScreenQuad();
  virtual ~FullScreenQuad();

  void Load();
};

#endif // __FULL_SCREEN_QUAD__
