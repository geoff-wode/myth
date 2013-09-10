#if ! defined(__VERTEX_BUFFER__)
#define __VERTEX_BUFFER__

#include <gl_loader/gl_loader.h>
#include <vertexdeclaration.h>

class VertexBuffer
{
public:
  const size_t VertexCount;

  VertexBuffer(const VertexDeclaration* const vertexDecl, size_t vertexCount, GLenum usage);
  ~VertexBuffer();

  void SetData(const void* const data, size_t vertexCount, size_t startVertex);
  void Bind();
  void Unbind();

private:
  GLuint handle;
  size_t vertexStride;
};

#endif // __VERTEX_BUFFER__
