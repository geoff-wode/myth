#if ! defined(__UNIFORM_BUFFER__)
#define __UNIFORM_BUFFER__

#include <gl_loader/gl_loader.h>

class UniformBuffer
{
public:
  const size_t SizeInBytes;

  UniformBuffer(unsigned int binding, size_t sizeInBytes, GLenum usage);
  ~UniformBuffer();

  void SetData(const void* const data, size_t sizeInBytes, size_t offsetInBytes);

  void Bind();
  void Unbind();

private:
  GLuint handle;
};

#endif // __UNIFORM_BUFFER__
