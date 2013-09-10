#include <SDL.h>
#include <SOIL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <stack>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gl_loader/gl_loader.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <debug.h>
#include <shader.h>
#include <buffers.h>
#include <texture.h>
#include <camera.h>
#include <device.h>
#include <vector>
#include <model.h>

//-----------------------------------------------------

FILE* debug::logFile;

static boost::shared_ptr<Camera> camera;

struct Vertex
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 textureCoord;
};

struct VertexAttribute
{
  const char* const name;
  size_t elementCount;
  GLenum elementType;
  size_t offset;
};

//-----------------------------------------------------

static const VertexAttribute ModelMeshAttributes[] =
{
  { "Position", 3, GL_FLOAT, offsetof(Vertex, position) },
  { "Normal", 3, GL_FLOAT, offsetof(Vertex, normal) },
  { "TextureCoord0", 2, GL_FLOAT, offsetof(Vertex, textureCoord) }
};
static const size_t ModelMeshAttributesCount = sizeof(ModelMeshAttributes) / sizeof(ModelMeshAttributes[0]);

//-----------------------------------------------------

static void Init(const std::string& title, int width, int height, bool fullScreen);
static void MoveCamera();

//-----------------------------------------------------

int main(int argc, char* argv[])
{
	debug::logFile = fopen("stderr.txt", "wb");

  boost::shared_ptr<Device> device(new Device());

  if (!device->Initialise("Myth", 1280, 720, false))
  {
    exit(EXIT_FAILURE);
  }

  const float aspectRatio((float)1280/(float)720);
  camera = boost::make_shared<Camera>(45.0f, aspectRatio, 0.1f, 1000.0f);
  camera->Position = glm::vec3(-15,2,50);
  camera->Yaw(0.0f);
  camera->Pitch(0.0f);

  const ClearState clearState;

  Shader shader("assets\\shaders\\simplest");

  Model model;
  model.Load("assets\\models\\phoenix_ugv.md2");

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

    const float elapsedMilliseconds = (float)(now - prevTime);
    prevTime = now;

    MoveCamera();
    camera->Update(elapsedMilliseconds);

    device->Clear(clearState, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Use();
    shader.Update();

    device->shaderVars.CameraPos = camera->Position;
    device->shaderVars.ViewMatrix = camera->ViewMatrix;
    device->shaderVars.ProjectionMatrix = camera->PerspectiveMatrix;

    device->shaderVars.WorldMatrix = glm::translate(0.0f, 0.0f, -8.0f);
    model.Render(device.get());
    
    device->SwapBuffers();
  }

  return 0;
}

//-----------------------------------------------------
static void MoveCamera()
{
  // keyboard input...
  {
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    const float unitsPerSecond = 2.0f;
    if (keyState[SDL_SCANCODE_W]) { camera->MoveForward(); }
    if (keyState[SDL_SCANCODE_S]) { camera->MoveBackward(); }
    if (keyState[SDL_SCANCODE_A]) { camera->MoveLeft(); }
    if (keyState[SDL_SCANCODE_D]) { camera->MoveRight(); }
    if (keyState[SDL_SCANCODE_X]) { camera->MoveUp(); }
    if (keyState[SDL_SCANCODE_Z]) { camera->MoveDown(); }
  }

  // mouse input...
  {
    const float sensitivity = 0.1f;
    glm::ivec2 position;
    const Uint32 buttons = SDL_GetRelativeMouseState(&position.x, &position.y);

    camera->Yaw(sensitivity * (float)position.x);
    camera->Pitch(sensitivity * (float)position.y);
  }
}

//-----------------------------------------------------

