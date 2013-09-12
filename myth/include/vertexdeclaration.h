#if ! defined(__VERTEX_DECLARATION__)
#define __VERTEX_DECLARATION__

#include <vector>
#include <gl_loader/gl_loader.h>

struct VertexAttribute
{
  VertexAttribute() : name(NULL), type(0), elementCount(0), offset(0) { }

  VertexAttribute(const char* const name, GLenum type, size_t elementCount, size_t offset)
    : name(name), type(type), elementCount(elementCount), offset(offset)
  {
  }

  const char* const name;
  const GLenum type;
  const size_t elementCount;
  const size_t offset;
};

struct VertexDeclaration
{
  const size_t vertexStride;
  static const size_t MaxAttributes = 16;
  const VertexAttribute attributes[MaxAttributes];
};

#endif // __VERTEX_DECLARATION__
