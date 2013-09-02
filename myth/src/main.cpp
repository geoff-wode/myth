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

struct Vertex
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 textureCoord;
};

struct ModelAsset
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

struct ModelInstance
{
  glm::mat4 transform;
  boost::shared_ptr<ModelAsset> asset;
};

//-----------------------------------------------------

FILE* debug::logFile;

static boost::shared_ptr<ModelAsset> woodenCrate;
static std::list<boost::shared_ptr<ModelInstance>> instances;

static boost::shared_ptr<Camera> camera;

static float rotation = 0.0f;

//-----------------------------------------------------

static void Init(const std::string& title, int width, int height, bool fullScreen);
static void MakeCrate();
static void CreateInstances();
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

  MakeCrate();
  CreateInstances();

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
static void MakeCrate()
{
  static Vertex vertices[] =
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

  woodenCrate = boost::make_shared<ModelAsset>();

  woodenCrate->drawType = GL_TRIANGLES;
  woodenCrate->drawFirstVertex = 0;
  woodenCrate->drawVertexCount = sizeof(vertices) / sizeof(vertices[0]);

  glGenVertexArrays(1, &woodenCrate->vao);
  glBindVertexArray(woodenCrate->vao);

  woodenCrate->vertexBuffer = boost::make_shared<VertexBuffer>(sizeof(vertices), GL_STATIC_DRAW);
  woodenCrate->vertexBuffer->Enable();
  woodenCrate->vertexBuffer->SetData(vertices, sizeof(vertices), 0);

  woodenCrate->shader = boost::make_shared<Shader>("shaders/phong");
  woodenCrate->shader->SetUniform("sampler", 0);
  woodenCrate->shader->SetUniform("light.colour", glm::vec3(1.0f));
  woodenCrate->shader->SetUniform("light.position", camera->Position);
  woodenCrate->shader->SetUniform("material.emissive", glm::vec3(0.0f));
  woodenCrate->shader->SetUniform("material.ambient", glm::vec3(0.2f));
  woodenCrate->shader->SetUniform("material.diffuse", glm::vec3(1.0f));
  woodenCrate->shader->SetUniform("material.specular", glm::vec3(0.0f));
  woodenCrate->shader->SetUniform("material.shininess", 0.0f);

  woodenCrate->sampler = boost::make_shared<Sampler2D>();

  woodenCrate->texture = boost::make_shared<Texture2D>("images/wooden-crate.jpg");
  woodenCrate->texture->Load();

  glEnableVertexAttribArray(woodenCrate->shader->GetAttributeIndex("Position"));
  glVertexAttribPointer(
    woodenCrate->shader->GetAttributeIndex("Position"), 3, GL_FLOAT,
    GL_FALSE,
    sizeof(Vertex),
    (const void*)offsetof(Vertex, position));

  glEnableVertexAttribArray(woodenCrate->shader->GetAttributeIndex("Normal"));
  glVertexAttribPointer(
    woodenCrate->shader->GetAttributeIndex("Normal"),
    3, GL_FLOAT,
    GL_FALSE,
    sizeof(Vertex),
    (const void*)offsetof(Vertex, normal));

  glEnableVertexAttribArray(woodenCrate->shader->GetAttributeIndex("TextureCoord"));
  glVertexAttribPointer(
    woodenCrate->shader->GetAttributeIndex("TextureCoord"),
    2, GL_FLOAT,
    GL_FALSE,
    sizeof(Vertex),
    (const void*)offsetof(Vertex, textureCoord));

  VertexBuffer::Disable();
  glBindVertexArray(0);
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

    if (pressedKeys[SDL_SCANCODE_L])
    {
      woodenCrate->shader->SetUniform("light.position", camera->Position);
    }
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
    rotation += rotationsPerSecond * elapsedSeconds;
    if (rotation > 360.0f) { rotation -= 360.0f; }
    instances.front()->transform = glm::rotate(glm::mat4(1), rotation, glm::vec3(0,1,0));
  }
}

//-----------------------------------------------------
static void Render()
{
  static const ClearState clearState;
  RenderState renderState;

  Device::Clear(clearState, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  BOOST_FOREACH(boost::shared_ptr<ModelInstance> instance, instances)
  {
    const boost::shared_ptr<ModelAsset> asset = instance->asset;

    renderState.vao = asset->vao;
    renderState.shader = asset->shader;

    renderState.textureUnits[0].active = true;
    renderState.textureUnits[0].sampler = asset->sampler;
    renderState.textureUnits[0].texture = asset->texture;

    Device::SetWorldMatrix(instance->transform);

    Device::Draw(asset->drawType, asset->drawFirstVertex, asset->drawVertexCount, renderState);
  }

  Device::SwapBuffers();
}

//-----------------------------------------------------
static void CreateInstances()
{
  boost::shared_ptr<ModelInstance> instance;

  /* dot over the 'i'*/
  instance = boost::make_shared<ModelInstance>();
  instance->asset = woodenCrate;
  instance->transform = glm::mat4();
  instances.push_back(instance);

  /* i */
  instance = boost::make_shared<ModelInstance>();
  instance->asset = woodenCrate;
  instance->transform = glm::translate(0.0f, -4.0f, 0.0f) * glm::scale(1.0f, 2.0f, 1.0f);
  instances.push_back(instance);

  /* left arm of H */
  instance = boost::make_shared<ModelInstance>();
  instance->asset = woodenCrate;
  instance->transform = glm::translate(-8.0f, 0.0f, 0.0f) * glm::scale(1.0f, 6.0f, 1.0f);
  instances.push_back(instance);

  /* right arm of H */
  instance = boost::make_shared<ModelInstance>();
  instance->asset = woodenCrate;
  instance->transform = glm::translate(-4.0f, 0.0f, 0.0f) * glm::scale(1.0f, 6.0f, 1.0f);
  instances.push_back(instance);

  /* middle bar of H */
  instance = boost::make_shared<ModelInstance>();
  instance->asset = woodenCrate;
  instance->transform = glm::translate(-6.0f, 0.0f, 0.0f) * glm::scale(2.0f, 1.0f, 0.8f);
  instances.push_back(instance);
}
