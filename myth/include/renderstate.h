#if ! defined(__RENDER_STATE__)
#define __RENDER_STATE__

#include <vertexarray.h>
#include <textureunit.h>
#include <glm/glm.hpp>
#include <gl_loader/gl_loader.h>
#include <boost/shared_ptr.hpp>

struct DepthTest
{
  DepthTest() : enabled(true), function(GL_LESS) { }
  bool  enabled;
  GLenum function;
};

struct CullTest
{
  CullTest() : enabled(false), cullFace(GL_BACK), frontFace(GL_CCW) { }
  bool  enabled;
  GLenum cullFace;
  GLenum frontFace;
};

struct Blending
{
  Blending()
    : enabled(true), source(GL_SRC_ALPHA), destination(GL_ONE_MINUS_SRC_ALPHA)
  { }
  bool enabled;
  GLenum source;
  GLenum destination;
};

// Describes a complete rendering state.
struct RenderState
{
  RenderState()
    : colourMask(true), depthMask(true)
  {
  }

  // Whether to allow changes to colour buffer channels.
  glm::bvec4 colourMask;

  // Whether to allow changes to the depth buffer value.
  bool depthMask;

  Blending  blending;
  DepthTest depthTest;
  CullTest  cullTest;

  boost::shared_ptr<VertexArray> vertexArray;

  static const size_t MaxTextureUnits = 4;
  TextureUnit textureUnits[MaxTextureUnits];
};

#endif // __RENDER_STATE__
