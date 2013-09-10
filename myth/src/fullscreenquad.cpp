
#include <glm/glm.hpp>
#include <gl_loader/gl_loader.h>
#include <vertexdeclaration.h>
#include <fullscreenquad.h>
#include <boost/make_shared.hpp>

static struct Vertex
{
  glm::vec3 position;
  glm::vec2 textureCoord;
} vertices[] =
{
  { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0,0) },
  { glm::vec3( 1.0f, -1.0f, 0.0f), glm::vec2(1,0) },
  { glm::vec3( 1.0f,  1.0f, 0.0f), glm::vec2(1,1) },

  { glm::vec3( 1.0f,  1.0f, 0.0f), glm::vec2(1,1) },
  { glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec2(1,0) },
  { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0,0) }
};
static const size_t VertexCount = sizeof(vertices) / sizeof(vertices[0]);

static VertexDeclaration vDecl =
{
  sizeof(Vertex),
  2,
  {
    VertexAttribute("Position", GL_FLOAT, 3, offsetof(Vertex, position)),
    VertexAttribute("TextureCoord0", GL_FLOAT, 2, offsetof(Vertex, textureCoord))
  }
};

FullScreenQuad::FullScreenQuad()
{
}

FullScreenQuad::~FullScreenQuad()
{
}

void FullScreenQuad::Load()
{
  vb = boost::make_shared<VertexBuffer>(&vDecl, VertexCount, GL_STATIC_DRAW);
  vb->SetData(vertices, VertexCount, 0);

  vertexArray = boost::make_shared<VertexArray>();
  vertexArray->Bind();
  vb->Bind();
  vertexArray->Configure(&vDecl);
  vertexArray->Unbind();

  vb->Unbind();
}

void FullScreenQuad::Draw(Device* const device)
{
  device->DrawArray(GL_TRIANGLES, 2, 0, vertexArray);
}
