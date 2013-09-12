#include <myth.h>
#include <effect.h>
#include <glm/ext.hpp>
#include <vertexdeclaration.h>
#include <boost/make_shared.hpp>

//-----------------------------------------------------------

class SimplestEffect : public Effect
{
public:
  SimplestEffect() { }

  virtual bool Load(const char* const name)
  {
    if (Effect::Load(name))
    {
      WorldViewProjectionMatrix = GetEffectParameter("WorldViewProjectionMatrix");
    }
    return false;
  }

  EffectParameter* WorldViewProjectionMatrix;
};

class TexturedEffect : public Effect
{
public:
  TexturedEffect() { }

  virtual bool Load(const char* const name)
  {
    if (Effect::Load(name))
    {
      Sampler = GetEffectParameter("sampler");
      SamplerRepeat = GetEffectParameter("samplerRepeat");
      WorldViewProjectionMatrix = GetEffectParameter("WorldViewProjectionMatrix");
    }
    return false;
  }

  EffectParameter* Sampler;
  EffectParameter* SamplerRepeat;
  EffectParameter* WorldViewProjectionMatrix;
};

//-----------------------------------------------------------

struct Vertex
{
  glm::vec3 position;
};

//-----------------------------------------------------------

static const VertexDeclaration vertexDecl =
{
  sizeof(Vertex),
  {
    VertexAttribute("Position", GL_FLOAT, 3, offsetof(Vertex, position))
  }
};

//-----------------------------------------------------------

static boost::shared_ptr<SimplestEffect> simplestEffect;
static boost::shared_ptr<TexturedEffect> texturedEffect;

//-----------------------------------------------------------

Myth::Myth(const char* const windowTitle)
  : Program(windowTitle)
{
}

Myth::~Myth()
{
}

//-----------------------------------------------------------
void Myth::Initialise()
{
  Device.Initialise(false, 1280, 720);

  Program::Initialise();
}

//-----------------------------------------------------------
void Myth::LoadContent()
{
  static const struct ModelSetup
  {
    const char* const filename;
    const glm::mat4 transform;
  } modelSetup[] =
  {
    { "assets\\models\\tree1_obj\\Tree1.obj", glm::mat4() }
  };
  static const size_t modelCount = sizeof(modelSetup) / sizeof(modelSetup[0]);

  for (size_t i = 0; i < modelCount; ++i)
  {
    boost::shared_ptr<Model> model(new Model());
    model->Load(modelSetup[i].filename);
    
    models.push_back(model);
    transforms.push_back(modelSetup[i].transform);
  }

  simplestEffect = boost::make_shared<SimplestEffect>();
  simplestEffect->Load("assets/shaders/simplest");

  texturedEffect = boost::make_shared<TexturedEffect>();
  texturedEffect->Load("assets/shaders/textured");
  texturedEffect->Sampler->Set(0);

  Program::LoadContent();
}

//-----------------------------------------------------------
void Myth::Update(float elapsedMS)
{
  Program::Update(elapsedMS);
}

//-----------------------------------------------------------
void Myth::Render(float elapsedMS)
{
  Device.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, clearState);

  const glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0,2,8), glm::vec3(0,5,0), glm::vec3(0,1,0));
  const glm::mat4 projectionMatrix = glm::perspective(45.0f, Device.GetWindow()->AspectRatio, 0.1f, 1000.0f);

  texturedEffect->Use();
  for (size_t i = 0; i < models.size(); ++i)
  {
    texturedEffect->WorldViewProjectionMatrix->Set(projectionMatrix * viewMatrix * transforms[i]);

    models[i]->Draw(&Device);
  }

  Program::Render(elapsedMS);
}
