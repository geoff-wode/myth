#include <buffers/vertexbuffer.h>


VertexBuffer::VertexBuffer(const VertexDeclaration* const vertexDecl, size_t vertexCount, GLenum usage)
  : VertexCount(vertexCount), vertexStride(vertexDecl->vertexStride)
{
  glGenBuffers(1, &handle);
  Bind();
  glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexStride, NULL, usage);
  Unbind();
}

VertexBuffer::~VertexBuffer()
{
  glDeleteBuffers(1, &handle);
}

void VertexBuffer::SetData(const void* const data, size_t vertexCount, size_t startVertex)
{
  Bind();
  glBufferSubData(GL_ARRAY_BUFFER, startVertex * vertexStride, vertexCount * vertexStride, data);
  Unbind();
}

void VertexBuffer::Bind()
{
  glBindBuffer(GL_ARRAY_BUFFER, handle);
}

void VertexBuffer::Unbind()
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
