#include <asset.h>
#include <debug.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <gl_loader/gl_loader.h>
#include <world/hellocrates.h>
#include <boost/make_shared.hpp>
#include <scenegraph/assetnode.h>
#include <scenegraph/transformnode.h>

//-----------------------------------------------------

static boost::shared_ptr<Asset> MakeCrateAsset();

//-----------------------------------------------------

struct SpinnerNode : public TransformNode
{
  const glm::vec3 axis;
  const float revsPerSecond;
  float angle;

  SpinnerNode(const glm::vec3& axis, float revsPerSecond)
    : axis(axis), revsPerSecond(revsPerSecond), angle(0)
  {
  }

  virtual void Update(Scene* const scene, float elapsedMS)
  {
    angle += revsPerSecond * (elapsedMS / 1000.0f);

    transform = glm::rotate(glm::mat4(1), angle, axis);

    SceneNode::Update(scene, elapsedMS);
  }

  virtual const char* TypeName() const { return "SpinnerNode"; }
};

//-----------------------------------------------------
boost::shared_ptr<SceneNode> HelloCrates::Create()
{
  boost::shared_ptr<AssetNode> crate(new AssetNode(MakeCrateAsset()));
  boost::shared_ptr<SceneNode> root(new SceneNode());

  // Build up the letter 'i' from 2 parts - the dot and the main body...
  {
    boost::shared_ptr<TransformNode> t(new TransformNode(glm::rotate(glm::mat4(), 45.0f, glm::vec3(1,0,1))));
    boost::shared_ptr<SpinnerNode> dot(new SpinnerNode(glm::vec3(0,1,0), 90.0f));
    dot->children.push_back(crate);
    t->children.push_back(dot);
    root->children.push_back(t);

    boost::shared_ptr<TransformNode> i(new TransformNode(glm::translate(0.0f, -4.0f, 0.0f) * glm::scale(1.0f, 2.0f, 1.0f)));
    i->children.push_back(crate);
    root->children.push_back(i);
  }

  // Build up the letter 'H' from 3 parts (left, right, bar)...
  {
    boost::shared_ptr<TransformNode> left(new TransformNode(glm::translate(-8.0f, 0.0f, 0.0f) * glm::scale(1.0f, 6.0f, 1.0f)));
    left->children.push_back(crate);
    root->children.push_back(left);


    boost::shared_ptr<TransformNode> right(new TransformNode(glm::translate(-4.0f, 0.0f, 0.0f) * glm::scale(1.0f, 6.0f, 1.0f)));
    right->children.push_back(crate);
    root->children.push_back(right);

    boost::shared_ptr<TransformNode> bar(new TransformNode(glm::translate(-6.0f, 0.0f, 0.0f) * glm::scale(2.0f, 1.0f, 0.8f)));
    bar->children.push_back(crate);
    root->children.push_back(bar);
  }

  return root;
}

//-----------------------------------------------------
static boost::shared_ptr<Asset> MakeCrateAsset()
{
  static struct Vertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoord;
  } vertices[] =
  {
    // bottom
    { glm::vec3(-1.0f,-1.0f,-1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
    { glm::vec3( 1.0f,-1.0f,-1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
    { glm::vec3(-1.0f,-1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
    { glm::vec3( 1.0f,-1.0f,-1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
    { glm::vec3( 1.0f,-1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
    { glm::vec3(-1.0f,-1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },

    // top
    { glm::vec3(-1.0f, 1.0f,-1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
    { glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
    { glm::vec3( 1.0f, 1.0f,-1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
    { glm::vec3( 1.0f, 1.0f,-1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
    { glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
    { glm::vec3( 1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },

    // front
    { glm::vec3(-1.0f,-1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) },
    { glm::vec3( 1.0f,-1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
    { glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f) },
    { glm::vec3( 1.0f,-1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
    { glm::vec3( 1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
    { glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f) },

    // back
    { glm::vec3(-1.0f,-1.0f,-1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
    { glm::vec3(-1.0f, 1.0f,-1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f) },
    { glm::vec3( 1.0f,-1.0f,-1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
    { glm::vec3( 1.0f,-1.0f,-1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
    { glm::vec3(-1.0f, 1.0f,-1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f) },
    { glm::vec3( 1.0f, 1.0f,-1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },

    // left
    { glm::vec3(-1.0f,-1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
    { glm::vec3(-1.0f, 1.0f,-1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
    { glm::vec3(-1.0f,-1.0f,-1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
    { glm::vec3(-1.0f,-1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
    { glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
    { glm::vec3(-1.0f, 1.0f,-1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },

    // right
    { glm::vec3( 1.0f,-1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
    { glm::vec3( 1.0f,-1.0f,-1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
    { glm::vec3( 1.0f, 1.0f,-1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
    { glm::vec3( 1.0f,-1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
    { glm::vec3( 1.0f, 1.0f,-1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
    { glm::vec3( 1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) }
  };

  boost::shared_ptr<Asset> crate = boost::make_shared<Asset>();

  crate->drawType = GL_TRIANGLES;
  crate->drawFirstVertex = 0;
  crate->drawVertexCount = sizeof(vertices) / sizeof(vertices[0]);

  glGenVertexArrays(1, &crate->vao);
  glBindVertexArray(crate->vao);

  crate->vertexBuffer = boost::make_shared<VertexBuffer>(sizeof(vertices), GL_STATIC_DRAW);
  crate->vertexBuffer->Enable();
  crate->vertexBuffer->SetData(vertices, sizeof(vertices), 0);

  crate->shader = boost::make_shared<Shader>("assets\\shaders\\directionallight");
  crate->shader->SetUniform("sampler", 0);
  
  crate->shader->SetUniform("material.emissive", glm::vec3(0.0f));
  crate->shader->SetUniform("material.ambient", glm::vec3(1.0f));
  crate->shader->SetUniform("material.diffuse", glm::vec3(1.0f));
  crate->shader->SetUniform("material.specular", glm::vec3(1.0f));
  crate->shader->SetUniform("material.shininess", 60.0f);

  crate->texture = boost::make_shared<Texture2D>("assets\\images\\wooden-crate.jpg");
  crate->texture->Load();

  struct VertexAttribute
  {
    GLint index;
    GLint componentCount;
    GLenum compoentType;
    size_t offset;
  } attributes[] =
  {
    { crate->shader->GetAttributeIndex("Position"), 3, GL_FLOAT, offsetof(Vertex, position) },
    { crate->shader->GetAttributeIndex("Normal"), 3, GL_FLOAT, offsetof(Vertex, normal) },
    { crate->shader->GetAttributeIndex("TextureCoord"), 2, GL_FLOAT, offsetof(Vertex, textureCoord) }
  };
  const size_t attributeCount = sizeof(attributes) / sizeof(attributes[0]);

  for (size_t i = 0; i < attributeCount; ++i)
  {
    glEnableVertexAttribArray(attributes[i].index);
    glVertexAttribPointer(
      attributes[i].index,
      attributes[i].componentCount, attributes[i].compoentType,
      GL_FALSE,
      sizeof(Vertex),
      (const void*)attributes[i].offset);
  }

  VertexBuffer::Disable();
  glBindVertexArray(0);

  return crate;
}
