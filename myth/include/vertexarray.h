#if ! defined(__VERTEX_ARRAY__)
#define __VERTEX_ARRAY__

#include <vertexdeclaration.h>
#include <gl_loader/gl_loader.h>

class VertexArray
{
public:
  VertexArray();
  ~VertexArray();

  void Bind();
  void Configure(const VertexDeclaration* const vertexDecl);
  void Unbind();

private:
  GLuint handle;
};

#endif // __VERTEX_ARRAY__
