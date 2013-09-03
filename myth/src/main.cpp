#include <SDL.h>
#include <SOIL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gl_loader/gl_loader.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

#include <debug.h>
#include <shader.h>
#include <buffers.h>
#include <texture.h>
#include <camera.h>
#include <sampler.h>
#include <textureunit.h>
#include <renderstate.h>
#include <clearstate.h>
#include <device.h>

//-----------------------------------------------------

struct Asset
{
  GLuint vao;
  GLenum drawType;
  GLint drawFirstVertex;
  GLint drawVertexCount;
  boost::shared_ptr<Shader> shader;
  boost::shared_ptr<Sampler2D> sampler;
  boost::shared_ptr<Texture2D> texture;
  boost::shared_ptr<VertexBuffer> vertexBuffer;
};

struct AssetInstance
{
  glm::mat4 transform;
  boost::shared_ptr<Asset> asset;
};

//-----------------------------------------------------

FILE* debug::logFile;

static boost::shared_ptr<Camera> camera;

static float rotationOfDot = 0.0f;
static float rotationOfLight = 0.0f;
static Light light(true, glm::vec4(-3.0f, 0.0f, 3.0f, 1.0f));

static boost::shared_ptr<Asset> woodenCrate;
static std::list<AssetInstance> instances;

//-----------------------------------------------------

static void Init(const std::string& title, int width, int height, bool fullScreen);
static boost::shared_ptr<Asset> MakeCrate();
static void CreateInstances(std::list<AssetInstance>& instances);
static void Update(unsigned int elapsedMS);
static void Render();

//-----------------------------------------------------

int main(int argc, char* argv[])
{
	debug::logFile = fopen("stderr.txt", "wb");

  if (!Device::Initialise("Myth", 1280, 720, false))
  {
    exit(EXIT_FAILURE);
  }

  const float aspectRatio((float)1280/(float)720);
  camera = boost::make_shared<Camera>(45.0f, aspectRatio, 0.1f, 100.0f);
  camera->Position = glm::vec3(-15,2,15);
  camera->Yaw = 40.0f;
  camera->Pitch = 5.0f;

  woodenCrate = MakeCrate();
  CreateInstances(instances);

  Device::SetLight(0, light);

  bool quit = false;
  unsigned int prevTime = 0;

  while (!quit)
  {
    {
      SDL_Event event;
      while (SDL_PollEvent(&event))
      {
        switch (event.type)
        {
        case SDL_QUIT: quit = true; break;
        case SDL_KEYDOWN:
          {
            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) { quit = true; }
          }
          break;
        default: break;
        }
      }
    }

    unsigned int now = SDL_GetTicks();
    if (prevTime == 0) { prevTime = now; }
    unsigned int elapsedMS = now - prevTime;
    prevTime = now;

    Update(elapsedMS);
    Render();
  }

  return 0;
}

//-----------------------------------------------------
static boost::shared_ptr<Asset> MakeCrate()
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

  crate->shader = boost::make_shared<Shader>("shaders/phong");
  crate->shader->SetUniform("sampler", 0);
  
  crate->shader->SetUniform("material.emissive", glm::vec3(0.0f));
  crate->shader->SetUniform("material.ambient", glm::vec3(1.0f));
  crate->shader->SetUniform("material.diffuse", glm::vec3(1.0f));
  crate->shader->SetUniform("material.specular", glm::vec3(1.0f));
  crate->shader->SetUniform("material.shininess", 60.0f);

  crate->sampler = boost::make_shared<Sampler2D>();

  crate->texture = boost::make_shared<Texture2D>("images/wooden-crate.jpg");
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
    { crate->shader->GetAttributeIndex("TextureCoord"), 3, GL_FLOAT, offsetof(Vertex, textureCoord) }
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

