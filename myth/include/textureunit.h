#if ! defined(__TEXTURE_UNIT__)
#define __TEXTURE_UNIT__

#include <gl_loader/gl_loader.h>
#include <boost/shared_ptr.hpp>
#include <sampler.h>
#include <texture.h>

// Associates a sampler and texture combination with a given texture unit slot on the GPU.
struct TextureUnit
{
  TextureUnit()
    : active(false), sampler(NULL), texture(NULL)
  {
  }

  bool operator==(const TextureUnit& other)
  {
    return (active == other.active) && (sampler == other.sampler) && (texture == other.texture);
  }

  bool operator!=(const TextureUnit& other)
  {
    return (active != other.active) && (sampler != other.sampler) && (texture != other.texture);
  }

  bool active;
  boost::shared_ptr<Sampler2D> sampler;
  boost::shared_ptr<Texture2D> texture;
};

#endif // __TEXTURE_UNIT__
