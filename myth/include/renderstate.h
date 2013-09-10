#if ! defined(__RENDER_STATE__)
#define __RENDER_STATE__

#include <vertexarray.h>
#include <glm/glm.hpp>
#include <gl_loader/gl_loader.h>
#include <boost/shared_ptr.hpp>
//#include <textureunit.h>

struct DepthTest
{
  DepthTest() : enabled(true), function(GL_LESS) { }
  bool  enabled;
  GLenum function;
};

struct CullTest
{
  CullTest() : enabled(true), cullFace(GL_BACK), frontFace(GL_CCW) { }
  bool  enabled;
  GLenum cullFace;
  GLenum frontFace;
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

  DepthTest depthTest;
  CullTest  cullTest;

  boost::shared_ptr<VertexArray> vertexArray;

  GLenum indexType; // for indexed drawing

  //static const size_t MaxTextureUnits = 4;
  //TextureUnit textureUnits[MaxTextureUnits];
};

#endif // __RENDER_STATE__