//-----------------------------------------------------
static void Update(unsigned int elapsedMS)
{
  const float elapsedSeconds = (float)elapsedMS / 1000.0f;

  // keyboard input...
  {
    const float unitsPerSecond = 2.0f;
    const Uint8* pressedKeys = SDL_GetKeyboardState(NULL);
    if (pressedKeys[SDL_SCANCODE_W]) { camera->Position += (camera->Forward * unitsPerSecond * elapsedSeconds); }
    if (pressedKeys[SDL_SCANCODE_S]) { camera->Position += (-camera->Forward * unitsPerSecond * elapsedSeconds); }
    if (pressedKeys[SDL_SCANCODE_A]) { camera->Position += (-camera->Right * unitsPerSecond * elapsedSeconds); }
    if (pressedKeys[SDL_SCANCODE_D]) { camera->Position += (camera->Right * unitsPerSecond * elapsedSeconds); }
    if (pressedKeys[SDL_SCANCODE_X]) { camera->Position += (glm::vec3(0,1,0) * unitsPerSecond * elapsedSeconds); }
    if (pressedKeys[SDL_SCANCODE_Z]) { camera->Position += (glm::vec3(0,-1,0) * unitsPerSecond * elapsedSeconds); }

    if (pressedKeys[SDL_SCANCODE_I]) { Device::EnableLight(0, true); }
    if (pressedKeys[SDL_SCANCODE_O]) { Device::EnableLight(0, false); }
  }

  // mouse input...
  {
    const float sensitivity = 0.1f;
    glm::ivec2 position;
    const Uint32 buttons = SDL_GetRelativeMouseState(&position.x, &position.y);
    camera->Yaw += (sensitivity * (float)position.x);
    camera->Pitch += (sensitivity * (float)position.y);
  }

  // update the camera and set the various shader uniforms accordingly...
  {
    camera->Update();
    Device::SetCameraPos(camera->Position);
    Device::SetViewMatrix(camera->ViewMatrix);
    Device::SetProjectionMatrix(camera->PerspectiveMatrix);
  }

  // update the object(s)...
  {
    const float rotationsPerSecond = 90.0f;
    rotationOfDot += rotationsPerSecond * elapsedSeconds;
    if (rotationOfDot > 360.0f) { rotationOfDot -= 360.0f; }
    instances.front().transform = glm::rotate(glm::mat4(1), rotationOfDot, glm::vec3(0,1,0));
  }
}

//-----------------------------------------------------
static void RenderHi(RenderState& renderState)
{
  BOOST_FOREACH(AssetInstance instance, instances)
  {
    const boost::shared_ptr<Asset> asset = instance.asset;

    renderState.vao = asset->vao;
    renderState.shader = asset->shader;

    renderState.textureUnits[0].active = true;
    renderState.textureUnits[0].sampler = asset->sampler;
    renderState.textureUnits[0].texture = asset->texture;

    Device::SetWorldMatrix(instance.transform);

    Device::Draw(asset->drawType, asset->drawFirstVertex, asset->drawVertexCount, renderState);
  }
}

//-----------------------------------------------------
static void Render()
{
  static const ClearState clearState;
  RenderState renderState;

  Device::Clear(clearState, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  RenderHi(renderState);

  Device::SwapBuffers();
}

//-----------------------------------------------------
static void CreateInstances(std::list<AssetInstance>& instances)
{
  AssetInstance instance;

  /* dot over the 'i'*/
  instance.asset = woodenCrate;
  instance.transform = glm::mat4();
  instances.push_back(instance);

  /* i */
  instance.asset = woodenCrate;
  instance.transform = glm::translate(0.0f, -4.0f, 0.0f) * glm::scale(1.0f, 2.0f, 1.0f);
  instances.push_back(instance);

  /* left arm of H */
  instance.asset = woodenCrate;
  instance.transform = glm::translate(-8.0f, 0.0f, 0.0f) * glm::scale(1.0f, 6.0f, 1.0f);
  instances.push_back(instance);

  /* right arm of H */
  instance.asset = woodenCrate;
  instance.transform = glm::translate(-4.0f, 0.0f, 0.0f) * glm::scale(1.0f, 6.0f, 1.0f);
  instances.push_back(instance);

  /* middle bar of H */
  instance.asset = woodenCrate;
  instance.transform = glm::translate(-6.0f, 0.0f, 0.0f) * glm::scale(2.0f, 1.0f, 0.8f);
  instances.push_back(instance);
}
