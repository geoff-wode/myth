#if ! defined(__TEXTURE_UNIT__)
#define __TEXTURE_UNIT__

#include <gl_loader/gl_loader.h>
#include <boost/shared_ptr.hpp>
#include <texture.h>

// Associates a texture with a given texture unit slot on the GPU.
struct TextureUnit
{
  TextureUnit()
    : active(false), texture(NULL)
  {
  }

  bool active;
  boost::shared_ptr<Texture2D> texture;
};

#endif // __TEXTURE_UNIT__
