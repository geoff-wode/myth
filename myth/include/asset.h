#if ! defined(__ASSET__)
#define __ASSET__

#include <buffers.h>
#include <shader.h>
#include <sampler.h>
#include <texture.h>
#include <glm/glm.hpp>
#include <boost/shared_ptr.hpp>
#include <gl_loader/gl_loader.h>

struct Asset
{
  GLuint vao;
  GLenum drawType;
  GLint drawFirstVertex;
  GLint drawVertexCount;
  boost::shared_ptr<Shader> shader;
  boost::shared_ptr<Sampler2D> sampler;
  boost::shared_ptr<Texture2D> texture;
  boost::shared_ptr<VertexBuffer> vertexBuffer;
};

#endif // __ASSET__
