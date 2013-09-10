#include <buffers/indexbuffer.h>

static size_t SizeOfElement(GLenum type)
{
  switch (type)
  {
  case GL_UNSIGNED_BYTE: return sizeof(unsigned char);
  case GL_UNSIGNED_SHORT: return sizeof(unsigned short);
  case GL_UNSIGNED_INT: return sizeof(unsigned int);
  default: break;
  }
  return 0;
}

IndexBuffer::IndexBuffer(GLenum indexType, size_t indexCount, GLenum usage)
  : IndexType(indexType), elementSize(SizeOfElement(indexType))
{
  glGenBuffers(1, &handle);
  Bind();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * elementSize, NULL, usage);
  Unbind();
}

IndexBuffer::~IndexBuffer()
{
  glDeleteBuffers(1, &handle);
}

void IndexBuffer::SetData(const void* const data, size_t indexCount, size_t startIndex)
{
  Bind();
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, startIndex * elementSize, indexCount * elementSize, data);
  Unbind();
}

void IndexBuffer::Bind()
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
}

void IndexBuffer::Unbind()
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
