#include <buffers/uniformbuffer.h>

UniformBuffer::UniformBuffer(size_t sizeInBytes, unsigned int binding)
  : SizeInBytes(sizeInBytes)
{
  glGenBuffers(1, &handle);
  Bind();
  glBindBufferBase(GL_UNIFORM_BUFFER, binding, handle);
  glBufferData(GL_UNIFORM_BUFFER, SizeInBytes, NULL, GL_DYNAMIC_DRAW);
  Unbind();
}

UniformBuffer::~UniformBuffer()
{
  glDeleteBuffers(1, &handle);
}

void UniformBuffer::SetData(const void* const data, size_t sizeInBytes, size_t offsetInBytes)
{
  Bind();
  glBufferSubData(GL_UNIFORM_BUFFER, offsetInBytes, sizeInBytes, data);
  Unbind();
}

void UniformBuffer::Bind()
{
  glBindBuffer(GL_UNIFORM_BUFFER, handle);
}

void UniformBuffer::Unbind()
{
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
