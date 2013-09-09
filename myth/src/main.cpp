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
#include <textureunit.h>
#include <renderstate.h>
#include <clearstate.h>
#include <device.h>
#include <scenegraph/scene.h>
#include <scenegraph/transformnode.h>
#include <scenegraph/assetnode.h>
#include <world/hellocrates.h>

//-----------------------------------------------------

struct DirectionalLight
{
  glm::vec3 colour;
  float ambient;
};

//-----------------------------------------------------

FILE* debug::logFile;

static boost::shared_ptr<Camera> camera;

static float rotationOfDot = 0.0f;
static float rotationOfLight = 0.0f;

//-----------------------------------------------------

static void Init(const std::string& title, int width, int height, bool fullScreen);
static void ControlCamera();
static boost::shared_ptr<Asset> CreateFloorAsset();

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
  camera->Position = glm::vec3(-15,2,15);
  camera->Yaw(40.0f);
  camera->Pitch(-5.0f);

  boost::shared_ptr<Scene> scene(new Scene(device));
  scene->root = boost::make_shared<SceneNode>();

  boost::shared_ptr<TransformNode> floor(new TransformNode(glm::translate(0.0f,-6.0f,0.0f)));
  boost::shared_ptr<AssetNode> floorAssetNode(new AssetNode(CreateFloorAsset()));
  floor->children.push_back(floorAssetNode);
  scene->root->children.push_back(floor);

  boost::shared_ptr<SceneNode> helloCrates(HelloCrates::Create());
  scene->root->children.push_back(helloCrates);

  scene->Load();
  
  const ClearState clearState;

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

    ControlCamera();
    // update the camera and set the various shader uniforms accordingly...
    camera->Update(elapsedMilliseconds);

    device->shaderVars.CameraPos = camera->Position;
    device->shaderVars.ViewMatrix = camera->ViewMatrix;
    device->shaderVars.ProjectionMatrix = camera->PerspectiveMatrix;

    scene->Update(elapsedMilliseconds);

    device->Clear(clearState, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    scene->Render();

    device->SwapBuffers();
  }

  return 0;
}

//-----------------------------------------------------
static void ControlCamera()
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
static boost::shared_ptr<Asset> CreateFloorAsset()
{
  static const struct Vertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoord;
  } vertices[] =
  {
    { glm::vec3(-100, 0, 100), glm::vec3(0,1,0), glm::vec2(0,0)},
    { glm::vec3( 100, 0, 100), glm::vec3(0,1,0), glm::vec2(1,0)},
    { glm::vec3( 100, 0,-100), glm::vec3(0,1,0), glm::vec2(1,1)},
    { glm::vec3( 100, 0,-100), glm::vec3(0,1,0), glm::vec2(1,1)},
    { glm::vec3(-100, 0,-100), glm::vec3(0,1,0), glm::vec2(0,1)},
    { glm::vec3(-100, 0, 100), glm::vec3(0,1,0), glm::vec2(0,0)}
  };

  boost::shared_ptr<Asset> asset(new Asset());

  asset->drawFirstVertex = 0;
  asset->drawVertexCount = 6;
  asset->drawType = GL_TRIANGLES;

  asset->texture = boost::make_shared<Texture2D>("images/marble_biegetiled_df_.dds");
  asset->texture->Load();

  asset->shader = boost::make_shared<Shader>("shaders/directionallight");
  asset->shader->SetUniform("samplerRepeat", glm::vec2(0.025f));
  asset->shader->SetUniform("sampler", 0);
  asset->shader->SetUniform("specularColour", glm::vec3(0.5f));
  asset->shader->SetUniform("shininess", 5.0f);

  glGenVertexArrays(1, &asset->vao);
  glBindVertexArray(asset->vao);

  asset->vertexBuffer = boost::make_shared<VertexBuffer>(sizeof(vertices), GL_STATIC_DRAW);
  asset->vertexBuffer->Enable();
  asset->vertexBuffer->SetData(vertices, sizeof(vertices), 0);

  struct VertexAttribute
  {
    GLint index;
    GLint componentCount;
    GLenum compoentType;
    size_t offset;
  } attributes[] =
  {
    { asset->shader->GetAttributeIndex("Position"), 3, GL_FLOAT, offsetof(Vertex, position) },
    { asset->shader->GetAttributeIndex("Normal"), 3, GL_FLOAT, offsetof(Vertex, normal) },
    { asset->shader->GetAttributeIndex("TextureCoord"), 2, GL_FLOAT, offsetof(Vertex, textureCoord) }
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

  glBindVertexArray(0);

  // Don't need to keep this state hanging around now...
  VertexBuffer::Disable();

  return asset;
}
