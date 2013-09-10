#if ! defined(__FULL_SCREEN_QUAD__)
#define __FULL_SCREEN_QUAD__

#include <device.h>
#include <vertexarray.h>
#include <boost/shared_ptr.hpp>
#include <buffers/vertexbuffer.h>

// Renders a texture-mapped full-screen quad using whatever render state is currently active.
class FullScreenQuad
{
public:
  FullScreenQuad();
  ~FullScreenQuad();

  void Load();
  void Draw(Device* const device);

private:
  boost::shared_ptr<VertexArray> vertexArray;
  boost::shared_ptr<VertexBuffer> vb;
};

#endif // __FULL_SCREEN_QUAD__
