#if ! defined(__INDEX_BUFFER__)
#define __INDEX_BUFFER__

#include <gl_loader/gl_loader.h>

class IndexBuffer
{
public:
  IndexBuffer(GLenum indexType, size_t indexCount, GLenum usage);
  ~IndexBuffer();

  const GLenum IndexType;

  void SetData(const void* const data, size_t indexCount, size_t startIndex);
  void Bind();
  void Unbind();

private:
  GLuint handle;
  size_t elementSize;
};

#endif // __INDEX_BUFFER__
