#include <vertexarray.h>
#include <effect.h>

VertexArray::VertexArray()
{
  glGenVertexArrays(1, &handle);
}

VertexArray::~VertexArray()
{
  glDeleteVertexArrays(1, &handle);
}

void VertexArray::Bind()
{
  glBindVertexArray(handle);
}

void VertexArray::Configure(const VertexDeclaration* const vertexDecl)
{
  for (size_t i = 0; i < vertexDecl->attributeCount; ++i)
  {
    glEnableVertexAttribArray(Effect::GetAttributeIndex(vertexDecl->attributes[i].name));
    glVertexAttribPointer(
      Effect::GetAttributeIndex(vertexDecl->attributes[i].name),
      vertexDecl->attributes[i].elementCount,
      vertexDecl->attributes[i].type,
      GL_FALSE,
      vertexDecl->vertexStride,
      (const void*)vertexDecl->attributes[i].offset);
  }
}

void VertexArray::Unbind()
{
  glBindVertexArray(0);
}
